/*
 * File:   logging.cpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 9:47 PM
 */

#include "logging.hpp"

#include <stdio.h>
#include <stdarg.h>

void yadfs::Logging::log(Level level, const char *fmt, ...)
{
  switch (level)
  {
  case INFO:
    printf("[YADFS][INFO]");
    break;
  case WARNING:
    printf("[YADFS][WARNING]");
    break;
  case ERROR:
    printf("[YADFS][ERROR]");
    break;
  default:
    printf("[YADFS][UNKNOWN]");
  }

  va_list vl;
  va_start(vl, fmt);
  vprintf(fmt, vl);
  va_end(vl);
  printf("\n");
}
