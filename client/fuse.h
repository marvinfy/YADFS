/* 
 * File:   fuse.h
 * Author: marcusviniciusns
 *
 * Created on March 22, 2011, 8:37 PM
 */

#ifndef FUSE_H
#define	FUSE_H

#define FUSE_USE_VERSION 26

#ifdef linux
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#endif

#include <fuse.h>

#endif	/* FUSE_H */

