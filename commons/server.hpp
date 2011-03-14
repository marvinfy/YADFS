/* 
 * File:   server.hpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 9:48 PM
 */

#ifndef SERVER_HPP
#define	SERVER_HPP

void *receive(void *data);

namespace yadfs
{

class ServerConfig
{
public:
  int m_port;
  unsigned int m_retries;

  ServerConfig()
  {
    m_port = 10000;
    m_retries = 10;
  }

  ServerConfig(int port, int retries) : m_port(port), m_retries(retries)
  {
  }
};

class Server
{
private:
  bool m_running;
  ServerConfig m_config;
public:
  Server(const ServerConfig& config);
  Server(const Server& orig);
  virtual ~Server();
  virtual int start();
  virtual int stop();
};

}

#endif	/* SERVER_HPP */
