#include "fuse.h"
#include "job.hpp"
#include "yadfs_client.hpp"
#include "worker.hpp"
#include "worker_pool.hpp"
#include "../commons/chunk.h"
#include "../commons/logging.hpp"
#include "../commons/messages.hpp"

#include <iostream>
#include <string.h>
#include <errno.h>
#include <pthread.h>

using std::cout;
using std::string;
using yadfs::ClientConfig;
using yadfs::YADFSClient;
using yadfs::JobData;
using yadfs::Logging;

ClientConfig *config = NULL;
static YADFSClient *client = NULL;

// -----------------------------------------------------------------------
// C++ Methods
// -----------------------------------------------------------------------

yadfs::YADFSClient::YADFSClient(const ClientConfig& config) : Client(config),
m_nodeCount(0)
{
}

yadfs::YADFSClient::~YADFSClient()
{
}

bool yadfs::YADFSClient::init()
{
  if (Connect() < 0)
  {
    return false;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_SERVERCONFIG;
  if (!client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return false;
  }

  msg_res_serverconfig res_srvcfg;
  if (!client->Read(&res_srvcfg, sizeof (msg_res_serverconfig)))
  {
    return false;
  }

  m_mode = res_srvcfg.m_mode;
  m_nodeCount = res_srvcfg.m_node_count;

  for (int i = 0; i < res_srvcfg.m_node_count; i++)
  {
    msg_res_datanode res_datanode;
    if (!client->Read(&res_datanode, sizeof (msg_res_datanode)))
    {
      return false;
    }

    // TODO: test if data node is online
    ClientConfig config(res_datanode.m_host, res_datanode.m_port);
    Client *client = new Client(config);
    m_node_clients.push_back(client);
  }

  return true;
}

bool yadfs::YADFSClient::enqueueWrite(const char *path, const char *buf,
  size_t size, off_t offset)
{
  if (offset == 0)
  {
    workers_pair pair;
    pair.first = path;
    pair.second = new WorkerPool(m_nodeCount);
    m_workers.insert(pair);
  }

  // Calculates the id of the chunk
  int chunk_id = offset / CHUNK_SIZE;

  if (m_mode == RAID_0)
  {
    // Calculates the id of the working thread (worker_id)
    int worker_id = chunk_id % m_nodeCount;

    JobData *data = new JobData();
    data->m_chunk_id = chunk_id;
    data->m_file_id = 100;
    memcpy(data->m_data, buf, size);
    data->m_size = size;
    data->m_node_client = m_node_clients[worker_id];

    Job *job = new Job(write_func, data);

    // Schedules the job to be done by the #worker_id worker thread
    workers_it it = m_workers.find(path);
    if (it == m_workers.end())
    {
      return false;
    }
    it->second->addJob(worker_id, job);
  }
  else // RAID_1
  {
    //m_workers[0]->addJob(*job);
    //m_workers[1]->addJob(*job);
  }

  // Increments the size of this path by size
  client->incSize(path, size);
  return true;
}

bool yadfs::YADFSClient::releaseWrite(const char *path)
{
  workers_it it = m_workers.find(path);
  if (it == m_workers.end())
  {
    return false;
  }

  // Wait until all job is done
  Logging::log(Logging::INFO, "[YADFSClient::releaseWrite]Waiting complete stop...");
  it->second->stop();
  Logging::log(Logging::INFO, "[YADFSClient::releaseWrite]Stopped");
    
  if (Connect() < 0)
  {
    return false;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_SETSIZE;
  if (!Write(&req_handshake, sizeof(msg_req_handshake)))
  {
    return false;
  }

  msg_req_setsize req_setsize;
  strcpy(req_setsize.m_path, path);
  req_setsize.m_size = getSize(path);

  if (!Write(&req_setsize, sizeof (msg_req_setsize)))
  {
    return false;
  }

  msg_res_setsize res_setsize;
  if (!Read(&res_setsize, sizeof (msg_res_setsize)))
  {
    return false;
  }

  Close();

  if (!res_setsize.m_ok)
  {
    return false;
  }

  // Clears the size entry for this path
  client->remSize(path);

  return true;
}

// -----------------------------------------------------------------------
// C Functions
// -----------------------------------------------------------------------
void write_func(void *data)
{
  JobData *dt = (JobData *)data;

  if (!dt->m_node_client->Connect())
  {
    return;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_ADDCHUNK;
  if (!dt->m_node_client->Write(&req_handshake, sizeof(msg_req_handshake)))
  {
    return;
  }

  msg_req_addchunk req_addchunk;
  req_addchunk.m_file_id = dt->m_file_id;
  req_addchunk.m_chunk_id = dt->m_chunk_id;
  if (!dt->m_node_client->Write(&req_addchunk, sizeof(msg_req_addchunk)))
  {
    return;
  }

  msg_res_addchunk res_addchunk;
  if (!dt->m_node_client->Read(&res_addchunk, sizeof(msg_res_addchunk)))
  {
    return;
  }

  if (!res_addchunk.m_ok)
  {
    Logging::log(Logging::ERROR, "Error adding chunk");
  }
 
  dt->m_node_client->Close();

  delete dt;
}

void read_func(void *data)
{

}

int yadfs_client_init(char *host, int port)
{
  if (config)
  {
    delete config;
  }
  config = new ClientConfig(host, port);


  if (client)
  {
    delete client;
  }
  client = new YADFSClient(*config);

  if (!client->init())
  {

    return -1;
  }

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
  if (!client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return -EPROTO; // -EILSEQ, -EPROTO
  }

  msg_req_getattr req_getattr;
  strcpy(req_getattr.m_path, path);
  if (!client->Write(&req_getattr, sizeof (msg_req_getattr)))
  {
    return -EPROTO;
  }

  msg_res_getattr res_getattr;
  if (!client->Read(&res_getattr, sizeof (msg_res_getattr)))
  {
    return -EPROTO;
  }

  if (res_getattr.m_err != 0)
  {
    return res_getattr.m_err;
  }

  memcpy(stbuf, &res_getattr.m_stat, sizeof (struct stat));

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
  if (!client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return -EPROTO;
  }

  // Requests the number of entries in the path
  msg_req_readdir req_readdir;
  strcpy(req_readdir.m_path, path);
  if (!client->Write(&req_readdir, sizeof (msg_req_readdir)))
  {
    return -EPROTO;
  }

  // Retrieves the number of entries
  msg_res_readdir res_readdir;
  if (!client->Read(&res_readdir, sizeof (msg_res_readdir)))
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
    if (!client->Read(&res_dirent, sizeof (msg_res_dirent)))
    {
      return -EPROTO;
    }

    // Passes the stat to FUSE
    struct stat st;
    memset(&st, 0, sizeof (struct stat));
    st.st_ino = res_dirent.m_dirent.d_ino;
    st.st_mode = res_dirent.m_dirent.d_type << 12;
    if (filler(buf, res_dirent.m_dirent.d_name, &st, 0))
    {

      break;
    }
  }

  return 0;
}

int yadfs_mknod_real(const char *path, mode_t mode, dev_t rdev)
{
  if (client->Connect() < 0)
  {
    return -ENOTCONN;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_MKNOD;
  if (!client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return -EPROTO;
  }

  msg_req_mknod req_mknod;
  strcpy(req_mknod.m_path, path);
  req_mknod.m_mode = mode;
  req_mknod.m_rdev = rdev;
  if (!client->Write(&req_mknod, sizeof (msg_req_mknod)))
  {
    return -EPROTO;
  }

  msg_res_mknod res_mknod;
  if (!client->Read(&res_mknod, sizeof (msg_res_mknod)))
  {

    return -EPROTO;
  }

  return res_mknod.m_err;
}

int yadfs_utimens_real(const char *path, const struct timespec ts[2])
{

  /*
        int res;
        struct timeval tv[2];

        tv[0].tv_sec = ts[0].tv_sec;
        tv[0].tv_usec = ts[0].tv_nsec / 1000;
        tv[1].tv_sec = ts[1].tv_sec;
        tv[1].tv_usec = ts[1].tv_nsec / 1000;

        res = utimes(path, tv);
        if (res == -1)
                return -errno;
   */
  return 0;
}

int yadfs_open_real(const char *path, struct fuse_file_info *fi)
{
  if (client->Connect() < 0)
  {
    return -ENOTCONN;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_OPEN;
  if (!client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return -EPROTO;
  }

  msg_req_open req_open;
  strcpy(req_open.m_path, path);
  memcpy(&req_open.fi, fi, sizeof (struct fuse_file_info));
  if (!client->Write(&req_open, sizeof (msg_req_open)))
  {
    return -EPROTO;
  }

  msg_res_open res_open;
  if (!client->Read(&res_open, sizeof (msg_res_open)))
  {

    return -EPROTO;
  }

  return res_open.m_err;
}

int yadfs_write_real(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
  client->enqueueWrite(path, buf, size, offset);
  return size;
}


int yadfs_read_real(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
  /*
  int fd;
  int res;


  char new_path[512];
  get_new_path(path, new_path, 512);

  (void) fi;
  fd = open(new_path, O_RDONLY);
  if (fd == -1)
    return -errno;

  res = pread(fd, buf, size, offset);
  if (res == -1)
    res = -errno;

  close(fd);*/
  return 0;
}

int yadfs_release_real(const char *path, struct fuse_file_info *fi)
{
  client->releaseWrite(path);
  return 0;
}
