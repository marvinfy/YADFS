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

namespace yadfs
{

class NodeServer : public Server
{
private:
  DIR *m_dir;
public:
  NodeServer(const ServerConfig& config);
  NodeServer(const NodeServer& orig);
  virtual ~NodeServer();
  int writeFile(unsigned int fileId, unsigned int chunkId, const char *data,
                size_t size);
  int readFile(unsigned int fileId, unsigned int chunkId, char *data,
               size_t size);
  void *Receive(int sockfd);
};

}

#endif	/* NODESERVER_HPP */

