/* 
 * File:   main.c
 * Author: marcusviniciusns
 *
 * Created on March 17, 2011, 11:16 PM
 */

#include <string.h>

#include "fuse.h"
#include "yadfs_client.hpp"

static int yadfs_getattr(const char *path, struct stat *stbuf)
{
  return yadfs_getattr_real(path, stbuf);
}

static int yadfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  return yadfs_readdir_real(path, buf, filler, offset, fi);
}

static struct fuse_operations yadfs_operations = {
  .getattr   = yadfs_getattr,
  .readdir   = yadfs_readdir
};

int main(int argc, char* argv[])
{
  int ret;
  
  ret = yadfs_client_init("192.168.1.4", 10005);
  if (ret != 0)
  {
    return ret;
  }

  sleep(10);
  
  ret = fuse_main(argc, argv, &yadfs_operations, NULL);
  
  return ret;
}
