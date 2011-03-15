#include "master_server.hpp"

using yadfs::ServerConfig;
using yadfs::MasterServer;

int main(int argc, char* argv[])
{
  ServerConfig srvConfig(1000, 5);
  MasterServer server(srvConfig);
  server.Start();

  return 0;
}
