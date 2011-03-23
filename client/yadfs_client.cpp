#include "fuse.h"
#include "yadfs_client.hpp"
#include "../commons/client.hpp"
#include "../commons/messages.hpp"

#include <string.h>
#include <errno.h>

using yadfs::ClientConfig;
using yadfs::Client;

ClientConfig *config = NULL;
Client *client = NULL;

int yadfs_client_init(char *host, int port)
{
  if (config)
  {
    delete config;
  }
  config = new ClientConfig(host, port);
  client = new Client(*config);
  
  return 0;
}

int yadfs_getattr_real(const char *path, struct stat *stbuf)
{
  msg_req_handshake req_handshake;
  msg_req_getattr req_getattr;
  msg_res_getattr res_getattr;

  if (client->Connect() < 0)
  {
    return -ENOTCONN; // -ECONNREFUSED;
  }

  req_handshake.m_msg_id = MSG_REQ_GETATTR;
  if (!client->Write(&req_handshake, sizeof(req_handshake)))
  {
    return -EPROTO; // EILSEQ, EPROTO
  }

  strcpy(req_getattr.m_path, path);
  if (!client->Write(&req_getattr, sizeof(req_getattr)))
  {
    return -EPROTO;
  }

  if (!client->Read(&res_getattr, sizeof(res_getattr)))
  {
    return -EPROTO;
  }
  
  if (res_getattr.m_err != 0)
  {
    return -res_getattr.m_err;
  }

  memcpy(stbuf, &res_getattr.m_stat, sizeof(stbuf));
  
  client->Close();

  return 0;
}

int yadfs_readdir_real(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  /*
  int sockfd;
  msg_req_handshake req_hs;
  msg_req_readdir req_readdir;
  msg_res_readdir res_readdir;
  msg_req_dirent req_dirent;
  msg_res_dirent res_dirent;
  int i;

  sockfd = socket_open();
  if (sockfd < 0)
  {
    return -errno;
  }

  req_hs.m_msg_id = MSG_REQ_READDIR;
  strcpy(&req_readdir.m_path, path);

  write(sockfd, &req_hs, sizeof(req_hs));
  write(sockfd, &req_readdir, sizeof(req_readdir));
  read(sockfd, &res_readdir, sizeof(res_readdir));

  if (res_readdir.m_count < 0)
  {
    return -errno;
  }

  strcpy(&req_dirent.m_path, path);
  for (i = 0; i < res_readdir.m_count; i++)
  {
    req_dirent.m_idx = i;
    write(sockfd, &req_dirent, sizeof(req_dirent));
    read(sockfd, &res_dirent, sizeof(res_dirent));

    struct stat st;
    memset(&st, 0, sizeof(st));

    st.st_ino = res_dirent.m_dirent.d_ino;
    st.st_mode = res_dirent.m_dirent.d_type << 12;
    if (filler(buf, res_dirent.m_dirent.d_name, &st, 0))
    {
      break;
    }
  }

  socket_close(sockfd);
  */
  return 0;
}

/*
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>

int socket_open()
{
  int sockfd;
  struct hostent *server;
  struct sockaddr_in srv_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    // Logging::log(Logging::ERROR, "Cannot open stream socket.");
    return -1;
  }

  server = gethostbyname(host);
  if (server == NULL)
  {
    // Logging::log(Logging::ERROR, "No such host.");
    close(sockfd);
    return -1;
  }

  memset((char *) &srv_addr, 0, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&srv_addr.sin_addr.s_addr,
        server->h_length);
  srv_addr.sin_port = htons(port);

  if (connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0)
  {
    //Logging::log(Logging::ERROR, "Could not connect.");
    close(sockfd);
    return -1;
  }

  return sockfd;
}

static void socket_close(int sockfd)
{
  if (sockfd < 0)
  {
    return;
  }
  close(sockfd);
}

*/