/* 
 * File:   main.c
 * Author: marcusviniciusns
 *
 * Created on March 17, 2011, 11:16 PM
 */

#include "fuse.h"
#include "../commons/utils.hpp"
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

static int yadfs_mknod(const char *path, mode_t mode, dev_t rdev)
{
  return yadfs_mknod_real(path, mode, rdev);
}

static int yadfs_utimens(const char *path, const struct timespec ts[2])
{
  return yadfs_utimens_real(path, ts);
}

static int yadfs_open(const char *path, struct fuse_file_info *fi)
{
  return yadfs_open_real(path, fi);
}

static int yadfs_write(const char *path, const char *buf, size_t size,
                       off_t offset, struct fuse_file_info *fi)
{
  return yadfs_write_real(path, buf, size, offset, fi);
}

static int yadfs_read(const char *path, char *buf, size_t size,
                      off_t offset, struct fuse_file_info *fi)
{
  return yadfs_read_real(path, buf, size, offset, fi);
}

static int yadfs_release(const char *path, struct fuse_file_info *fi)
{
  return yadfs_release_real(path, fi);
}

static struct fuse_operations yadfs_operations = {
  .getattr = yadfs_getattr,
  .readdir = yadfs_readdir,
  .mknod = yadfs_mknod,
  .utimens = yadfs_utimens,
  .open = yadfs_open,
  .write = yadfs_write,
  .read = yadfs_read,
  .release = yadfs_release
};

int main(int argc, char* argv[])
{
  int ret, nPort;
  char host[128], port[8];

  sleep(15);

  parse_argv(argc, argv);

  ret = get_parameter("master.host", host, 128);
  if (ret == -1)
  {
    strcpy(host, "localhost");
  }

  ret = get_parameter("master.port", port, 8);
  if (ret == -1)
  {
    strcpy(port, "10005");
  }

  nPort = atoi(port);

  ret = yadfs_client_init(host, nPort);
  if (ret != 0)
  {
    return ret;
  }

  ret = fuse_main(argc, argv, &yadfs_operations, NULL);

  return ret;
}
