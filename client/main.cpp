#include <iostream>

#include "../commons/client.hpp"

using yadfs::ClientConfig;
using yadfs::Client;

int main(int argc, char* argv[])
{
  ClientConfig config("localhost", 1000);
  Client client(config);

  client.connect_();
  client.write_((void *)"oi!", 4);
  client.close_();
  return 0;
}
