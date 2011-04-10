/* 
 * File:   node_server.cpp
 * Author: marcusviniciusns
 * 
 * Created on April 6, 2011, 6:56 PM
 */

#include "node_server.hpp"

#include "../commons/messages.hpp"

#include <iostream>
using std::cout;

yadfs::NodeServer::NodeServer(const ServerConfig& config) : Server(config)
{
}

yadfs::NodeServer::NodeServer(const NodeServer& orig) : Server(orig)
{
}

yadfs::NodeServer::~NodeServer()
{
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

    cout << req_addchunk.m_file_id << " " << req_addchunk.m_chunk_id << "\n";

    msg_res_addchunk res_addchunk;
    res_addchunk.m_ok = true;
    if (!Write(sockfd, &res_addchunk, sizeof (msg_res_addchunk)))
    {
      goto cleanup;
    }
  }

  }
  
cleanup:
  close(sockfd);
  return NULL;
}



