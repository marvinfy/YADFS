#include "master_server.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "../commons/utils.hpp"
#include "../commons/logging.hpp"

using yadfs::Logging;
using yadfs::DataNode;
using yadfs::MasterServer;
using yadfs::ServerConfig;

int main(int argc, char* argv[])
{
  parse_argv(argc, argv);

  string *port = getParameter("master.port");
  if (port == NULL)
  {
    port = new string("10005");
  }
  int nPort = atoi(port->c_str());
  int nRetries = 1000;

  string *raid = getParameter("master.raid.mode");
  if (raid == NULL)
  {
    raid = new string("0");
  }
  Mode mode;
  if (*raid == "0")
  {
    mode = RAID_0;
  }
  else if (*raid == "1")
  {
    mode = RAID_1;
  }
  else
  {
    Logging::log(Logging::ERROR, "Invalid configuration file");
    return -1;
  }

  // Starting server..
  Logging::log(Logging::INFO, "Master is about to start on port %d", nPort);
  ServerConfig config(nPort, nRetries);
  MasterServer server(config);
  server.setMode(mode);

  // Registering Data Nodes
  string *count = getParameter("datanode.count");
  if (count == NULL)
  {
    count = new string("0");
  }
  int nCount = atoi(count->c_str());

  for (int i = 1; i <= nCount; i++)
  {
    char hostKey[32], portKey[32];
    string *hostValue, *portValue;
    int nPortValue;

    sprintf(hostKey, "datanode[%d].host", i);
    hostValue = getParameter(hostKey);
    if (hostValue == NULL)
    {
      Logging::log(Logging::ERROR, "Invalid configuration file");
      return -1;
    }

    sprintf(portKey, "datanode[%d].port", i);
    portValue = getParameter(portKey);
    if (portValue == NULL)
    {
      Logging::log(Logging::ERROR, "Invalid configuration file");
      return -1;
    }
    nPortValue = atoi(portValue->c_str());

    server.registerDataNode(DataNode(*hostValue, nPortValue));
    Logging::log(Logging::INFO, "DataNode registered: %s:%s",
                 hostValue->c_str(), portValue->c_str());

    delete hostValue;
    delete portValue;
  }

  server.Start();

  delete port;
  delete raid;
  delete count;

  return 0;
}

