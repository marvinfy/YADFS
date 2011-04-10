/* 
 * File:   server.cpp
 * Author: marcusviniciusns
 * 
 * Created on March 10, 2011, 9:47 PM
 */

#include "logging.hpp"
#include "server.hpp"

#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

using yadfs::Logging;

typedef struct _receive_data {
  yadfs::Server *m_server;
  int m_sockfd;
} receive_data;

void *yadfs::Server::Receive(void *data)
{
  receive_data *rd = (receive_data *)data;
  void *ret = rd->m_server->Receive(rd->m_sockfd);
  delete rd;
  
  return ret;
}

yadfs::Server::Server(const ServerConfig& config) : m_running(false), m_sockfd(-1), m_config(config)
{
}

yadfs::Server::Server(const Server& orig)
{
}

yadfs::Server::~Server()
{
}

int yadfs::Server::Start()
{
  int sockfd, len;
  sockaddr_in srv_addr, cli_addr;
  int tries;
  pthread_t thread;

  m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_sockfd < 0)
  {
    Logging::log(Logging::ERROR, "Cannot open stream socket.");
    return -1;
  }

  memset((char *) &srv_addr, 0, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  srv_addr.sin_port = htons(m_config.m_port);

  for (tries = 1;;)
  {
    if(bind(m_sockfd, (sockaddr *)&srv_addr, sizeof(srv_addr)) == 0)
    {
      break;
    }

    Logging::log(Logging::ERROR, "Cannot bind local address - #%d attempt.", tries);
    
    if (tries++ >= m_config.m_retries)
    {
      close(m_sockfd);
      return -1;
    }

    sleep(1);
  }

  Logging::log(Logging::INFO, "Server started");
  m_running = true;

  listen(m_sockfd, 5);
  len = sizeof(cli_addr);
  
  while (m_running)
  {
    int sockfd = accept(m_sockfd, (sockaddr *) &cli_addr, (socklen_t *) &len);
    if (sockfd < 0)
    {
      Logging::log(Logging::ERROR, "Accept error.");
      continue;
    }

    receive_data *rd = new receive_data;
    rd->m_server = this;
    rd->m_sockfd = sockfd;

    if (m_config.m_mode == MULTI_THREADED)
    {
      pthread_create(&thread, NULL, yadfs::Server::Receive, (void *)rd);
    }
    else // SINGLE_THREADED
    {
      yadfs::Server::Receive((void *)rd);
    }
  }

  return 0;
}

int yadfs::Server::Stop()
{
  m_running = false;
  close(m_sockfd);
  return 0;
}

void *yadfs::Server::Receive(int sockfd)
{
  return 0;
}

bool yadfs::Server::Read(int sockfd, void* data, int len)
{
  if (sockfd < 0)
  {
    Logging::log(Logging::ERROR, "Can not read from socket (fd=%d)", sockfd);
    return false;
  }

  int _read = read(sockfd, data, len);
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

bool yadfs::Server::Write(int sockfd, void* data, int len)
{
  if (sockfd < 0)
  {
    Logging::log(Logging::ERROR, "Can not write to socket (fd=%d).", sockfd);
    return false;
  }

  if (write(sockfd, data, len) == len)
  {
    return true;
  }

  Logging::log(Logging::ERROR, "Failed to send packet.");
  return false;
}


