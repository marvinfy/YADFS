/* 
 * File:   messages.hpp
 * Author: marcusviniciusns
 *
 * Created on 15 de Março de 2011, 00:45
 */

#ifndef MESSAGES_HPP
#define	MESSAGES_HPP

#include "raid_mode.h"

#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fuse.h>

#pragma pack(16)

typedef char boolean;

enum msgs
{
  MSG_REQ_HANDSHAKE,
  MSG_RES_HANDSHAKE,
  MSG_REQ_ECHO,
  MSG_REQ_SHUTDOWN,
  MSG_RES_SHUTDOWN,
  MSG_REQ_GETATTR,
  MSG_RES_GETATTR,
  MSG_REQ_READDIR,
  MSG_RES_READDIR,
  MSG_REQ_MKNOD,
  MSG_RES_MKNOD,
  MSG_REQ_OPEN,
  MSG_RES_OPEN,
  MSG_REQ_SERVERCONFIG,
  MSG_REQ_GETSIZE,
  MSG_REQ_SETSIZE,
  MSG_REQ_DELALL,
  MSG_RES_DELALL,
  MSG_REQ_ADDCHUNK,
  MSG_RES_ADDCHUNK
};

typedef struct _msg_req_handshake {
  int m_msg_id;
} msg_req_handshake;

/*
 * Not in use right now
 */
/*
typedef struct _msg_res_handshake {
  int m_session_id;
} msg_res_handshake;
*/

typedef struct _msg_req_echo {
  int m_len;
} msg_req_echo;

typedef struct _msg_req_shutdown {
  char m_user[64];
  char m_pass[64];
  time_t m_timeout;
} msg_req_shutdown;

typedef struct _msg_res_shutdown {
  boolean m_ok;
} msg_res_shutdown;

typedef struct _msg_req_path {
  char m_path[256];
} msg_req_path;

typedef msg_req_path msg_req_getattr;

typedef struct _msg_res_getattr {
  int m_err;
  struct stat m_stat;
} msg_res_getattr;

typedef msg_req_path msg_req_readdir;

typedef struct _msg_res_readdir {
  int m_children_count;
} msg_res_readdir;

/*
 * Not in use right now
 */
/*
typedef struct _msg_req_dirent {
  char m_path[256];
  int m_idx;
} msg_req_dirent;
*/

typedef struct _msg_res_dirent {
  struct dirent m_dirent;
} msg_res_dirent;

typedef struct _msg_req_mknod {
  char m_path[256];
  mode_t m_mode;
  dev_t m_rdev;
} msg_req_mknod;

typedef struct _msg_res_mknod {
  int m_err;
} msg_res_mknod;

typedef struct _msg_req_open {
  char m_path[256];
  struct fuse_file_info fi;
} msg_req_open;

typedef struct _msg_res_open {
  int m_err;
} msg_res_open;

typedef struct _msg_res_serverconfig {
  Mode m_mode;
  int m_node_count;
} msg_res_serverconfig;

typedef struct _msg_res_datanode {
  char m_host[128];
  int m_port;
} msg_res_datanode;

typedef struct _msg_req_getsize {
  char m_path[256];
} msg_req_getsize;

typedef struct _msg_res_getsize {
  boolean m_ok;
  size_t m_size;
} msg_res_getsize;

typedef struct _msg_req_setsize {
  char m_path[256];
  size_t m_size;
} msg_req_setsize;

typedef struct _msg_res_setsize {
  boolean m_ok;
} msg_res_setsize;

typedef struct _msg_req_delall {
} msg_req_delall;

typedef struct _msg_res_delall {
  boolean m_ok;
} msg_res_delall;

typedef struct _msg_req_addchunk {
  unsigned int m_file_id;
  unsigned int m_chunk_id;
  // data
} msg_req_addchunk;

typedef struct _msg_res_addchunk {
  boolean m_ok;
} msg_res_addchunk;


#endif	/* MESSAGES_HPP */
