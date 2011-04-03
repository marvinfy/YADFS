/* 
 * File:   job.hpp
 * Author: marcusviniciusns
 *
 * Created on April 3, 2011, 2:46 AM
 */

#ifndef JOB_HPP
#define	JOB_HPP

namespace yadfs
{

class Job
{
private:
  void (*m_func)(void *);
  void *m_data;
public:
  Job(void (*func)(void *), void *data);
  Job(const Job& orig);
  virtual ~Job();
  void execute() const;
};

}
#endif	/* JOB_HPP */

