/* 
 * File:   job.cpp
 * Author: marcusviniciusns
 * 
 * Created on April 3, 2011, 2:46 AM
 */

#include "job.hpp"

yadfs::Job::Job(void (*func)(void *), void *data)
{
  m_func = func;
  m_data = data;
}

yadfs::Job::Job(const Job& orig)
{
}

yadfs::Job::~Job()
{
}

void yadfs::Job::execute() const
{
  m_func(m_data);
}
