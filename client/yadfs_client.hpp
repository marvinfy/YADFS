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


#include "../commons/raid_mode.h"
#include "../commons/data_node.hpp"
#include "../commons/client.hpp"
#include <vector>

using std::vector;

namespace yadfs
{

class YADFSClient : public Client
{
private:
  vector<YADFSClient *> m_workers;
  vector<DataNode *> m_nodes;
  Mode m_mode;
  int m_count_cache;

public:
  YADFSClient(const ClientConfig& config) : Client(config), m_count_cache(0)
  {
  }
  virtual ~YADFSClient()
  {
  }
  bool init();
  int getNodeCount()
  {
    return m_count_cache;
  }
  Mode getMode()
  {
    return m_mode;
  }
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

#ifdef	__cplusplus
}
#endif

#endif	/* YADFS_CLIENT_HPP */

