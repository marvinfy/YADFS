/* 
 * File:   worker_pool.hpp
 * Author: marcusviniciusns
 *
 * Created on April 5, 2011, 11:06 PM
 */

#ifndef WORKER_POOL_HPP
#define	WORKER_POOL_HPP

#include <vector>
#include <pthread.h>

using std::vector;

namespace yadfs
{

class Job;
class Worker;
class WorkerPool
{
private:
  vector<Worker *> m_workers;
  pthread_mutex_t m_mutex;
  pthread_cond_t m_cond;
  int m_remaining;

  static void workerDone(void *instance);
public:
  WorkerPool(unsigned int workers);
  WorkerPool(const WorkerPool& orig);
  virtual ~WorkerPool();
  unsigned int getWorkersCount();
  bool addJob(unsigned int workerId, Job *job);
  void stop();
};
}
#endif	/* WORKER_POOL_HPP */

