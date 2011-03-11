/* 
 * File:   server.hpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 9:48 PM
 */

#ifndef SERVER_HPP
#define	SERVER_HPP

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
};

class Server
{
private:
  bool m_running;
  ServerConfig m_config;
  static void *receive(void *data);
public:
  Server(const ServerConfig& config);
  Server(const Server& orig);
  virtual ~Server();
  int start();
  int stop();
};

}
#endif	/* SERVER_HPP */

