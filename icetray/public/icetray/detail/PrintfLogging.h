/**
 *    $Id$
 *
 *    Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
 *    and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *    
 *    This file is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *    
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//
//   Just log to printf, no logging level checks or logging library.
//   Useful when chasing strange crashes that might be due to tool
//   conflicts.
//
#ifndef ICETRAY_DETAIL_PRINTFLOGGING_H_INCLUDED
#define ICETRAY_DETAIL_PRINTFLOGGING_H_INCLUDED

#include <stdexcept>
#include <stdio.h>

#define LOG_TRACE 0
#define LOG_DEBUG 1
#define LOG_INFO  2
#define LOG_WARN  3
#define LOG_ERROR 4
#define LOG_FATAL 5

// implmentation macro of macros visible to user.
#define LOG_IMPL(LEVEL, format, ...) printf("%s:%d\t", __FILE__, __LINE__), printf(format, ##__VA_ARGS__ ), printf("\n")
  
#if I3_PRINTF_LOGGING_LEVEL <= LOG_TRACE
#  define log_trace(format, ...) LOG_IMPL(LOG_TRACE, format, ##__VA_ARGS__)
#else
#  define log_trace(format, ...)
#endif

#if I3_PRINTF_LOGGING_LEVEL <= LOG_DEBUG
#  define log_debug(format, ...) LOG_IMPL(LOG_DEBUG, format, ##__VA_ARGS__)
#else
#  define log_debug(format, ...)
#endif

#if I3_PRINTF_LOGGING_LEVEL <= LOG_INFO
#  define log_info(format, ...) LOG_IMPL(LOG_INFO, format, ##__VA_ARGS__)
#else
#  define log_info(format, ...)
#endif

#if I3_PRINTF_LOGGING_LEVEL <= LOG_WARN
#  define log_warn(format, ...) LOG_IMPL(LOG_WARN, format, ##__VA_ARGS__)
#else
#  define log_warn(format, ...)
#endif

#if I3_PRINTF_LOGGING_LEVEL <= LOG_ERROR
#  define log_error(format, ...) LOG_IMPL(LOG_ERROR, format, ##__VA_ARGS__)
#else
#  define log_error(format, ...)
#endif

#define log_fatal(format, ...) LOG_IMPL(LOG_FATAL, format, ##__VA_ARGS__), throw std::runtime_error("log_fatal")

#define SET_LOGGER(X)

#endif //ifndef ICETRAY_DETAIL_PRINTFLOGGING_H_INCLUDED
