/* 
 * File:   worker_pool.hpp
 * Author: marcusviniciusns
 *
 * Created on April 5, 2011, 11:06 PM
 */

#ifndef WORKER_POOL_HPP
#define	WORKER_POOL_HPP

#include <vector>

using std::vector;

namespace yadfs
{

class Job;
class Worker;
class WorkerPool
{
private:
  vector<Worker *> m_workers;
public:
  WorkerPool(unsigned int workers);
  WorkerPool(const WorkerPool& orig);
  virtual ~WorkerPool();
  unsigned int getWorkersCount();
  bool addJob(unsigned int workerId, Job *job);
  void stopWhenComplete();
  void stopAndWaitCompletition();
};
}
#endif	/* WORKER_POOL_HPP */

