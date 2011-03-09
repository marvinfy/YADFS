#ifndef FILE_MANAGER_HPP
#define	FILE_MANAGER_HPP

#include "file.hpp"

#include <map>
#include <string>

using std::map;
using std::string;

namespace yadfs
{


typedef map<string, File> FileMap;
typedef FileMap::iterator FileMapIt;
typedef FileMap::const_iterator FileMapConstIt;

class FileManager
{
private:
  map<string, File> m_files;
  int load();
  int persist();
public:
  int init(const map<string, string>& config)
  {
    load();
  }

  int stop()
  {
    persist();
  }

  File *getFile(string& path)
  {
    FileMapIt it = m_files.find(path);
    if (it == m_files.end())
    {
      return NULL;
    }
    return &it->second;
  }

  int addFile(File& file)
  {
    string *path = file.getPath();
    FileMapIt it = m_files.find(*path);
    if (it != m_files.end())
    {
      return -1;
    }

    m_files[*path] = file;
    return 0;
  }
};

}

#endif	/* FILE_MANAGER_HPP */
