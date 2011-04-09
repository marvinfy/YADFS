/* 
 * File:   worker.hpp
 * Author: marcusviniciusns
 *
 * Created on April 3, 2011, 2:45 AM
 */

#ifndef WORKER_HPP
#define	WORKER_HPP

#include <pthread.h>
#include <queue>

using std::queue;

namespace yadfs
{

class Job;
class Worker
{
private:
  pthread_mutex_t m_mutex_work;
  pthread_cond_t m_cond_work;
  pthread_t m_thread;
  
  queue<Job *> m_queue;
  bool m_stopping;

  void (*m_done)(void *);
  void *m_data;

  static void *work(void *data);
public:
  Worker();
  Worker(const Worker& orig);
  virtual ~Worker();
  bool addJob(Job *job);
  void stopAndCallbackWhenDone(void (*done)(void *), void *data);
};

}
#endif	/* WORKER_HPP */
