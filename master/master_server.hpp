/* 
 * File:   MasterServer.hpp
 * Author: marcusviniciusns
 *
 * Created on 14 de Março de 2011, 23:57
 */

#ifndef MASTERSERVER_HPP
#define	MASTERSERVER_HPP

#include "fs.hpp"
#include "../commons/data_node.hpp"
#include "../commons/raid_mode.h"
#include "../commons/server.hpp"

#include <vector>

using std::vector;

namespace yadfs
{

class MasterServerConfig : public ServerConfig
{
private:
  Mode m_mode;
public:

  MasterServerConfig() : ServerConfig()
  {
  }

  MasterServerConfig(int port, int retries, Mode mode = RAID_0) :
  ServerConfig(port, retries), m_mode(mode)
  {
  }

  Mode getMode()
  {
    return m_mode;
  }
};

class MasterServer : public Server
{
private:
  FileSystem m_fs;
  vector<DataNode> m_data_nodes;
  MasterServerConfig *getConfig()
  {
    return (MasterServerConfig *)&m_config;
  }
public:
  MasterServer(const MasterServerConfig *config);
  MasterServer(const MasterServer& orig);
  virtual ~MasterServer();
  void registerDataNode(const DataNode& node);
  void *Receive(int sockfd);
};

}
#endif	/* MASTERSERVER_HPP */

