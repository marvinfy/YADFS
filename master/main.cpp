#include <iostream>

#include "file_manager.hpp"
#include "server.hpp"

int main(int argc, char* argv[])
{
  yadfs::ServerConfig config;
  config.m_port = 10000;
  config.m_retries = 5;

  yadfs::Server server(config);

  server.start();

  return 0;
}







