/* 
 * File:   main.cpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 12:18 AM
 */

#include "node_server.hpp"
#include "../commons/utils.hpp"
#include "../commons/logging.hpp"
#include <stdlib.h>

using yadfs::NodeServer;
using yadfs::ServerConfig;
using yadfs::Logging;

/*
 * 
 */
int main(int argc, char* argv[])
{
  parse_argv(argc, argv);
  string *port = getParameter("datanode.port");
  if (port == NULL)
  {
    port = new string("10100");
  }

  int nPort = atoi(port->c_str());
  int nRetries = 1000;
  
  Logging::log(Logging::INFO, "Data Node is about to start on port %d", nPort);
  
  ServerConfig config(nPort, nRetries);
  NodeServer server(config);
  server.Start();

  delete port;

  // cria res = open(new_path, O_CREAT | O_EXCL | O_WRONLY, mode);

  return 0;
}
