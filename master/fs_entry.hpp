/* 
 * File:   FileSystemEntry.hpp
 * Author: marcusviniciusns
 *yadfs::
 * Created on March 26, 2011, 5:46 PM
 */

#ifndef FS_ENTRY_HPP
#define	FS_ENTRY_HPP

#include <dirent.h>
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
private:
  dirent m_dirent;
  vector<FileSystemEntry> m_children;

public:
  FileSystemEntry();
  FileSystemEntry(const FileSystemEntry& orig);
  virtual ~FileSystemEntry();

  dirent *getDirent()
  {
    return &m_dirent;
  }

  int getChildrenCount()
  {
    return m_children.size();
  }

  FileSystemEntry *getChild(int index)
  {
    if (index < m_children.size())
    {
      return &m_children[index];
    }
    return NULL;
  }
  
};

}
#endif	/* FS_ENTRY_HPP */

