/* 
 * File:   fs.cpp
 * Author: marcusviniciusns
 * 
 * Created on March 26, 2011, 5:39 PM
 */

#include "fs.hpp"

yadfs::FileSystem::FileSystem()
{
  m_root = new FileSystemEntry(0, DT_DIR, "/");

  entry_pair root_pair;
  root_pair.first = "/";
  root_pair.second = m_root;
  
  m_entries.insert(root_pair);
}

yadfs::FileSystem::FileSystem(const FileSystem& orig)
{
  delete m_root;
}

yadfs::FileSystem::~FileSystem()
{
}

yadfs::FileSystemEntry *yadfs::FileSystem::getEntry(const string& path)
{
  entry_it it = m_entries.find(path);
  if(it == m_entries.end())
  {
    return NULL;
  }
  return it->second;
}


