/* 
 * File:   messages.hpp
 * Author: marcusviniciusns
 *
 * Created on 15 de Março de 2011, 00:45
 */

#ifndef MESSAGES_HPP
#define	MESSAGES_HPP

#include <dirent.h>
#include <sys/stat.h>
#include <time.h>


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
  MSG_RES_READDIR
};

typedef struct _msg_req_handshake {
  int m_msg_id;
} msg_req_handshake;

/*
 * Not in use right now
 */
typedef struct _msg_res_handshake {
  int m_session_id;
} msg_res_handshake;

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
  int m_count;
} msg_res_readdir;

typedef struct _msg_req_dirent {
  char m_path[256];
  int m_idx;
} msg_req_dirent;

typedef struct _msg_res_dirent {
  struct dirent m_dirent;
} msg_res_dirent;


#endif	/* MESSAGES_HPP */

