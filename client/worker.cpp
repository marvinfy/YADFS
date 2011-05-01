/* 
 * File:   worker.cpp
 * Author: marcusviniciusns
 * 
 * Created on April 3, 2011, 2:45 AM
 */

#include "worker.hpp"
#include "job.hpp"

#include "../commons/logging.hpp"
using yadfs::Logging;

yadfs::Worker::Worker() : m_stopping(false)
{
  pthread_mutex_init(&m_mutex_work, NULL);
  pthread_cond_init(&m_cond_work, NULL);
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

  while (true)
  {
    pthread_mutex_lock(&worker->m_mutex_work);

    if (worker->m_queue.empty())
    {
      if (!worker->m_stopping)
      {
        pthread_cond_wait(&worker->m_cond_work, &worker->m_mutex_work);
      }

      if (worker->m_stopping && worker->m_queue.empty())
      {
        pthread_mutex_unlock(&worker->m_mutex_work);
        break;
      }
    }

    // Gets a job to work with
    Job *job = worker->m_queue.front();
    worker->m_queue.pop();

    // Releases the lock
    pthread_mutex_unlock(&worker->m_mutex_work);

    // Execute the job and then deletes it
    job->execute();
    delete job;
  }

  if (worker->m_done)
  {
    worker->m_done(worker->m_data);
  }

}

bool yadfs::Worker::addJob(Job *job)
{
  bool added = false;

  pthread_mutex_lock(&m_mutex_work);
  if (!m_stopping)
  {
    m_queue.push(job);
    pthread_cond_signal(&m_cond_work);
    added = true;
  }
  pthread_mutex_unlock(&m_mutex_work);

  return added;
}

void yadfs::Worker::stopAndCallbackWhenDone(void (*done)(void *), void *data)
{
  pthread_mutex_lock(&m_mutex_work);

  if (m_stopping)
  {
    pthread_mutex_unlock(&m_mutex_work);
    return;
  }

  m_stopping = true;
  m_done = done;
  m_data = data;

  pthread_cond_signal(&m_cond_work);
  pthread_mutex_unlock(&m_mutex_work);
}
