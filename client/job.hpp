/* 
 * File:   job.hpp
 * Author: marcusviniciusns
 *
 * Created on April 3, 2011, 2:46 AM
 */

#ifndef JOB_HPP
#define	JOB_HPP

#include <iostream>

using std::cout;

namespace yadfs
{

class Job
{
private:
public:
  Job()
  {
  }
  Job(const Job& orig)
  {
  }
  virtual ~Job()
  {
  }

  void execute() const
  {
    cout << "Executando trabalho!\n";
  }
};

}
#endif	/* JOB_HPP */

