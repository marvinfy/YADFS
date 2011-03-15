/* 
 * File:   messages.hpp
 * Author: marcusviniciusns
 *
 * Created on 15 de Março de 2011, 00:45
 */

#ifndef MESSAGES_HPP
#define	MESSAGES_HPP

#include <time.h>

enum msgs
{
  MSG_REQ_HANDSHAKE,
  MSG_REQ_ECHO,
  MSG_REQ_SHUTDOWN,
  MSG_RES_SHUTDOWN
};

typedef struct _msg_req_handshake {
  int m_msg_id;
} msg_req_handshake;

typedef struct _msg_req_echo {
  int m_len;
} msg_req_echo;

typedef struct _msg_req_shutdown {
  char m_user[64];
  char m_pass[64];
  time_t m_timeout;
} msg_req_shutdown;

typedef struct _msg_res_shutdown {
  bool m_ok;
} msg_res_shutdown;

#endif	/* MESSAGES_HPP */

