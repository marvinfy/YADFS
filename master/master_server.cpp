/* 
 * File:   MasterServer.cpp
 * Author: marcusviniciusns
 * 
 * Created on 14 de Março de 2011, 23:57
 */

#include "master_server.hpp"
#include "fs.hpp"
#include "../commons/logging.hpp"
#include "../commons/messages.hpp"

#include <dirent.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using yadfs::FileSystem;
using yadfs::FileSystemEntry;
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
      if (m_config.m_verbose)
      {
        Logging::log(Logging::INFO, "[MSG_REQ_GETATTR]Receiving started");
      }

      msg_req_getattr req;
      if (!Read(sockfd, &req, sizeof(req)))
      {
        return NULL;
      }
      if (m_config.m_verbose)
      {
        Logging::log(Logging::INFO, "[MSG_REQ_GETATTR]Path: %s", req.m_path);
      }
      // req.m_path - ignore for now..

      msg_res_getattr res;
      memset(&res, 0, sizeof(res));
      res.m_err = 0;
      res.m_stat.st_mode = S_IFDIR | 0755;
      res.m_stat.st_nlink = 3;
      res.m_stat.st_size = 4096;
      if (!Write(sockfd, &res, sizeof(res)))
      {
        return NULL;
      }

      if (m_config.m_verbose)
      {
        Logging::log(Logging::INFO, "[MSG_REQ_GETATTR]OK", req.m_path);
      }
      
      break;
    }
    case MSG_REQ_READDIR:
    {
      // Reads the path of the requested dir
      msg_req_readdir req_readdir;
      if (!Read(sockfd, &req_readdir, sizeof(req_readdir)))
      {
        return NULL;
      }

      // Gets the entry for this path
      FileSystemEntry *dir = m_fs.getEntry(req_readdir.m_path);

      // Responds with the number of children
      msg_res_readdir res_readdir;
      if (dir)
      {
        res_readdir.m_children_count = dir->getChildrenCount();
      }
      else
      {
        res_readdir.m_children_count = -1;
      }
      if (!Write(sockfd, &res_readdir, sizeof(res_readdir)))
      {
        return NULL;
      }
      if (res_readdir.m_children_count == -1)
      {
        return NULL;
      }

      // Writes each entry back to client
      for (int i = 0; i < res_readdir.m_children_count; i++)
      {
        FileSystemEntry *child = dir->getChild(i);
        if (child == NULL)
        {
          return NULL;
        }

        msg_res_dirent res_dirent;
        memcpy(&res_dirent.m_dirent, child->getDirent(), sizeof(dirent));
        if (!Write(sockfd, &res_dirent, sizeof(res_dirent)))
        {
          return NULL;
        }
      }

      break;
    }
  }

  return NULL;
}

