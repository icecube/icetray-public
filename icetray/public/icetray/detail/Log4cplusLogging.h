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
#ifndef ICETRAY_LOG4CPLUSLOGGING_H_INCLUDED
#define ICETRAY_LOG4CPLUSLOGGING_H_INCLUDED

#include <string>
#include <stdio.h>

#undef PACKAGE
#undef HAVE_STAT
#include <log4cplus/logger.h>
// irritating, irritating stuff that doesn't belong in the public interface
// of log4cplus
#undef PACKAGE
#undef HAVE_STAT

/**
 * @brief A namespace used for the icetray logging system.
 */ 
namespace I3Logging 
{
  const size_t logbuf_size_ = 2048;
  extern char logbuf_[logbuf_size_]; 

  /**
   * @brief This class is used in configuring the logging system
   * 
   * Making an instance of this class will configure the logging system by
   * first checking for a log4cplus.conf pointed to by the I3LOGGING_CONFIG 
   * environment variable, then checking for a log4cplus.conf in 
   * $I3_BUILD/log4cplus.conf.  Failing both of these, it defaults to something
   * reasonable.
   */
  struct Configurator 
  {
    /**
     * Default constructor.
     */
    Configurator();
  };

};

/**
 * @brief A global function that will return a log4cplus::Logger for logging.  
 *
 * If a particular object wants a different logger it should implement 
 * a 'get_logger()' method so that the logging macros find it rather than
 * this global function.
 */
log4cplus::Logger get_logger();

// implmentation macro of macros visible to user.
#define LOG_IMPL(LEVEL, format, ...)					\
  get_logger().isEnabledFor(::log4cplus::LEVEL ## _LOG_LEVEL) &&	\
  snprintf(I3Logging::logbuf_, I3Logging::logbuf_size_, format, ##__VA_ARGS__), \
    get_logger().log(::log4cplus::LEVEL ## _LOG_LEVEL, I3Logging::logbuf_, __FILE__, __LINE__)
  
/**
 * @brief log a message of the 'trace' priority.
 *
 * Takes arguments like 'printf'.  This will be formatted by the logging system
 * so no need to specify things like line numbers.  Also the logging system 
 * appends a '\n' so no need to do that when using this macro.
 */
#ifndef I3_OPTIMIZE

#  define log_trace(format, ...) LOG_IMPL(TRACE, format, ##__VA_ARGS__)
#  define log_debug(format, ...) LOG_IMPL(DEBUG, format, ##__VA_ARGS__)
#  define log_info(format, ...) LOG_IMPL(INFO, format, ##__VA_ARGS__)

#else

#  define log_trace(format, ...)
#  define log_debug(format, ...)
#  define log_info(format, ...)

#endif

/**
 * @brief log a message of the 'warn' priority.
 *
 * Takes arguments like 'printf'.  This will be formatted by the logging system
 * so no need to specify things like line numbers.  Also the logging system 
 * appends a '\n' so no need to do that when using this macro.
 */
#define log_warn(format, ...) LOG_IMPL(WARN, format, ##__VA_ARGS__)

/**
 * @brief log a message of the 'error' priority.
 *
 * Takes arguments like 'printf'.  This will be formatted by the logging system
 * so no need to specify things like line numbers.  Also the logging system 
 * appends a '\n' so no need to do that when using this macro.
 */
#define log_error(format, ...) LOG_IMPL(ERROR, format, ##__VA_ARGS__)

/**
 * @brief log a message of the 'fatal' priority.
 *
 * Takes arguments like 'printf'.  This will be formatted by the logging system
 * so no need to specify things like line numbers.  Also the logging system 
 * appends a '\n' so no need to do that when using this macro.
 * This guy also throws a fatal error
 */
#define log_fatal(format, ...) LOG_IMPL(FATAL, format, ##__VA_ARGS__),	\
    throw std::runtime_error(I3Logging::logbuf_)

/**
 * @brief sets a logger for your class
 *
 * This macro adds a function to the current scope called get_logger,
 * which the log_* functions use to get the logger they log to.  The
 * idea is that if you want local control over your logger, you call
 * this in your class, like SET_LOGGER("I3LineFit"), and then you can
 * configure a logger called I3LineFit in your log4cplus.conf.  The
 * function must be static to ensure it can be called from static functions.
 *
 * This function has to contain the call to ::get_logger() to ensure that 
 * the logging system has been configured before we getInstance of our logger.
 */
#define SET_LOGGER(X)						\
  static log4cplus::Logger get_logger()					\
  {									\
    static log4cplus::Logger global_logger_ = ::get_logger();		\
    static log4cplus::Logger logger_(log4cplus::Logger::getInstance(X)); \
    return logger_;							\
  }

#endif //ndef ICETRAY_LOG4CPLUSLOGGING_H_INCLUDED
