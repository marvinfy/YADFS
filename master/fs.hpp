/* 
 * File:   fs.hpp
 * Author: marcusviniciusns
 *
 * Created on March 26, 2011, 5:39 PM
 */

#ifndef FS_HPP
#define	FS_HPP

#include "fs_entry.hpp"

#include <string>
#include <map>

using std::string;
using std::map;

namespace yadfs
{

typedef map<string, FileSystemEntry> entry_map;
typedef entry_map::iterator entry_it;

class FileSystem
{
private:
  FileSystemEntry m_root;
  entry_map m_entries;
public:
  FileSystem();
  FileSystem(const FileSystem& orig);
  virtual ~FileSystem();
  FileSystemEntry *getEntry(string path)
  {
    entry_it it = m_entries.find(path);
    if(it == m_entries.end())
    {
      return NULL;
    }
    return &it->second;
  }

};

}

#endif	/* FS_HPP */

