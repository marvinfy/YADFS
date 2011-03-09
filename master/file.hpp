#ifndef FILE_HPP
#define	FILE_HPP

#include <pthread.h>

#include <string>
#include <vector>

#include "chunk.hpp"

using std::vector;
using std::string;

namespace yadfs
{


enum OpenMode
{
  RO,
  RW
};

class Chunk;
class File
{
private:
  pthread_mutex_t m_mutex;
  OpenMode m_mode;

  string m_path;
  vector<Chunk> m_chunks;
  // int permissions, char *user, etc
public:
  File(const string& path) : m_path(path)
  {
    pthread_mutex_init(&m_mutex, NULL);
  }

  ~File()
  {
    pthread_mutex_destroy(&m_mutex);
  }

  const string& getPath() const
  {
    return m_path;
  }
  
  int getChunkCount()
  {
    return m_chunks.size();
  }

  Chunk *getChunk(int index)
  {
    return &m_chunks[index];
  }
};

}

#endif	/* FILE_HPP */

