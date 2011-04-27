/*
 * File:   utils.cpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 9:47 PM
 */


#include "utils.hpp"
#include "../commons/logging.hpp"

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

using std::ifstream;
using std::map;
using std::pair;
using std::string;
using std::ios;
using yadfs::Logging;

typedef map<string, string> par_map;
typedef par_map::iterator par_it;
typedef pair<string, string> par_pair;

static par_map gblParameters;

void parse_argv(int argc, char *argv[])
{
  int i, idx;
  char path[128];
  string line;
  par_pair p;

  if (argc < 2)
  {
    return;
  }

  memset(path, 0, 128);
  for (i = 1; i < argc; i++)
  {
    if (strlen(argv[i]) < 9)
    {
      continue;
    }

    if (memcmp(argv[i], "-config=", 8) == 0)
    {
      strncpy(path, &argv[i][8], 127);
      break;
    }
  }

  if (path[0] == 0)
  {
    Logging::log(Logging::ERROR, "Configuration file was not found");
    return;
  }

  ifstream file(path, ios::in);
  if (file.is_open())
  {
    while(file.good())
    {
      getline(file, line);

      idx = line.find_first_of('=');
      p.first = line.substr(0, idx);
      p.second = line.substr(idx + 1, line.length());
      gblParameters.insert(p);
    }
  }
  else
  {
    Logging::log(Logging::ERROR, "Configuration file could not be opened");
  }

  file.close();
}

int get_parameter(const char *key, char *val, int size)
{
  par_it it = gblParameters.find(key);
  if (it == gblParameters.end())
  {
    return -1;
  }
  strncpy(val, it->second.c_str(), size);
  return 0;
}

string *getParameter(const string& key)
{
  par_it it = gblParameters.find(key);
  if (it == gblParameters.end())
  {
    return NULL;
  }

  return new string(it->second);
}

