/* 
 * File:   MasterServer.hpp
 * Author: marcusviniciusns
 *
 * Created on 14 de Março de 2011, 23:57
 */

#ifndef MASTERSERVER_HPP
#define	MASTERSERVER_HPP

#include "../commons/server.hpp"
#include "fs.hpp"
#include <vector>

using std::vector;

enum Mode
{
  RAID_1,
  RAID_5
};

namespace yadfs
{

class MasterServer;

class DataNode
{
  friend class MasterServer;
private:
  string m_host;
  int m_port;

public:
  DataNode(const string& host, int port) : m_host(host), m_port(port)
  {
  }

  DataNode(const DataNode& orig)
  {
    m_host = orig.m_host;
    m_port = orig.m_port;
  }

  virtual ~DataNode()
  {
  }
};

class MasterServerConfig : public ServerConfig
{
private:
  Mode m_mode;
public:

  MasterServerConfig() : ServerConfig()
  {
  }

  MasterServerConfig(int port, int retries, Mode mode = RAID_5) :
  ServerConfig(port, retries), m_mode(mode)
  {
  }
};

class MasterServer : public Server
{
private:
  FileSystem m_fs;
  vector<DataNode> m_data_nodes;
public:
  MasterServer(const MasterServerConfig *config);
  MasterServer(const MasterServer& orig);
  virtual ~MasterServer();
  void registerDataNode(const DataNode& node);
  void *Receive(int sockfd);
};

}
#endif	/* MASTERSERVER_HPP */

