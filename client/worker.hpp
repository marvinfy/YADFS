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
  pthread_mutex_t m_mutex;
  pthread_cond_t m_cond;
  pthread_t m_thread;
  queue<Job *> m_queue;
  bool m_running;

  void (*m_work_done)(void*);
  void *m_work_done_param;
  
  static void *work(void *data);
public:
  Worker();
  Worker(const Worker& orig);
  virtual ~Worker();
  void addJob(Job *job);
  void callbackWhenDone(void (*work_done)(void*), void *param);
  void stop();
};

}
#endif	/* WORKER_HPP */
