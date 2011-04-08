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

yadfs::WorkerPool::WorkerPool(unsigned int workers)
{
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
  stopAndWaitCompletition();
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

void yadfs::WorkerPool::stopWhenComplete()
{
  for (int i = 0; i < m_workers.size(); i++)
  {
    m_workers[i]->stopWhenComplete();
  }
}

void yadfs::WorkerPool::stopAndWaitCompletition()
{
  Logging::log(Logging::INFO, "Stopping workers");
  for (int i = 0; i < m_workers.size(); i++)
  {
    m_workers[i]->stopAndWaitCompletition();
  }
  Logging::log(Logging::INFO, "Workers stopped");
}

