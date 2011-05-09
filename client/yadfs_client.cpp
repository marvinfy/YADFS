/*
 * File:   yadfs_client.cpp
 * Author: marcusviniciusns
 *
 * Created on April 1, 2011, 11:06 PM
 */

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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>


using std::cout;
using std::string;
using yadfs::ClientConfig;
using yadfs::YADFSClient;
using yadfs::ReadJobData;
using yadfs::WriteJobData;
using yadfs::Logging;

// -----------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------
static ClientConfig *config = NULL;
static YADFSClient *client = NULL;
static pthread_mutex_t gbl_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t gbl_cond = PTHREAD_COND_INITIALIZER;
static int gbl_waiting_for = -1;
static bool gbl_last_write = false;
static timeval gbl_start;
static timeval gbl_end;

// -----------------------------------------------------------------------
// C++ Methods
// -----------------------------------------------------------------------

yadfs::YADFSClient::YADFSClient(const ClientConfig& config) : Client(config),
m_nodeCount(0)
{
  gbl_waiting_for = -1;
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
  if (m_nodeCount == 0)
  {
    yadfs::Logging::log(Logging::ERROR, "Node count is zero.");
    return false;
  }


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

int yadfs::YADFSClient::read(const char *path, char *buf, size_t size,
                             off_t offset)
{
  FileSystemEntry *entry = getEntry(path);

  if (offset == 0)
  {
    gettimeofday(&gbl_start, 0);

    // Workers map
    workers_pair pair;
    pair.first = path;
    pair.second = new WorkerPool(m_nodeCount);
    m_workers.insert(pair);

    entry->m_id = getId(path);
    entry->m_size = getSize(path);
    entry->m_chunk_count = entry->m_size / CHUNK_SIZE;
    if (entry->m_size % CHUNK_SIZE)
    {
      entry->m_chunk_count++;
    }
    entry->m_data = new msg_res_readchunk*[entry->m_chunk_count];
    memset(entry->m_data, 0,
           entry->m_chunk_count * (sizeof (msg_res_readchunk *)));

    for (int i = 0; i < entry->m_chunk_count; i++)
    {
      // Calculates the id of the working thread (worker_id)
      int worker_id = i % m_nodeCount;

      ReadJobData *data = new ReadJobData();
      data->m_chunk_id = i;
      data->m_file_id = entry->m_id;
      data->m_size = size;
      data->m_node_client = m_node_clients[worker_id];
      data->m_entry = entry;

      Job *job = new Job(read_func, data);

      // Schedules the job to be done by the #worker_id worker thread
      pair.second->addJob(worker_id, job);
    }
  }

  int chunk_id = offset / CHUNK_SIZE;
  if (chunk_id >= entry->m_chunk_count)
  {
    return 0;
  }

  pthread_mutex_lock(&gbl_mutex);
  if (entry->m_data[chunk_id] == NULL)
  {
    gbl_waiting_for = chunk_id;
    Logging::log(Logging::INFO, "[YADFSClient::read]Waiting for chunk_id %d", chunk_id);
    pthread_cond_wait(&gbl_cond, &gbl_mutex);
    Logging::log(Logging::INFO, "[YADFSClient::read]Got chunk_id %d", chunk_id);
  }
  pthread_mutex_unlock(&gbl_mutex);

  msg_res_readchunk *chunk = (msg_res_readchunk *) entry->m_data[chunk_id];
  size_t read = chunk->m_read;
  memcpy(buf, chunk->m_data, size);

  delete entry->m_data[chunk_id];
  entry->m_data[chunk_id] = NULL;

  return read;
}

bool yadfs::YADFSClient::enqueueWrite(const char *path, const char *buf,
                                      size_t size, off_t offset)
{
  FileSystemEntry *entry = getEntry(path);

  if (offset == 0)
  {
    gettimeofday(&gbl_start, 0);

    workers_pair pair;
    pair.first = path;
    pair.second = new WorkerPool(m_nodeCount);
    m_workers.insert(pair);

    entry->m_id = getId(path);
  }

  // Calculates the id of the chunk
  int chunk_id = offset / CHUNK_SIZE;

  if (m_mode == RAID_0)
  {
    // Calculates the id of the working thread (worker_id)
    int worker_id = chunk_id % m_nodeCount;

    WriteJobData *data = new WriteJobData();
    data->m_chunk_id = chunk_id;
    data->m_file_id = entry->m_id;
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
    // Calculates the id of the working thread (worker_id)
    // int worker_id = chunk_id % m_nodeCount;

    WriteJobData *data1, *data2;
    
    data1 = new WriteJobData();
    data2 = new WriteJobData();

    data1->m_chunk_id = chunk_id;
    data2->m_chunk_id = chunk_id;

    data1->m_file_id = entry->m_id;
    data2->m_file_id = entry->m_id;

    memcpy(data1->m_data, buf, size);
    memcpy(data2->m_data, buf, size);

    data1->m_size = size;
    data2->m_size = size;

    data1->m_node_client = m_node_clients[0];
    data2->m_node_client = m_node_clients[1];

    Job *job1 = new Job(write_func, data1);
    Job *job2 = new Job(write_func, data2);

    // Schedules the job to be done by the #worker_id worker thread
    workers_it it = m_workers.find(path);
    if (it == m_workers.end())
    {
      return false;
    }

    it->second->addJob(0, job1);
    it->second->addJob(1, job2);
  }

  // Increments the size of this path by size
  entry->m_size += size;

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
  Logging::log(Logging::INFO, "[YADFSClient::releaseWrite]Stopping...");
  it->second->stop();
  Logging::log(Logging::INFO, "[YADFSClient::releaseWrite]Stopped");

  if (Connect() < 0)
  {
    return false;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_SETSIZE;
  if (!Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return false;
  }

  msg_req_setsize req_setsize;
  strcpy(req_setsize.m_path, path);
  req_setsize.m_size = getEntry(path)->m_size;

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

  // Removes the entry for this path
  client->removeEntry(path);

  return true;
}

bool yadfs::YADFSClient::releaseFiles(const char *path)
{
  for (int i = 0; i < m_nodeCount; i++)
  {
    yadfs::Logging::log(Logging::INFO, "[YADFSClient::releaseFiles]Releasing %d of %d", i + 1, m_nodeCount);

    if (m_node_clients[i]->Connect() < 0)
    {
      return false;
    }

    msg_req_handshake req_handshake;
    req_handshake.m_msg_id = MSG_REQ_RELEASE;
    if (!m_node_clients[i]->Write(&req_handshake, sizeof (msg_req_handshake)))
    {
      return false;
    }

    msg_req_release req_release;
    req_release.m_file_id = getId(string(path));
    if (!m_node_clients[i]->Write(&req_release, sizeof (msg_req_release)))
    {
      return false;
    }

    msg_res_release res_release;
    if (!m_node_clients[i]->Read(&res_release, sizeof (msg_res_release)))
    {
      return false;
    }

    m_node_clients[i]->Close();

    if (!res_release.m_ok)
    {
      return false;
    }
    yadfs::Logging::log(Logging::INFO, "Released");
  }

  yadfs::Logging::log(Logging::INFO, "[YADFSClient::releaseFiles]Removing entry");
  removeEntry(path);

  return true;
}

void yadfs::YADFSClient::sendTime(long time)
{
  if (Connect() < 0)
  {
    return;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_SENDTIME;
  if (!Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return;
  }

  msg_req_sendtime req_sendtime;
  req_sendtime.m_time = time;
  if (!Write(&req_sendtime, sizeof (msg_req_sendtime)))
  {
    return;
  }
  Close();

}


yadfs::FileSystemEntry *yadfs::YADFSClient::getEntry(const string& path)
{
  entries_it it = m_entries.find(path);
  if (it == m_entries.end())
  {
    entries_pair p;
    p.first = path;
    p.second = new FileSystemEntry();
    p.second->m_path = path;
    m_entries.insert(p);
    return p.second;
  }
  return it->second;
}

void yadfs::YADFSClient::removeEntry(const string& path)
{
  entries_it it = m_entries.find(path);
  if (it == m_entries.end())
  {
    return;
  }
  m_entries.erase(it);
}

int yadfs::YADFSClient::getId(const string& path)
{
  if (Connect() < 0)
  {
    return -1;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_GETID;
  if (!client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return -1;
  }

  msg_req_getid req_getid;
  strcpy(req_getid.m_path, path.c_str());
  if (!client->Write(&req_getid, sizeof (msg_req_getid)))
  {
    return -1;
  }

  msg_res_getid res_getid;
  if (!client->Read(&res_getid, sizeof (msg_res_getid)))
  {
    return -1;
  }

  if (!res_getid.m_ok)
  {
    return -1;
  }

  Close();

  return res_getid.m_id;
}

size_t yadfs::YADFSClient::getSize(const string& path)
{
  if (Connect() < 0)
  {
    return -1;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_GETSIZE;
  if (!client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return -1;
  }
  msg_req_getsize req_getsize;
  strcpy(req_getsize.m_path, path.c_str());
  if (!client->Write(&req_getsize, sizeof (msg_req_getsize)))
  {
    return -1;
  }
  msg_res_getsize res_getsize;
  if (!client->Read(&res_getsize, sizeof (msg_res_getsize)))
  {
    return -1;
  }
  if (!res_getsize.m_ok)
  {
    return -1;
  }
  Close();

  return res_getsize.m_size;
}

// -----------------------------------------------------------------------
// C Functions
// -----------------------------------------------------------------------

void write_func(void *data)
{
  WriteJobData *dt = (WriteJobData *) data;

  if (!dt->m_node_client->Connect())
  {
    return;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_ADDCHUNK;
  if (!dt->m_node_client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return;
  }

  msg_req_addchunk req_addchunk;
  req_addchunk.m_file_id = dt->m_file_id;
  req_addchunk.m_chunk_id = dt->m_chunk_id;
  req_addchunk.m_size = dt->m_size;
  if (!dt->m_node_client->Write(&req_addchunk, sizeof (msg_req_addchunk)))
  {
    return;
  }

  if (!dt->m_node_client->Write(&dt->m_data, sizeof (msg_req_addchunk_data)))
  {
    return;
  }

  msg_res_addchunk res_addchunk;
  if (!dt->m_node_client->Read(&res_addchunk, sizeof (msg_res_addchunk)))
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
  ReadJobData *dt = (ReadJobData *) data;

  if (dt->m_node_client->Connect() < 0)
  {
    return;
  }

  msg_req_handshake req_handshake;
  req_handshake.m_msg_id = MSG_REQ_READCHUNK;
  if (!dt->m_node_client->Write(&req_handshake, sizeof (msg_req_handshake)))
  {
    return;
  }

  msg_req_readchunk req_readchunk;
  req_readchunk.m_file_id = dt->m_file_id;
  req_readchunk.m_chunk_id = dt->m_chunk_id;
  if (!dt->m_node_client->Write(&req_readchunk, sizeof (msg_req_readchunk)))
  {
    return;
  }

  msg_res_readchunk *res_readchunk = new msg_res_readchunk;
  if (!dt->m_node_client->Read(res_readchunk, sizeof (msg_res_readchunk)))
  {
    return;
  }
  if (!res_readchunk->m_ok)
  {
    return;
  }


  yadfs::FileSystemEntry *entry = dt->m_entry;

  pthread_mutex_lock(&gbl_mutex);
  entry->m_data[dt->m_chunk_id] = res_readchunk;
  if (gbl_waiting_for == dt->m_chunk_id)
  {
    gbl_waiting_for = -1;
    yadfs::Logging::log(Logging::INFO, "Broadcasting condition for chunk_id %d", dt->m_chunk_id);
    pthread_cond_broadcast(&gbl_cond);
  }
  pthread_mutex_unlock(&gbl_mutex);

  dt->m_node_client->Close();

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
  gbl_last_write = true;
  client->enqueueWrite(path, buf, size, offset);
  return size;
}

int yadfs_read_real(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
  gbl_last_write = false;
  return client->read(path, buf, size, offset);
}

int yadfs_release_real(const char *path, struct fuse_file_info *fi)
{
  long mtime, seconds, useconds;

  if (gbl_last_write)
  {
    client->releaseWrite(path);
  }

  client->releaseFiles(path);

  gettimeofday(&gbl_end, 0);
  seconds = gbl_end.tv_sec - gbl_start.tv_sec;
  useconds = gbl_end.tv_usec - gbl_start.tv_usec;
  mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

  yadfs::Logging::log(Logging::INFO, "Operation took %ld ms", mtime);
  yadfs::Logging::log(Logging::INFO, "Operation took %ld sec %ld ms", seconds, useconds/1000.0);

  client->sendTime(mtime);

  return 0;
}
