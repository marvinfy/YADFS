/* 
 * File:   chunk.h
 * Author: marcusviniciusns
 *
 * Created on April 2, 2011, 12:24 AM
 */

#ifndef CHUNK_H
#define	CHUNK_H

#define CHUNK_SIZE 4096

typedef unsigned char byte;

typedef struct _chunk {
  int m_id;
  int m_node_id;
  byte m_data[CHUNK_SIZE];
} chunk;

#endif	/* CHUNK_H */
