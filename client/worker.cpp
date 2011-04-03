/* 
 * File:   worker.cpp
 * Author: marcusviniciusns
 * 
 * Created on April 3, 2011, 2:45 AM
 */

#include "worker.hpp"
#include "job.hpp"

#include <iostream>

yadfs::Worker::Worker() : m_running(true)
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
      pthread_cond_wait(&worker->m_cond, &worker->m_mutex);
    }
    const Job& job = worker->m_queue.front();
    worker->m_queue.pop();
    pthread_mutex_unlock(&worker->m_mutex);
    job.execute();
  }
}

void yadfs::Worker::addJob(const Job& job)
{
  pthread_mutex_lock(&m_mutex);
  m_queue.push(job);
  pthread_cond_signal(&m_cond);
  pthread_mutex_unlock(&m_mutex);
}

void yadfs::Worker::stop()
{
  m_running = false;
}
