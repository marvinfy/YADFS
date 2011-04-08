/* 
 * File:   utils.hpp
 * Author: marcusviniciusns
 *
 * Created on April 6, 2011, 11:46 PM
 */

#ifndef UTILS_HPP
#define	UTILS_HPP

#ifdef	__cplusplus

#include <string>
using std::string;
string *getParameter(const string& key);

extern "C"
{
#endif
void parse_argv(int argc, char *argv[]);
int get_parameter(const char *key, char *val, int size);
#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_HPP */

