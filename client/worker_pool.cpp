/* 
 * File:   worker_pool.cpp
 * Author: marcusviniciusns
 * 
 * Created on April 5, 2011, 11:06 PM
 */

#include "worker_pool.hpp"
#include "worker.hpp"

#include "../commons/logging.hpp"
using yadfs::Logging;

yadfs::WorkerPool::WorkerPool(unsigned int workers) : m_remaining(workers)
{
  pthread_mutex_init(&m_mutex, NULL);
  pthread_cond_init(&m_cond, NULL);
  for (int i = 0; i < workers; i++)
  {
    m_workers.push_back(new Worker());
  }
}

yadfs::WorkerPool::WorkerPool(const WorkerPool& orig)
{
}

yadfs::WorkerPool::~WorkerPool()
{
}

unsigned int yadfs::WorkerPool::getWorkersCount()
{
  return m_workers.size();
}

bool yadfs::WorkerPool::addJob(unsigned int workerId, Job *job)
{
  if (workerId >= m_workers.size())
  {
    return false;
  }
  return m_workers[workerId]->addJob(job);
}

void yadfs::WorkerPool::workerDone(void *instance)
{
  WorkerPool *pool = (WorkerPool *) instance;

  pthread_mutex_lock(&pool->m_mutex);
  Logging::log(Logging::INFO, "[WorkerPool::workerDone]Done");
  if (--pool->m_remaining == 0)
  {

    Logging::log(Logging::INFO,
                 "[WorkerPool::workerDone]Sending cond a signal");
    pthread_cond_signal(&pool->m_cond);
  }
  pthread_mutex_unlock(&pool->m_mutex);
}

void yadfs::WorkerPool::stop()
{
  pthread_mutex_lock(&m_mutex);

  for (int i = 0; i < m_workers.size(); i++)
  {
    Logging::log(Logging::INFO,
                 "[WorkerPool::stop]Asking worker %d to stop", i);
    m_workers[i]->stopAndCallbackWhenDone(&workerDone, this);
  }

  Logging::log(Logging::INFO, "[WorkerPool::stop]Waiting cond");
  pthread_cond_wait(&m_cond, &m_mutex);
  Logging::log(Logging::INFO, "[WorkerPool::stop]Stopped");
  pthread_mutex_unlock(&m_mutex);
}
