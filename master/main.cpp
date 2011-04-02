#include "master_server.hpp"

#include <stdio.h>

using yadfs::DataNode;
using yadfs::MasterServer;
using yadfs::MasterServerConfig;

int main(int argc, char* argv[])
{
  MasterServerConfig *config = new MasterServerConfig(10005, 5, RAID_5);

  MasterServer server(config);
  server.registerDataNode(DataNode("localhost", 10010));
  server.registerDataNode(DataNode("localhost", 10011));
  server.Start();

  return 0;
}

