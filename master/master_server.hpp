/* 
 * File:   MasterServer.hpp
 * Author: marcusviniciusns
 *
 * Created on 14 de Março de 2011, 23:57
 */

#ifndef MASTERSERVER_HPP
#define	MASTERSERVER_HPP

#include "../commons/server.hpp"

namespace yadfs
{

class MasterServer : public Server
{
private:
public:
  MasterServer(const ServerConfig& config);
  MasterServer(const MasterServer& orig);
  virtual ~MasterServer();
  void *Receive();
};

}
#endif	/* MASTERSERVER_HPP */

