/*
 * File:   client.cpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 9:47 PM
 */

#include "client.hpp"

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


#include "logging.hpp"

using yadfs::Logging;

yadfs::Client::Client(const ClientConfig& config) :
m_sockfd(-1), m_config(config)
{
}

yadfs::Client::Client(const Client& orig)
{
}

yadfs::Client::~Client()
{
}

int yadfs::Client::Connect()
{
  hostent *server;
  sockaddr_in srv_addr;

  m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_sockfd < 0)
  {
    Logging::log(Logging::ERROR, "Cannot open stream socket.");
    return -1;
  }

  server = gethostbyname(m_config.m_host.c_str());
  if (server == NULL)
  {
    Logging::log(Logging::ERROR, "No such host.");
    close(m_sockfd);
    return -1;
  }

  memset((char *) &srv_addr, 0, sizeof (srv_addr));
  srv_addr.sin_family = AF_INET;
  bcopy((char *) server->h_addr,
        (char *) &srv_addr.sin_addr.s_addr,
        server->h_length);
  srv_addr.sin_port = htons(m_config.m_port);

  /*
  unsigned int tries = 0;
  int status = -1;
  while (42)
  {
    status = connect(m_sockfd, (sockaddr *) & srv_addr, sizeof (srv_addr));
    if (status != 0)
    {
        tries++;
        Logging::log(Logging::ERROR, "[CLIENT]Could not connect. Server %s. #%d try", m_config.m_host.c_str(), tries);
        sleep(1);

        if (tries >= 100)
        {
            Logging::log(Logging::ERROR, "Fatal: could not connect after 100 tries");
            close(m_sockfd);
            return -1;
        }
    }
    else
    {
        break;
    }
  }
   */

  int status = connect(m_sockfd, (sockaddr *) & srv_addr, sizeof (srv_addr));
  if (status != 0)
  {
    Logging::log(Logging::ERROR, "Fatal: could not connect to %s", m_config.m_host.c_str());
    close(m_sockfd);
    return -1;
  }


  return m_sockfd;
}

bool yadfs::Client::Write(void *data, int len)
{
  if (m_sockfd < 0)
  {
    Logging::log(Logging::ERROR, "Can not write to socket (fd=%d).", m_sockfd);
    return false;
  }

  /*
  if (len == 4096)
  {
    printf("[CLIENT]Writting to socket:\n");
    for (int i = 0; i < 4096; i++)
    {
      printf("%c", ((char*)data)[i]);
    }
    printf("\n[CLIENT]End\n");
  }
   */


  if (write(m_sockfd, data, len) == len)
  {
    return true;
  }

  Logging::log(Logging::ERROR, "[CLIENT]Failed to send packet.");
  return false;
}

bool yadfs::Client::Read(void *data, int len)
{
  if (m_sockfd < 0)
  {
    Logging::log(Logging::ERROR, "Can not read from socket (fd=%d)", m_sockfd);
    return false;
  }


  char *buf = (char *)data;
  int _read_tmp;
  int _read = 0;
  int _left = len;
  while (_read < len)
  {
    _read_tmp = read(m_sockfd, (void *)(&buf[_read]), _left);

    // Error
    if (_read_tmp < 0)
    {
      break;
    }

    _read += _read_tmp;

    // Every byte was read
    if (_read == len)
    {
      break;
    }

    // Remaining bytes to read
    _left -= _read_tmp;
  }

  /*
  int _read_tmp, _read = 0, _left = len;
  while (_read < len)
  {
    _read_tmp = read(m_sockfd, data, _left);
    if (_read_tmp < 0)
    {
      break;
    }
    _read += _read_tmp;
    _left -= _read_tmp;
  }*/

  /*
  if (len == 4096)
  {
    printf("[CLIENT]Read from socket:\n");
    for (int i = 0; i < 4096; i++)
    {
      printf("%c", ((char*)data)[i]);
    }
    printf("\n[CLIENT]FIM\n\n");
  }
   */


  if (_read == len)
  {
    return true;
  }

  if (_read == 0)
  {
    Logging::log(Logging::ERROR, "EOF caught while receiving packet.");
    return false;
  }

  if (_read == -1)
  {
    Logging::log(Logging::ERROR, "Unknown error while receiving packet.");
    return false;
  }

  Logging::log(Logging::ERROR, "Invalid packet size. Expected %d. Read %d.",
               len, _read);
  return false;
}

void yadfs::Client::Close()
{
  if (m_sockfd < 0)
  {
    return;
  }
  close(m_sockfd);
  m_sockfd = -1;
}
