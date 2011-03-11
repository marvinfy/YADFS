#include <iostream>

#include "file_manager.hpp"
#include "../commons/server.hpp"

using yadfs::ServerConfig;
using yadfs::Server;

int main(int argc, char* argv[])
{
  ServerConfig config(1000, 5);
  Server server(config);

  server.start();

  return 0;
}







