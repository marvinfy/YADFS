/* 
 * File:   FileSystemEntry.hpp
 * Author: marcusviniciusns
 *yadfs::
 * Created on March 26, 2011, 5:46 PM
 */

#ifndef FS_ENTRY_HPP
#define	FS_ENTRY_HPP

#include "../commons/chunk.h"

#include <assert.h>
#include <dirent.h>
#include <iterator>
#include <string>
#include <string.h>
#include <map>
#include <sys/stat.h>
#include <vector>

using std::string;
using std::vector;

namespace yadfs
{

class FileSystemEntry
{
  friend class FileSystem;
private:
  string m_path;
  dirent m_dirent;
  struct stat m_stat;
  off_t m_size;
  vector<chunk *> m_chunks;
  vector<FileSystemEntry *> m_children;

  static void init(FileSystemEntry* instance, ino_t ino, unsigned char type,
                   const char *name);
  bool addChild(FileSystemEntry *child);

public:
  FileSystemEntry();
  FileSystemEntry(ino_t ino, unsigned char type, const char *name);
  FileSystemEntry(const FileSystemEntry& orig);
  virtual ~FileSystemEntry();
  dirent *getDirent();
  struct stat *getStat();
  off_t getSize();
  int getChildrenCount();
  FileSystemEntry *getChild(int index);
  bool isDirectory();
};

}
#endif	/* FS_ENTRY_HPP */

