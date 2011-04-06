#include "master_server.hpp"

#include <stdio.h>

using yadfs::DataNode;
using yadfs::MasterServer;
using yadfs::ServerConfig;

int main(int argc, char* argv[])
{
  ServerConfig config(10005, 1000);
  MasterServer server(config);

  server.setMode(RAID_0);
  server.registerDataNode(DataNode("localhost", 10005));
  server.registerDataNode(DataNode("localhost", 10005));
  server.registerDataNode(DataNode("localhost", 10005));
  server.registerDataNode(DataNode("localhost", 10005));

  server.Start();

  return 0;
}

