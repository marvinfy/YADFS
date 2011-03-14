#include <iostream>

#include "file_manager.hpp"
#include "../commons/server.hpp"

#include <unistd.h>

using yadfs::ServerConfig;
using yadfs::Server;

int main(int argc, char* argv[])
{
  ServerConfig srvConfig(1000, 5);
  Server server(srvConfig);
  server.start();

  return 0;
}

void *receive(void *data)
{
  int *sockfd = (int *)data;
  char buff[4];
  read(*sockfd, buff, 4);

  printf("%s\n", buff);

  return NULL;
}
