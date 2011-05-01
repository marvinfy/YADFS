/* 
 * File:   node_server.hpp
 * Author: marcusviniciusns
 *
 * Created on April 6, 2011, 6:56 PM
 */

#ifndef NODESERVER_HPP
#define	NODESERVER_HPP

#include "../commons/server.hpp"
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>


#include <map>


using std::map;
using std::iterator;
using std::pair;

namespace yadfs
{

class NodeServer : public Server
{
  typedef map<unsigned int, FILE *> fd_map;
  typedef pair<unsigned int, FILE *> fd_pair;
  typedef fd_map::iterator fd_it;

private:
  unsigned int m_id;
  unsigned int m_count;
  DIR *m_dir;
  fd_map m_fds;
public:
  NodeServer(const ServerConfig& config, unsigned int id, unsigned int count);
  NodeServer(const NodeServer& orig);
  virtual ~NodeServer();
  FILE *getFD(unsigned int fileId, const char *mode);
  void closeFD(unsigned int fileId);
  int readFile(unsigned int fileId, unsigned int chunkId, char *data,
               size_t size);
  int writeFile(unsigned int fileId, unsigned int chunkId, const char *data,
                size_t size);
  int readChunk(unsigned int fileId, unsigned int chunkId, char *data,
                size_t size);
  int writeChunk(unsigned int fileId, unsigned int chunkId, const char *data,
                 size_t size);
  void *Receive(int sockfd);
};

}

#endif	/* NODESERVER_HPP */

