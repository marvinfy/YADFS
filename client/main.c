/* 
 * File:   main.c
 * Author: marcusviniciusns
 *
 * Created on March 17, 2011, 11:16 PM
 */
#define FUSE_USE_VERSION 26

#ifdef linux
#define _XOPEN_SOURCE 500
#endif

#include "../commons/messages.hpp"

#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <sys/types.h>
#include <dirent.h>


static const char* temp = "/home/marcusviniciusns/temp";

static int yadfs_getattr(const char *path, struct stat *stbuf)
{
  int res;

  char path_new[255];
  memset(path_new, 0, 255);
  memcpy(path_new, temp, strlen(temp));
  memcpy(&(path_new[strlen(temp)]), path, strlen(path));

  res = lstat(path_new, stbuf);

  if (res == -1)
  {
    return -errno;
  }

  return 0;
}


static int yadfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  DIR *dp;
  struct dirent *de;

  (void) offset;
  (void) fi;

  char path_new[255];
  memset(path_new, 0, 255);
  memcpy(path_new, temp, strlen(temp));
  memcpy(&(path_new[strlen(temp)]), path, strlen(path));

  dp = opendir(path_new);

  if (dp == NULL)
  {
    return -errno;
  }

  while ((de = readdir(dp)) != NULL) {
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    if (filler(buf, de->d_name, &st, 0))
    {
      break;
    }
  }

  closedir(dp);
  return 0;
}


static struct fuse_operations yadfs_operations = {
  .getattr   = yadfs_getattr,
  .readdir   = yadfs_readdir
};

static int init()
{
  //sleep(5);
  return 1;
}

int main(int argc, char** argv)
{
  int ret;

  ret = init();
  
  if (ret < 0)
  {
    return ret;
  }

  ret = fuse_main(argc, argv, &yadfs_operations, NULL);

  return ret;
}

