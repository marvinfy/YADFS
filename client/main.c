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

static int yadfs_getattr(const char *path, struct stat *stbuf)
{
  int res;

  res = lstat(path, stbuf);

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

	dp = opendir(path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static struct fuse_operations yadfs_operations = {
    .getattr   = yadfs_getattr,
    .readdir   = yadfs_readdir
};

/*
 * 
 */
int main(int argc, char** argv)
{
  int ret;
  ret = fuse_main(argc, argv, &yadfs_operations, NULL);

  return ret;
}

