/* 
 * File:   data_node.hpp
 * Author: marcusviniciusns
 *
 * Created on April 2, 2011, 10:04 PM
 */

#ifndef DATA_NODE_HPP
#define	DATA_NODE_HPP

#include <string>

using std::string;

namespace yadfs
{

class Client;
class Server;
class DataNode
{
  friend class Client;
  friend class Server;
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

  string *getHost()
  {
    return &m_host;
  }
  int getPort()
  {
    return m_port;
  }
};

}

#endif	/* DATA_NODE_HPP */

