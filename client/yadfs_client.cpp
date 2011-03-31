#include "fuse.h"
#include "yadfs_client.hpp"
#include "../commons/client.hpp"
#include "../commons/messages.hpp"

#include <string.h>
#include <errno.h>

using yadfs::ClientConfig;
using yadfs::Client;

ClientConfig *config = NULL;
static Client *client = NULL;

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
  if (client->Connect() < 0)
  {
    return -ENOTCONN; // -ECONNREFUSED;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_GETATTR;
  if (!client->Write(&req_handshake, sizeof(msg_req_handshake)))
  {
    return -EPROTO; // -EILSEQ, -EPROTO
  }

  msg_req_getattr req_getattr;
  strcpy(req_getattr.m_path, path);
  if (!client->Write(&req_getattr, sizeof(msg_req_getattr)))
  {
    return -EPROTO;
  }

  msg_res_getattr res_getattr;
  if (!client->Read(&res_getattr, sizeof(msg_res_getattr)))
  {
    return -EPROTO;
  }
  
  if (res_getattr.m_err != 0)
  {
    return res_getattr.m_err;
  }

  memcpy(stbuf, &res_getattr.m_stat, sizeof(struct stat));
  
  client->Close();

  return 0;
}

int yadfs_readdir_real(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  if (client->Connect() < 0)
  {
    return -ENOTCONN;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_READDIR;
  if (!client->Write(&req_handshake, sizeof(msg_req_handshake)))
  {
    return -EPROTO;
  }

  // Requests the number of entries in the path
  msg_req_readdir req_readdir;
  strcpy(req_readdir.m_path, path);
  if (!client->Write(&req_readdir, sizeof(msg_req_readdir)))
  {
    return -EPROTO;
  }

  // Retrieves the number of entries
  msg_res_readdir res_readdir;
  if (!client->Read(&res_readdir, sizeof(msg_res_readdir)))
  {
    return -EPROTO;
  }

  if (res_readdir.m_children_count < 0)
  {
    return res_readdir.m_children_count;
  }
  
  if (res_readdir.m_children_count == 0)
  {
    return 0;
  }

  for (int i = 0; i < res_readdir.m_children_count; i++)
  {
    // For each entry, reads the dirent
    msg_res_dirent res_dirent;
    if (!client->Read(&res_dirent, sizeof(msg_res_dirent)))
    {
      return -EPROTO;
    }

    // Passes the stat to FUSE
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    st.st_ino = res_dirent.m_dirent.d_ino;
    st.st_mode = res_dirent.m_dirent.d_type << 12;
    if (filler(buf, res_dirent.m_dirent.d_name, &st, 0))
    {
      break;
    }
  }
  
  return 0;
}
