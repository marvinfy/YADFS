/* 
 * File:   node_server.hpp
 * Author: marcusviniciusns
 *
 * Created on April 6, 2011, 6:56 PM
 */

#ifndef NODESERVER_HPP
#define	NODESERVER_HPP

#include "../commons/server.hpp"

namespace yadfs
{

class NodeServer : public Server
{
private:
public:
  NodeServer(const ServerConfig& config);
  NodeServer(const NodeServer& orig);
  virtual ~NodeServer();
  void *Receive(int sockfd);
};

}

#endif	/* NODESERVER_HPP */

