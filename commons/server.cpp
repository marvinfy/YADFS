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
using yadfs::Logging;

yadfs::Server::Server(const ServerConfig& config) : m_running(false), m_config(config)
{
}

yadfs::Server::Server(const Server& orig)
{
}

yadfs::Server::~Server()
{
}

int yadfs::Server::start()
{
  int sockfd, newsockfd, len;
  sockaddr_in srv_addr, cli_addr;
  int tries;
  pthread_t thread;
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
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
    if(bind(sockfd, (sockaddr *)&srv_addr, sizeof(srv_addr)) == 0)
    {
      break;
    }

    Logging::log(Logging::ERROR, "Cannot bind local address - #%d attempt.", tries);
    
    if (tries++ >= m_config.m_retries)
    {
      close(sockfd);
      return -1;
    }

    sleep(1);
  }

  Logging::log(Logging::INFO, "Server started");
  m_running = true;

  listen(sockfd, 5);
  len = sizeof(cli_addr);
  
  while (m_running)
  {
    newsockfd = accept(sockfd, (sockaddr *) &cli_addr, (socklen_t *) &len);
    if(newsockfd < 0)
    {
      Logging::log(Logging::ERROR, "Accept error.");
      continue;
    }
    
    pthread_create(&thread, NULL, yadfs::Server::receive, (void *)&newsockfd);
  }

  return 0;
}

int yadfs::Server::stop()
{
  m_running = false;
  return 0;
}

void *yadfs::Server::receive(void *data)
{

}
