#include "master_server.hpp"

#include <stdio.h>

using yadfs::DataNode;
using yadfs::MasterServer;
using yadfs::ServerConfig;

int main(int argc, char* argv[])
{
  ServerConfig config(10005, 5);
  MasterServer server(config);

  server.setMode(RAID_0);
  server.registerDataNode(DataNode("localhost", 10010));
  server.registerDataNode(DataNode("localhost", 10011));
  server.registerDataNode(DataNode("localhost", 10012));
  server.registerDataNode(DataNode("localhost", 10013));

  server.Start();

  return 0;
}

