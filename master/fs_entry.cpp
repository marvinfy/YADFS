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
  init(this, ino, type, name);

  if (type == DT_DIR)
  {
    FileSystemEntry *currentDir = new FileSystemEntry();
    init(currentDir, 0, DT_DIR, ".");
    m_children.push_back(currentDir);

    FileSystemEntry *parentDir = new FileSystemEntry();
    init(parentDir, 0, DT_DIR, "..");
    m_children.push_back(parentDir);
  }
}

yadfs::FileSystemEntry::FileSystemEntry(const FileSystemEntry& orig)
{
  m_path = orig.m_path;
  m_size = orig.m_size;
  memcpy(&m_dirent, &orig.m_dirent, sizeof(dirent));
  m_children.operator =(orig.m_children);
}

yadfs::FileSystemEntry::~FileSystemEntry()
{
}

void yadfs::FileSystemEntry::init(FileSystemEntry *instance, ino_t ino,
                                  unsigned char type, const char *name)
{
  memset(&instance->m_dirent, 0, sizeof(dirent));
  instance->m_dirent.d_ino = ino;
  instance->m_dirent.d_type = type;
  strncpy(instance->m_dirent.d_name, name, NAME_MAX + 1);

  memset(&instance->m_stat, 0, sizeof(struct stat));
  if (type == DT_DIR)
  {
    instance->m_stat.st_mode = S_IFDIR | 0755;
    instance->m_stat.st_nlink = 2;
    instance->m_stat.st_size = 4096; // Default directory size
  }
  else
  {
    assert(type == DT_REG); // Must be DT_REG
    instance->m_stat.st_mode = S_IFREG | 0666;
    instance->m_stat.st_nlink = 1;
    instance->m_stat.st_size = 0;
  }

  instance->m_path = name;
}

dirent *yadfs::FileSystemEntry::getDirent()
{
  return &m_dirent;
}

struct stat *yadfs::FileSystemEntry::getStat()
{
  return &m_stat;
}

int yadfs::FileSystemEntry::getChildrenCount()
{
  assert(m_children.size() == m_stat.st_nlink);
  return m_stat.st_nlink;
}

yadfs::FileSystemEntry *yadfs::FileSystemEntry::getChild(int index)
{
  if (index < m_children.size())
  {
    return m_children[index];
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

bool yadfs::FileSystemEntry::addChild(FileSystemEntry *child)
{
  if (child == NULL || !isDirectory())
  {
    return false;
  }

  if (m_path == "/")
  {
    child->m_path = m_path + child->m_path;
  }
  else
  {
    child->m_path = m_path + "/" + child->m_path;
  }

  m_children.push_back(child);
  m_stat.st_nlink++;

  return true;
}