/* 
 * File:   fs.cpp
 * Author: marcusviniciusns
 * 
 * Created on March 26, 2011, 5:39 PM
 */

#include "fs.hpp"

yadfs::FileSystem::FileSystem()
{
  FileSystemEntry *root = new FileSystemEntry(0, DT_DIR, "/");

  entry_pair root_pair;
  root_pair.first = root->m_path;
  root_pair.second = root;
  m_entries.insert(root_pair);

  FileSystemEntry *folder1 = new FileSystemEntry(0, DT_DIR, "folder_01");
  FileSystemEntry *folder2 = new FileSystemEntry(0, DT_DIR, "folder_02");
  FileSystemEntry *folder1_1 = new FileSystemEntry(0, DT_DIR, "folder_01_01");
  FileSystemEntry *folder1_2 = new FileSystemEntry(0, DT_DIR, "folder_01_02");
  FileSystemEntry *file1 = new FileSystemEntry(0, DT_REG, "file_01.txt");
  FileSystemEntry *file2 = new FileSystemEntry(0, DT_REG, "file_02.txt");
  FileSystemEntry *file3 = new FileSystemEntry(0, DT_REG, "file_03.txt");
  FileSystemEntry *file4 = new FileSystemEntry(0, DT_REG, "file_04.txt");
  FileSystemEntry *file5 = new FileSystemEntry(0, DT_REG, "file_05.txt");

  addEntry(root, folder1);
  addEntry(root, folder2);
  addEntry(root, file1);
  addEntry(root, file2);
  addEntry(folder1, folder1_1);
  addEntry(folder1, folder1_2);
  addEntry(folder1_1, file3);
  addEntry(folder1_1, file4);
  addEntry(folder1_2, file5);
}

yadfs::FileSystem::FileSystem(const FileSystem& orig)
{
}

yadfs::FileSystem::~FileSystem()
{
}

yadfs::FileSystemEntry *yadfs::FileSystem::getEntry(const string& path)
{
  entry_it it = m_entries.find(path);
  if (it == m_entries.end())
  {
    return NULL;
  }
  return it->second;
}

bool yadfs::FileSystem::addEntry(const string& parentPath, FileSystemEntry *child)
{
  FileSystemEntry *parent = getEntry(parentPath);
  if (parent == NULL || child == NULL)
  {
    return false;
  }

  parent->addChild(child);

  entry_pair new_pair;
  new_pair.first = child->m_path;
  new_pair.second = child;
  m_entries.insert(new_pair);

  return true;
}

bool yadfs::FileSystem::addEntry(FileSystemEntry *parent, FileSystemEntry *child)
{
  if (parent == NULL || child == NULL)
  {
    return false;
  }

  return addEntry(parent->m_path, child);
}
