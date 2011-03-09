#ifndef FILE_MANAGER_HPP
#define	FILE_MANAGER_HPP

#include "file.hpp"

#include <map>
#include <string>

using std::map;
using std::string;


namespace yadfs
{

class File;
class Chunk;

typedef map<string, File> FileMap;
typedef FileMap::iterator FileMapIt;

class FileManager
{
private:
  FileMap m_files;
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


  File *getFile(const string& path) const
  {
    m_files.find(path);
    /*
    FileMapIt it = 
    if (it == m_files.end())
    {
      return NULL;
    }
    return &(it->second());*/return NULL;
  }

  int addFile(const File& file)
  {
    const string& path = file.getPath();
    /*m_files[path] = file;*/

    /*
    string *path = file.getPath();

    FileMapIt it = m_files.find(*path);
    if (it != m_files.end())
    {
      return -1;
    }
*/
    //m_files[*path] = file;
    return 0;
  }
};

}

#endif	/* FILE_MANAGER_HPP */
