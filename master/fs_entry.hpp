/* 
 * File:   FileSystemEntry.hpp
 * Author: marcusviniciusns
 *yadfs::
 * Created on March 26, 2011, 5:46 PM
 */

#ifndef FS_ENTRY_HPP
#define	FS_ENTRY_HPP

#include <dirent.h>
#include <sys/stat.h>
#include <iterator>
#include <string>
#include <map>
#include <vector>

using std::string;
using std::vector;

namespace yadfs
{

class FileSystemEntry
{
  friend class FileSystem;
private:
  dirent m_dirent;
  vector<FileSystemEntry> m_children;
  off_t m_size;

  static void init(FileSystemEntry& instance, ino_t ino, unsigned char type,
                   const char *name);
public:
  FileSystemEntry();
  FileSystemEntry(ino_t ino, unsigned char type, const char *name);
  FileSystemEntry(const FileSystemEntry& orig);
  virtual ~FileSystemEntry();
  dirent *getDirent();
  int getChildrenCount();
  FileSystemEntry *getChild(int index);
  bool isDirectory();
  off_t getSize();
};

}
#endif	/* FS_ENTRY_HPP */

