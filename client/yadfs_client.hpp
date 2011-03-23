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
extern "C"
{
#endif

int yadfs_client_init(char *host, int port);
int yadfs_getattr_real(const char *path, struct stat *stbuf);
int yadfs_readdir_real(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi);

#ifdef	__cplusplus
}
#endif

#endif	/* YADFS_CLIENT_HPP */

