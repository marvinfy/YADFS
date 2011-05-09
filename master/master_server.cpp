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
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using yadfs::FileSystem;
using yadfs::FileSystemEntry;
using yadfs::Logging;

yadfs::MasterServer::MasterServer(const ServerConfig& config) : Server(config),
m_mode(RAID_0)
{
}

yadfs::MasterServer::MasterServer(const MasterServer& orig) : Server(orig)
{
}

yadfs::MasterServer::~MasterServer()
{
}

void yadfs::MasterServer::registerDataNode(const DataNode& node)
{
  m_data_nodes.push_back(node);
}

void *yadfs::MasterServer::Receive(int sockfd)
{
  msg_req_handshake msg_hs;
  if (!Read(sockfd, &msg_hs, sizeof (msg_req_handshake)))
  {
    goto cleanup;
  }

  switch (msg_hs.m_msg_id)
  {
  case MSG_REQ_ECHO:
  {
    msg_req_echo req;
    if (!Read(sockfd, &req, sizeof (msg_req_echo)))
    {
      goto cleanup;
    }

    char *msg = new char[req.m_len];
    if (!Read(sockfd, msg, req.m_len))
    {
      goto cleanup;
    }

    printf("%s", msg);
    delete msg;

    break;
  }
  case MSG_REQ_SHUTDOWN:
  {
    msg_req_shutdown req;
    if (!Read(sockfd, &req, sizeof (msg_req_shutdown)))
    {
      goto cleanup;
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
    if (!Write(sockfd, &res, sizeof (msg_res_shutdown)))
    {
      goto cleanup;
    }

    if (res.m_ok)
    {
      int timeout = req.m_timeout / 1000;
      Logging::log(Logging::INFO, "Server is going to shutdown in %ds",
                   timeout);
      sleep(timeout);
      Stop();
    }
    break;
  }
  case MSG_REQ_GETATTR:
  {
    msg_req_getattr req;
    if (!Read(sockfd, &req, sizeof (msg_req_getattr)))
    {
      goto cleanup;
    }

    // Gets the entry for this path
    string path(req.m_path);
    FileSystemEntry *entry = m_fs.getEntry(path);

    msg_res_getattr res;
    memset(&res, 0, sizeof (msg_res_getattr));
    if (entry == NULL)
    {
      res.m_err = -ENOENT;
    }
    else
    {
      res.m_err = 0;
      memcpy(&res.m_stat, entry->getStat(), sizeof (struct stat));
    }
    if (!Write(sockfd, &res, sizeof (msg_res_getattr)))
    {
      goto cleanup;
    }

    break;
  }
  case MSG_REQ_READDIR:
  {
    // Reads the path of the requested dir
    msg_req_readdir req_readdir;
    if (!Read(sockfd, &req_readdir, sizeof (msg_req_readdir)))
    {
      goto cleanup;
    }

    // Gets the entry for this path
    string path(req_readdir.m_path);
    FileSystemEntry *dir = m_fs.getEntry(path);

    // Responds with the number of children
    msg_res_readdir res_readdir;
    if (dir)
    {
      if (!dir->isDirectory())
      {
        res_readdir.m_children_count = -ENOTDIR;
      }
      else
      {
        res_readdir.m_children_count = dir->getChildrenCount();
      }
    }
    else
    {
      res_readdir.m_children_count = -ENOENT;
    }
    if (!Write(sockfd, &res_readdir, sizeof (msg_res_readdir)))
    {
      goto cleanup;
    }
    if (res_readdir.m_children_count < 0)
    {
      goto cleanup;
    }

    // Writes each entry back to client
    FileSystemEntry *child;
    msg_res_dirent res_dirent;
    for (int i = 0; i < res_readdir.m_children_count; i++)
    {
      child = dir->getChild(i);
      if (child == NULL)
      {
        goto cleanup;
      }

      memset(&res_dirent, 0, sizeof (msg_res_dirent));
      memcpy(&res_dirent.m_dirent, child->getDirent(), sizeof (dirent));
      if (!Write(sockfd, &res_dirent, sizeof (msg_res_dirent)))
      {
        goto cleanup;
      }
    }

    break;
  }
  case MSG_REQ_MKNOD:
  {
    msg_req_mknod req_mknod;
    if (!Read(sockfd, &req_mknod, sizeof (msg_req_mknod)))
    {
      goto cleanup;
    }

    msg_res_mknod res_mknod;
    if (!S_ISREG(req_mknod.m_mode))
    {
      res_mknod.m_err = -EPERM;
    }
    else
    {
      FileSystemEntry *file;
      string path(req_mknod.m_path);

      file = m_fs.getEntry(path);
      if (file)
      {
        res_mknod.m_err = -EEXIST;
      }
      else
      {
        int idx = path.find_last_of('/');

        string fileName = path.substr(idx + 1);
        string parentPath;
        if (idx == 0)
        {
          parentPath = "/";
        }
        else
        {
          parentPath = path.substr(0, idx - 1);
        }

        FileSystemEntry *parent = NULL;
        FileSystemEntry *child = NULL;
        parent = m_fs.getEntry(parentPath);
        child = new FileSystemEntry(0, DT_REG, fileName.c_str());
        if (m_fs.addEntry(parent, child))
        {
          res_mknod.m_err = 0;
        }
        else
        {
          delete child;
          res_mknod.m_err = -ENOENT;
        }
      }
    }

    if (!Write(sockfd, &res_mknod, sizeof (msg_res_mknod)))
    {
      goto cleanup;
    }
    break;
  }
  case MSG_REQ_OPEN:
  {
    msg_req_open req_open;
    if (!Read(sockfd, &req_open, sizeof (msg_req_open)))
    {
      goto cleanup;
    }

    string path(req_open.m_path);
    FileSystemEntry *entry = m_fs.getEntry(path);

    msg_res_open res_open;
    if (entry && !entry->isDirectory())
    {
      res_open.m_err = 0;
    }
    else
    {
      res_open.m_err = -1;
    }

    if (!Write(sockfd, &res_open, sizeof (msg_res_open)))
    {
      goto cleanup;
    }

    /*
    int res;
    res = open(path, fi->flags);
    if (res == -1)
      return -errno;

    close(res);
     */

    break;
  }
  case MSG_REQ_SERVERCONFIG:
  {
    msg_res_serverconfig res_srvcfg;
    res_srvcfg.m_mode = m_mode;
    res_srvcfg.m_node_count = m_data_nodes.size();
    if (!Write(sockfd, &res_srvcfg, sizeof (msg_res_serverconfig)))
    {
      goto cleanup;
    }

    for (int i = 0; i < res_srvcfg.m_node_count; i++)
    {
      msg_res_datanode datanode;

      DataNode node = m_data_nodes[i];
      strcpy(datanode.m_host, node.getHost()->c_str());
      datanode.m_port = node.getPort();
      if (!Write(sockfd, &datanode, sizeof (msg_res_datanode)))
      {
        goto cleanup;
      }
    }

    break;
  }
  case MSG_REQ_GETSIZE:
  {
    msg_req_getsize req_getsize;
    if (!Read(sockfd, &req_getsize, sizeof (msg_req_getsize)))
    {
      goto cleanup;
    }

    string path(req_getsize.m_path);
    FileSystemEntry *entry = m_fs.getEntry(path);

    msg_res_getsize res_getsize;
    if (entry)
    {
      res_getsize.m_ok = true;
      res_getsize.m_size = entry->getSize();
    }
    else
    {
      res_getsize.m_ok = false;
      res_getsize.m_size = 0;
    }

    if (!Write(sockfd, &res_getsize, sizeof (msg_res_getsize)))
    {
      goto cleanup;
    }

    break;
  }
  case MSG_REQ_SETSIZE:
  {
    msg_req_setsize req_setsize;
    if (!Read(sockfd, &req_setsize, sizeof (msg_req_setsize)))
    {
      goto cleanup;
    }

    msg_res_setsize res_setsize;
    string path(req_setsize.m_path);
    FileSystemEntry *entry = m_fs.getEntry(path);
    if (entry)
    {
      entry->setSize(req_setsize.m_size);
      res_setsize.m_ok = true;
    }
    else
    {
      res_setsize.m_ok = false;
    }

    if (!Write(sockfd, &res_setsize, sizeof (msg_res_setsize)))
    {
      goto cleanup;
    }

    break;
  }
  case MSG_REQ_GETID:
  {
    msg_req_getid req_getid;
    if (!Read(sockfd, &req_getid, sizeof (msg_req_getid)))
    {
      goto cleanup;
    }

    msg_res_getid res_getid;
    string path(req_getid.m_path);
    FileSystemEntry *entry = m_fs.getEntry(path);
    if (entry)
    {
      res_getid.m_id = entry->getId();
      res_getid.m_ok = true;
    }
    else
    {
      res_getid.m_ok = false;
    }

    if (!Write(sockfd, &res_getid, sizeof (msg_res_getid)))
    {
      goto cleanup;
    }
    break;
  }
  case MSG_REQ_SENDTIME:
  {
    msg_req_sendtime req_sendtime;
    if (!Read(sockfd, &req_sendtime, sizeof (msg_req_sendtime)))
    {
      goto cleanup;
    }

    yadfs::Logging::log(Logging::INFO, "Operation took %ld ms", req_sendtime.m_time);
    break;
  }

  }

cleanup:

  close(sockfd);
  return NULL;
}

