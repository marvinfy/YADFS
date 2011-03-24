#include "master_server.hpp"

using yadfs::ServerConfig;
using yadfs::MasterServer;

#include <stdio.h>

int main(int argc, char* argv[])
{
  ServerConfig srvConfig(10000, 5);
  srvConfig.m_verbose = true;
  
  MasterServer server(srvConfig);
  server.Start();

  return 0;
}

