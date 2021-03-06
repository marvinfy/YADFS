/* 
 * File:   main.cpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 12:18 AM
 */

#include "node_server.hpp"
#include "../commons/utils.hpp"
#include "../commons/logging.hpp"
#include "../commons/raid_mode.h"
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

  string *id = getParameter("datanode.id");
  if (id == NULL)
  {
    id = new string("0");
  }
  string *count = getParameter("datanode.count");
  if (count == NULL)
  {
    count = new string("0");
  }
  string *port = getParameter("datanode.port");
  if (port == NULL)
  {
    port = new string("10100");
  }
  string *mode = getParameter("datanode.raid.mode");
  if (mode == NULL)
  {
    mode = new string("0");
  }

  int nId = atoi(id->c_str());
  int nCount = atoi(count->c_str());
  int nPort = atoi(port->c_str());
  Mode raidMode = (Mode) atoi(mode->c_str());
  int nRetries = 1000;

  Logging::log(Logging::INFO, "Data Node %d is about to start on port %d",
               nId, nPort);
  Logging::log(Logging::INFO, "Node count: %d", nCount);
  Logging::log(Logging::INFO, "Raid mode: %d", raidMode);



  ServerConfig config(nPort, nRetries, yadfs::SINGLE_THREADED);
  NodeServer server(config, nId, nCount, raidMode);
  server.Start();

  delete id;
  delete count;
  delete port;

  return 0;
}
