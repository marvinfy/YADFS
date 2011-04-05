/* 
 * File:   worker.cpp
 * Author: marcusviniciusns
 * 
 * Created on April 3, 2011, 2:45 AM
 */

#include "worker.hpp"
#include "job.hpp"

yadfs::Worker::Worker() : m_running(true), m_work_done(NULL)
{
  pthread_mutex_init(&m_mutex, NULL);
  pthread_cond_init(&m_cond, NULL);
  pthread_create(&m_thread, NULL, &work, this);
}

yadfs::Worker::Worker(const Worker& orig)
{
}

yadfs::Worker::~Worker()
{
}

void *yadfs::Worker::work(void *data)
{
  Worker *worker = (Worker *) data;

  while (worker->m_running)
  {
    pthread_mutex_lock(&worker->m_mutex);
    if (worker->m_queue.empty())
    {
      if (worker->m_work_done)
      {
        worker->m_work_done(worker->m_work_done_param);
        worker->m_work_done = NULL;
        worker->m_work_done_param = NULL;
      }
      pthread_cond_wait(&worker->m_cond, &worker->m_mutex);
    }

    // Checking again because callbackWhenDone sends a signal and the queue
    // could be empty
    if (worker->m_queue.empty())
    {
      pthread_mutex_unlock(&worker->m_mutex);
      continue;
    }
    
    Job *job = worker->m_queue.front();
    worker->m_queue.pop();

    pthread_mutex_unlock(&worker->m_mutex);

    job->execute();
    delete job;
  }
}

void yadfs::Worker::addJob(Job *job)
{
  pthread_mutex_lock(&m_mutex);
  m_queue.push(job);
  pthread_cond_signal(&m_cond);
  pthread_mutex_unlock(&m_mutex);
}

void yadfs::Worker::callbackWhenDone(void (*work_done)(void*), void *param)
{
  pthread_mutex_lock(&m_mutex);
  m_work_done = work_done;
  m_work_done_param = param;
  pthread_cond_signal(&m_cond);
  pthread_mutex_unlock(&m_mutex);
}

void yadfs::Worker::stop()
{
  m_running = false;
}
