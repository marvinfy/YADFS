#include "logging.hpp"

#include <stdio.h>
#include <stdarg.h>

void yadfs::Logging::log(Level level, const char *fmt, ...)
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
