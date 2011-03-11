/* 
 * File:   commons.hpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 10:08 PM
 */

#ifndef COMMONS_HPP
#define	COMMONS_HPP

#include <stdio.h>
#include <stdarg.h>

namespace yadfs
{

class Logging
{
public:
  enum Level { INFO, WARNING, ERROR };
  static void log(Level level, const char *fmt, ...)
  {
    switch (level)
    {
    case INFO:
      printf("[INFO]");
      break;
    case WARNING:
      printf("[WARNING]");
      break;
    case ERROR:
      printf("[ERROR]");
      break;
    default:
      printf("[UNKNOWN]");
    }

   va_list vl;
   va_start(vl,fmt);
   vprintf(fmt,vl);
   va_end(vl);
   printf("\n");
  }
};

}
#endif	/* COMMONS_HPP */

