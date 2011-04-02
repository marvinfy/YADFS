/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags --libs` fusexmp.c -o fusexmp
 */

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static const char *origem = "/home/marcusviniciusns/dummy_origem";

static void get_new_path(const char *path, char *new_path, size_t size)
{
  memset(new_path, 0, size);
  strcpy(new_path, origem);
  strcpy(&new_path[strlen(origem)], path);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;

  char new_path[512];
  get_new_path(path, new_path, 512);

  res = lstat(new_path, stbuf);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
  DIR *dp;
  struct dirent *de;

  (void) offset;
  (void) fi;

  char new_path[512];
  get_new_path(path, new_path, 512);

  dp = opendir(new_path);
  if (dp == NULL)
    return -errno;

  while ((de = readdir(dp)) != NULL)
  {
    struct stat st;
    memset(&st, 0, sizeof (st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    if (filler(buf, de->d_name, &st, 0))
      break;
  }

  closedir(dp);
  return 0;
}

static int xmp_access(const char *path, int mask)
{
  int res;

  char new_path[512];
  get_new_path(path, new_path, 512);

  res = access(new_path, mask);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
  int res;

  char new_path[512];
  get_new_path(path, new_path, 512);

  res = readlink(new_path, buf, size - 1);
  if (res == -1)
    return -errno;

  buf[res] = '\0';
  return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
  int res;

  /* On Linux this could just be 'mknod(path, mode, rdev)' but this
     is more portable */
  char new_path[512];
  get_new_path(path, new_path, 512);


  if (S_ISREG(mode))
  {
    res = open(new_path, O_CREAT | O_EXCL | O_WRONLY, mode);
    if (res >= 0)
      res = close(res);
  }
  else if (S_ISFIFO(mode))
    res = mkfifo(new_path, mode);
  else
    res = mknod(new_path, mode, rdev);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
  int res;

  char new_path[512];
  get_new_path(path, new_path, 512);

  res = mkdir(new_path, mode);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_unlink(const char *path)
{
  int res;

  char new_path[512];
  get_new_path(path, new_path, 512);

  res = unlink(new_path);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_rmdir(const char *path)
{
  int res;
  char new_path[512];
  get_new_path(path, new_path, 512);
  res = rmdir(new_path);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
  int res;

  res = symlink(from, to);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_rename(const char *from, const char *to)
{
  int res;

  res = rename(from, to);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_link(const char *from, const char *to)
{
  int res;

  res = link(from, to);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
  int res;
  char new_path[512];
  get_new_path(path, new_path, 512);
  res = chmod(new_path, mode);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
  int res;
  char new_path[512];
  get_new_path(path, new_path, 512);
  res = lchown(new_path, uid, gid);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
  int res;
  char new_path[512];
  get_new_path(path, new_path, 512);
  res = truncate(new_path, size);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
  int res;
  struct timeval tv[2];

  tv[0].tv_sec = ts[0].tv_sec;
  tv[0].tv_usec = ts[0].tv_nsec / 1000;
  tv[1].tv_sec = ts[1].tv_sec;
  tv[1].tv_usec = ts[1].tv_nsec / 1000;
  char new_path[512];
  get_new_path(path, new_path, 512);

  res = utimes(new_path, tv);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
  int res;
  char new_path[512];
  get_new_path(path, new_path, 512);
  res = open(new_path, fi->flags);
  if (res == -1)
    return -errno;

  close(res);
  return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
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

  close(fd);
  return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
  int fd;
  int res;

  char new_path[512];
  get_new_path(path, new_path, 512);

  (void) fi;
  fd = open(new_path, O_WRONLY);
  if (fd == -1)
    return -errno;

  res = pwrite(fd, buf, size, offset);
  if (res == -1)
    res = -errno;

  close(fd);
  return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
  int res;
  char new_path[512];
  get_new_path(path, new_path, 512);
  res = statvfs(new_path, stbuf);
  if (res == -1)
    return -errno;

  return 0;
}

static int xmp_release(const char *path, struct fuse_file_info *fi)
{
  /* Just a stub.	 This method is optional and can safely be left
     unimplemented */

  (void) path;
  (void) fi;
  return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
                     struct fuse_file_info *fi)
{
  /* Just a stub.	 This method is optional and can safely be left
     unimplemented */

  (void) path;
  (void) isdatasync;
  (void) fi;
  return 0;
}

#ifdef HAVE_SETXATTR

/* xattr operations are optional and can safely be left unimplemented */
static int xmp_setxattr(const char *path, const char *name, const char *value,
                        size_t size, int flags)
{
  int res = lsetxattr(path, name, value, size, flags);
  if (res == -1)
    return -errno;
  return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
                        size_t size)
{
  int res = lgetxattr(path, name, value, size);
  if (res == -1)
    return -errno;
  return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size)
{
  int res = llistxattr(path, list, size);
  if (res == -1)
    return -errno;
  return res;
}

static int xmp_removexattr(const char *path, const char *name)
{
  int res = lremovexattr(path, name);
  if (res == -1)
    return -errno;
  return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
  .getattr = xmp_getattr,
  .access = xmp_access,
  .readlink = xmp_readlink,
  .readdir = xmp_readdir,
  .mknod = xmp_mknod,
  .mkdir = xmp_mkdir,
  .symlink = xmp_symlink,
  .unlink = xmp_unlink,
  .rmdir = xmp_rmdir,
  .rename = xmp_rename,
  .link = xmp_link,
  .chmod = xmp_chmod,
  .chown = xmp_chown,
  .truncate = xmp_truncate,
  .utimens = xmp_utimens,
  .open = xmp_open,
  .read = xmp_read,
  .write = xmp_write,
  .statfs = xmp_statfs,
  .release = xmp_release,
  .fsync = xmp_fsync,
#ifdef HAVE_SETXATTR
  .setxattr = xmp_setxattr,
  .getxattr = xmp_getxattr,
  .listxattr = xmp_listxattr,
  .removexattr = xmp_removexattr,
#endif
};

int main(int argc, char *argv[])
{
  umask(0);

  //  sleep(15);

  return fuse_main(argc, argv, &xmp_oper, NULL);
}
