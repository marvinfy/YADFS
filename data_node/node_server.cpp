/* 
 * File:   node_server.cpp
 * Author: marcusviniciusns
 * 
 * Created on April 6, 2011, 6:56 PM
 */

#include "node_server.hpp"

#include "../commons/messages.hpp"
#include <assert.h>
#include <stdio.h>

#include <iostream>
using std::string;

yadfs::NodeServer::NodeServer(const ServerConfig& config) : Server(config)
{
  m_dir = opendir("data");
  assert(m_dir);
}

yadfs::NodeServer::NodeServer(const NodeServer& orig) : Server(orig)
{
}

yadfs::NodeServer::~NodeServer()
{
}

int yadfs::NodeServer::writeFile(unsigned int fileId, unsigned int chunkId,
                                 const char *data, size_t size)
{
  FILE *fd;
  char name[64];
  size_t written;

  sprintf(name, "data/%08d_%016d.bin", fileId, chunkId);
  fd = fopen(name, "w");

  written = fwrite(data, 1, size, fd);
  fclose(fd);

  return 0;
}

int yadfs::NodeServer::readFile(unsigned int fileId, unsigned int chunkId,
                                char *data, size_t size)
{
  FILE *fd;
  char name[64];
  size_t read;

  sprintf(name, "data/%08d_%016d.bin", fileId, chunkId);
  fd = fopen(name, "r");

  read = fread(data, 1, size, fd);
  fclose(fd);

  return read;
}

void *yadfs::NodeServer::Receive(int sockfd)
{
  msg_req_handshake msg_hs;
  if (!Read(sockfd, &msg_hs, sizeof (msg_req_handshake)))
  {
    goto cleanup;
  }

  switch (msg_hs.m_msg_id)
  {
  case MSG_REQ_DELALL:
  {

    msg_req_shutdown req;
    if (!Read(sockfd, &req, sizeof (msg_req_shutdown)))
    {
      goto cleanup;
    }

    msg_res_shutdown res;
    if (!Write(sockfd, &res, sizeof (msg_res_shutdown)))
    {
      goto cleanup;
    }

    break;
  }
  case MSG_REQ_ADDCHUNK:
  {
    msg_req_addchunk req_addchunk;
    if (!Read(sockfd, &req_addchunk, sizeof (msg_req_addchunk)))
    {
      goto cleanup;
    }

    msg_req_addchunk_data req_addchunk_data;
    if (!Read(sockfd, &req_addchunk_data, sizeof (msg_req_addchunk_data)))
    {
      goto cleanup;
    }

    msg_res_addchunk res_addchunk;
    if (writeFile(req_addchunk.m_file_id, req_addchunk.m_chunk_id,
                  req_addchunk_data.m_data, req_addchunk.m_size) == 0)
    {
      res_addchunk.m_ok = true;
    }
    else
    {
      res_addchunk.m_ok = false;
    }

    if (!Write(sockfd, &res_addchunk, sizeof (msg_res_addchunk)))
    {
      goto cleanup;
    }
    break;
  }
  case MSG_REQ_READCHUNK:
  {
    msg_req_readchunk req_readchunk;
    if (!Read(sockfd, &req_readchunk, sizeof (msg_req_readchunk)))
    {
      goto cleanup;
    }

    msg_res_readchunk res_readchunk;
    size_t read;
    read = readFile(req_readchunk.m_file_id, req_readchunk.m_chunk_id,
                    res_readchunk.m_data, CHUNK_SIZE);
    if (read == 0)
    {
      res_readchunk.m_ok = false;
    }
    else
    {
      res_readchunk.m_ok = true;
    }
    res_readchunk.m_read = read;

    if (!Write(sockfd, &res_readchunk, sizeof (msg_res_readchunk)))
    {
      goto cleanup;
    }
    break;
  }
  }

cleanup:
  close(sockfd);
  return NULL;
}



