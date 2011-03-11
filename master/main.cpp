#include <iostream>

#include "file_manager.hpp"
#include "../commons/server.hpp"

int main(int argc, char* argv[])
{
  yadfs::ServerConfig config(1000, 5);
  yadfs::Server server(config);

  server.start();

  return 0;
}







