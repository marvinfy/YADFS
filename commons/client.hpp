/* 
 * File:   Client.hpp
 * Author: marcusviniciusns
 *
 * Created on 14 de Março de 2011, 01:10
 */

#ifndef CLIENT_HPP
#define	CLIENT_HPP

#include <iostream>
#include <string>

using std::string;

namespace yadfs
{

class ClientConfig
{
public:
  string m_host;
  int m_port;

  ClientConfig()
  {
    m_host = "localhost";
    m_port = 10000;
  }

  ClientConfig(string host, int port) : m_host(host), m_port(port) {}
};

class Client
{
private:
  int m_sockfd;
  ClientConfig m_config;
public:
  Client(const ClientConfig& config);
  Client(const Client& orig);
  virtual ~Client();

  int Connect();
  bool Write(void *data, int len);
  bool Read(void *data, int len);
  void Close();
};

}
#endif	/* CLIENT_HPP */

