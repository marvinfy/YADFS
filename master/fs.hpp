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
using std::pair;

namespace yadfs
{

typedef map<string, FileSystemEntry *> entry_map;
typedef entry_map::iterator entry_it;
typedef pair<string, FileSystemEntry *> entry_pair;

class FileSystem
{
private:
  entry_map m_entries;
public:
  FileSystem();
  FileSystem(const FileSystem& orig);
  virtual ~FileSystem();
  FileSystemEntry *getEntry(const string& path);
  bool addEntry(const string& parentPath, FileSystemEntry *child);
  bool addEntry(FileSystemEntry *parent, FileSystemEntry *child);
  static unsigned int getNextId();
};

}

#endif	/* FS_HPP */

