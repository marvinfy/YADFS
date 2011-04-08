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

yadfs::Worker::Worker() : m_stopping(false), m_waiting(false)
{
  pthread_mutex_init(&m_mutex_work, NULL);
  pthread_mutex_init(&m_mutex_finished, NULL);
  pthread_cond_init(&m_cond_work, NULL);
  pthread_cond_init(&m_cond_finished, NULL);
  pthread_create(&m_thread, NULL, &work, this);
}

yadfs::Worker::Worker(const Worker& orig)
{
}

yadfs::Worker::~Worker()
{
  stopAndWaitCompletition();
}

void *yadfs::Worker::work(void *data)
{
  Worker *worker = (Worker *)data;

  while (true)
  {
    pthread_mutex_lock(&worker->m_mutex_work);

    if (worker->m_queue.empty())
    {
      // Wait until a job is added or the worker is stopped
      pthread_cond_wait(&worker->m_cond_work, &worker->m_mutex_work);

      if (worker->m_stopping && worker->m_queue.empty())
      {
        if (worker->m_waiting)
        {
          pthread_mutex_lock(&worker->m_mutex_finished);
          pthread_cond_signal(&worker->m_cond_finished);
          pthread_mutex_unlock(&worker->m_mutex_finished);
        }
        
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

void yadfs::Worker::stopWhenComplete()
{
  pthread_mutex_lock(&m_mutex_work);

  if (m_stopping)
  {
    pthread_mutex_unlock(&m_mutex_work);
    return;
  }

  // Just to prevent further jobs to be added
  m_stopping = true;
  pthread_cond_signal(&m_cond_work);
  pthread_mutex_unlock(&m_mutex_work);
}

void yadfs::Worker::stopAndWaitCompletition()
{
  pthread_mutex_lock(&m_mutex_work);
  
  m_stopping = true;
  m_waiting = true;
  
    pthread_mutex_lock(&m_mutex_finished);

  pthread_cond_signal(&m_cond_work);
  pthread_mutex_unlock(&m_mutex_work);

    pthread_cond_wait(&m_cond_finished, &m_mutex_finished);
    pthread_mutex_unlock(&m_mutex_finished);

  return;
}




//void yadfs::Worker::stopAndWaitCompletition()
//{
//
//  Logging::log(Logging::INFO, "[stopAnd]trying to lock m_mutex_work");
//  pthread_mutex_lock(&m_mutex_work);
//
//  if (m_stopping)
//  {
//
//    Logging::log(Logging::INFO, "[stopAnd]Already stopping");
//    pthread_mutex_unlock(&m_mutex_work);
//    Logging::log(Logging::INFO, "[stopAnd]so exiting");
//    return;
//  }
//
//  m_stopping = true;
//  m_waiting = true;
//
//
//    Logging::log(Logging::INFO, "[stopAnd]Trying to lock m_mutex_finished");
//    pthread_mutex_lock(&m_mutex_finished);
//    Logging::log(Logging::INFO, "[stopAnd]Lock m_mutex_finished is mine");
//
//  // if (worker->m_queue.empty())
//  pthread_cond_signal(&m_cond_work);
//  pthread_mutex_unlock(&m_mutex_work);
//
//    pthread_cond_wait(&m_cond_finished, &m_mutex_finished);
//    pthread_mutex_unlock(&m_mutex_finished);
//
////  pthread_mutex_lock(&m_mutex_work);
////  m_stopping = true;
////  m_waiting = true;
////  pthread_cond_signal(&m_cond_work);
////  pthread_mutex_lock(&m_mutex_finished);
////  pthread_mutex_unlock(&m_mutex_work);
////  pthread_cond_wait(&m_cond_finished, &m_mutex_finished);
////  pthread_mutex_unlock(&m_mutex_finished);
//
//  return;
//}
