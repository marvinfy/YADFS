/* 
 * File:   commons.hpp
 * Author: marcusviniciusns
 *
 * Created on March 10, 2011, 10:08 PM
 */

#ifndef LOGGING_HPP
#define	LOGGING_HPP


#ifdef	__cplusplus
extern "C"
{
#endif

namespace yadfs
{

class Logging
{
public:
  enum Level { INFO, WARNING, ERROR };
  static void log(Level level, const char *fmt, ...);
};

}

#ifdef	__cplusplus
}
#endif





#endif	/* LOGGING_HPP */
