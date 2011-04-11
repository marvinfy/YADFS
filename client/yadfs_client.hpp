/* 
 * File:   yadfs_client.hpp
 * Author: marcusviniciusns
 *
 * Created on March 22, 2011, 7:31 PM
 */
#ifndef YADFS_CLIENT_HPP
#define	YADFS_CLIENT_HPP

#include <fuse.h>

#ifdef	__cplusplus

#include "job.hpp"
#include "worker.hpp"
#include "worker_pool.hpp"
#include "../commons/raid_mode.h"
#include "../commons/data_node.hpp"
#include "../commons/chunk.h"
#include "../commons/client.hpp"

#include <assert.h>
#include <map>
#include <vector>
#include <pthread.h>

using std::map;
using std::pair;
using std::vector;


namespace yadfs
{

class YADFSClient;
class FileSystemEntry;

typedef map<string, FileSystemEntry *> entries_map;
typedef entries_map::iterator entries_it;
typedef pair<string, FileSystemEntry *> entries_pair;

typedef map<string, WorkerPool *> workers_map;
typedef workers_map::iterator workers_it;
typedef pair<string, WorkerPool *> workers_pair;

class FileSystemEntry
{
  friend class YADFSClient;
private:
  string m_path;
  size_t m_size;
  unsigned int m_id;
public:

  FileSystemEntry() : m_size(0)
  {
  }
};

class YADFSClient : public Client
{
private:
  unsigned int m_nodeCount;

  // vector<Worker *> m_workers;
  workers_map m_workers;
  vector<Client *> m_node_clients;

  entries_map m_entries;
  Mode m_mode;

  pthread_mutex_t m_mutex;
  pthread_cond_t m_cond;

  // int m_waiting_count;
  // string m_path_to_release;
  // pthread_mutex_t m_mutex;

  void releaseWrite(void *instance);
public:
  YADFSClient(const ClientConfig& config);
  virtual ~YADFSClient();
  bool init();
  bool enqueueWrite(const char *path, const char *buf, size_t size,
                    off_t offset);
  bool releaseWrite(const char *path);
  FileSystemEntry *getEntry(const string& path);
  void removeEntry(const string& path);

};

class JobData
{
public:
  unsigned int m_file_id;
  unsigned int m_chunk_id;
  char m_path[256];
  char m_data[CHUNK_SIZE];
  size_t m_size;
  Client *m_node_client;
};

}

extern "C"
{
#endif

int yadfs_client_init(char *host, int port);
int yadfs_getattr_real(const char *path, struct stat *stbuf);
int yadfs_readdir_real(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi);
int yadfs_mknod_real(const char *path, mode_t mode, dev_t rdev);
int yadfs_utimens_real(const char *path, const struct timespec ts[2]);
int yadfs_open_real(const char *path, struct fuse_file_info *fi);
int yadfs_write_real(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi);
int yadfs_read_real(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi);
int yadfs_release_real(const char *path, struct fuse_file_info *fi);

void write_func(void *data);
void read_func(void *data);

#ifdef	__cplusplus
}
#endif

#endif	/* YADFS_CLIENT_HPP */

