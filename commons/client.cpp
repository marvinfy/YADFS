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

yadfs::Client::Client(const ClientConfig& config) : m_sockfd(-1), m_config(config)
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

  memset((char *) &srv_addr, 0, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&srv_addr.sin_addr.s_addr,
        server->h_length);
  srv_addr.sin_port = htons(m_config.m_port);

  if (connect(m_sockfd, (sockaddr *)&srv_addr, sizeof(srv_addr)) < 0)
  {  
    Logging::log(Logging::ERROR, "Could not connect.");
    close(m_sockfd);
    return -1;
  }

  return m_sockfd;
}

bool yadfs::Client::Write(void *data, int len)
{
  if (m_sockfd < 0)
  {
    return false;
  }
  
  if (write(m_sockfd, data, len) == len)
  {
    return true;
  }
  
  return false;
}

bool yadfs::Client::Read(void *data, int len)
{
  if (m_sockfd < 0)
  {
    return false;
  }

  if (read(m_sockfd, data, len) == len)
  {
    return true;
  }

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
