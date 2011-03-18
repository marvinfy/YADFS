/* 
 * File:   MasterServer.cpp
 * Author: marcusviniciusns
 * 
 * Created on 14 de Março de 2011, 23:57
 */

#include "master_server.hpp"
#include "../commons/logging.hpp"
#include "../commons/messages.hpp"

#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdio.h>

using yadfs::Logging;

yadfs::MasterServer::MasterServer(const ServerConfig& config) : Server(config)
{
  
}

yadfs::MasterServer::MasterServer(const MasterServer& orig) : Server(orig)
{
}

yadfs::MasterServer::~MasterServer()
{
}

void *yadfs::MasterServer::Receive(int sockfd)
{
  msg_req_handshake msg_hs;

  memset(&msg_hs, 0, sizeof(msg_hs));
  read(sockfd, &msg_hs, sizeof(msg_hs));

  switch (msg_hs.m_msg_id)
  {
    case MSG_REQ_ECHO:
    {
      msg_req_echo req;
      read(sockfd, &req, sizeof(req));
      char *msg = new char[req.m_len];
      read(sockfd, msg, req.m_len);
      printf("%s", msg);
      delete msg;
      
      break;
    }
    case MSG_REQ_SHUTDOWN:
    {
      msg_req_shutdown req;
      msg_res_shutdown res;
      memset(&req, 0, sizeof(req));
      memset(&res, 0, sizeof(res));

      read(sockfd, &req, sizeof(req));
      if (strcmp(req.m_user, m_config.m_user) == 0 &&
          strcmp(req.m_pass, m_config.m_pass) == 0)
      {
        res.m_ok = true;
      }
      else
      {
        res.m_ok = false;
      }

      write(sockfd, &res, sizeof(res));
      if (res.m_ok)
      {
        int timeout = req.m_timeout / 1000;
        Logging::log(Logging::INFO, "Server is going to shutdown in %ds", timeout);
        sleep(timeout);
        Stop();
      }
      break;
    }
  }

  return 0;
}

