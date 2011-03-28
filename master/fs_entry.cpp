/* 
 * File:   fs_entry.cpp
 * Author: marcusviniciusns
 * 
 * Created on March 26, 2011, 5:46 PM
 */

#include "fs_entry.hpp"

yadfs::FileSystemEntry::FileSystemEntry() : m_size(0)
{
}

yadfs::FileSystemEntry::FileSystemEntry(ino_t ino, unsigned char type,
                                        const char *name) : m_size(0)
{
  FileSystemEntry curDir;
  FileSystemEntry parDir;

  init(*this, ino, type, name);
  init(curDir, 0, DT_DIR, ".");
  init(parDir, 0, DT_DIR, "..");

  m_children.push_back(curDir);
  m_children.push_back(parDir);
}

yadfs::FileSystemEntry::FileSystemEntry(const FileSystemEntry& orig)
{
  m_size = orig.m_size;
  memcpy(&m_dirent, &orig.m_dirent, sizeof(dirent));
  m_children.operator =(orig.m_children);
}

yadfs::FileSystemEntry::~FileSystemEntry()
{
}

void yadfs::FileSystemEntry::init(FileSystemEntry& instance, ino_t ino,
                                  unsigned char type, const char *name)
{
  memset(&instance.m_dirent, 0, sizeof(dirent));
  instance.m_dirent.d_ino = ino;
  instance.m_dirent.d_type = type;
  strncpy(instance.m_dirent.d_name, name, NAME_MAX + 1);
}

dirent *yadfs::FileSystemEntry::getDirent()
{
  return &m_dirent;
}

int yadfs::FileSystemEntry::getChildrenCount()
{
  return m_children.size();
}

yadfs::FileSystemEntry *yadfs::FileSystemEntry::getChild(int index)
{
  if (index < m_children.size())
  {
    return &m_children[index];
  }
  return NULL;
}

bool yadfs::FileSystemEntry::isDirectory()
{
  return m_dirent.d_type == DT_DIR;
}

off_t yadfs::FileSystemEntry::getSize()
{
  return m_size;
}
