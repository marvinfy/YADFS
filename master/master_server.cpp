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
  if (!Read(sockfd, &msg_hs, sizeof(msg_hs)))
  {
    return NULL;
  }

  switch (msg_hs.m_msg_id)
  {
    case MSG_REQ_ECHO:
    {
      msg_req_echo req;
      if (!Read(sockfd, &req, sizeof(req)))
      {
        return NULL;
      }
      
      char *msg = new char[req.m_len];
      if (!Read(sockfd, msg, req.m_len))
      {
        return NULL;
      }
      
      printf("%s", msg);
      delete msg;
      
      break;
    }
    case MSG_REQ_SHUTDOWN:
    {
      msg_req_shutdown req;
      if (!Read(sockfd, &req, sizeof(req)))
      {
        return NULL;
      }

      msg_res_shutdown res;
      if (strcmp(req.m_user, m_config.m_user) == 0 &&
          strcmp(req.m_pass, m_config.m_pass) == 0)
      {
        res.m_ok = true;
      }
      else
      {
        res.m_ok = false;
      }
      if (!Write(sockfd, &res, sizeof(res)))
      {
        return NULL;
      }
      
      if (res.m_ok)
      {
        int timeout = req.m_timeout / 1000;
        Logging::log(Logging::INFO, "Server is going to shutdown in %ds", timeout);
        sleep(timeout);
        Stop();
      }
      break;
    }
    case MSG_REQ_GETATTR:
    {
      msg_req_getattr req;
      if (!Read(sockfd, &req, sizeof(req)))
      {
        return NULL;
      }
      // Ignore path for now..

      msg_res_getattr res;
      memset(&res, 0, sizeof(res));
      res.m_err = 0;
      res.m_stat.st_mode = S_IFDIR | 0755;
      res.m_stat.st_nlink = 3;
      res.m_stat.st_size = 4096;
      write(sockfd, &res, sizeof(res));
      
      break;
    }
    case MSG_REQ_READDIR:
    {
      /*
      msg_req_readdir req_readdir;
      msg_res_readdir res_readdir;
      msg_req_dirent req_dirent;
      msg_res_dirent res_dirent;

      read(sockfd, &req_readdir, sizeof(req_readdir));

      // Ignore path for instance..

      res_readdir.m_count = 3;
      write(sockfd, &res_readdir, sizeof(res_readdir));


      read(sockfd, &req_dirent, sizeof(req_dirent)); // 0
      read(sockfd, )
      */


      break;
    }
  }

  return NULL;
}

