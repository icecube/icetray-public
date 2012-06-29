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

#include <stdexcept>
#include <string>
#include <stdarg.h>
#include <stdio.h>

#ifdef I3_ONLINE
#  include <boost/thread/locks.hpp>
#  include <boost/thread/mutex.hpp>
#endif

// we must #undef a bunch of crap that doesn't belong in the public
// interface of log4cplus

#undef VERSION
#undef PACKAGE
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef HAVE_STAT

#include <log4cplus/logger.h>

#undef VERSION
#undef PACKAGE
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef HAVE_STAT

/**
 * @brief A namespace used for the icetray logging system.
 */ 
namespace I3Logging 
{
  /**
   * @brief This class is used in configuring the logging system
   * 
   * Making an instance of this class will configure the logging system by
   * first checking for a log4cplus.conf pointed to by the I3LOGGING_CONFIG 
   * environment variable, then checking for ./log4cplus.conf and
   * $I3_BUILD/log4cplus.conf finally. Failing all of these, it defaults
   * to something reasonable.
   */
  struct Configurator 
  {
    /**
     * Default constructor.
     */
    Configurator();
  };
  
  /** This is a wrapper for a <em>log4cplus</em> logger.
   */
  class Log4CPlusLogger
  {
   private:
    static const size_t BUFFER_SIZE = 2048;
#ifdef I3_ONLINE
    static boost::mutex mtx_;
#endif
    static char buffer_[BUFFER_SIZE];
    
    
    log4cplus::Logger logger_;
    
   public:
    /** Default constructor.
     */
    Log4CPlusLogger() : logger_(log4cplus::Logger::getRoot()) {}
    /** Constructor.
     * 
     * @param name Use a logger named according to <VAR>name</VAR>.
     */
    Log4CPlusLogger(const log4cplus::tstring& name) : logger_(log4cplus::Logger::getInstance(name)) {}
    /** Destructor.
     */
    ~Log4CPlusLogger() {}
    /** Set the minimum log level of the logger.
     *
     * @param ll The log level.
     */
    void setLogLevel(log4cplus::LogLevel ll)
    {
      logger_.setLogLevel(ll);
    }

    log4cplus::LogLevel getLogLevel()
    {
      return logger_.getLogLevel();
    }
    /** Create and log a message.
     *
     * @note
     * This generic method is intended to be used by wrappers
     * such as log_trace(), log_debug(), log_info(), log_warn(), log_error()
     * and log_fatal().
     * 
     * @param ll The log level of this message. A call to Log() is ignored
     *           if <VAR>ll</VAR> is less than the minimum log level of the logger.
     * @param file A file name and
     * @param line a line number that is assigned to the created message.
     * @param format A format string and
     * @param ... an arbitrary number of varied data type parameter(s) as for 'printf'.
     */
    void Log(log4cplus::LogLevel ll, const char* file, int line,
             const char* format, ...)
    {
      if(logger_.isEnabledFor(ll))
      {
#ifdef I3_ONLINE
        boost::lock_guard<boost::mutex> g(mtx_);
#endif
        va_list vl;
        va_start(vl, format);
        vsnprintf(buffer_, BUFFER_SIZE, format, vl);
        va_end(vl);
        logger_.log(ll, buffer_, file, line);
      }
    }
    /** Create, log and return a message.
     *
     * @note
     * This generic method is intended to be used by wrappers
     * such as log_trace(), log_debug(), log_info(), log_warn(), log_error()
     * and log_fatal().
     *
     * @param ll The log level of this message. A call to Log() is ignored
     *           if <VAR>ll</VAR> is less than the minimum log level of the logger.
     * @param file A file name and
     * @param line a line number that is assigned to the created message.
     * @param format A format string and
     * @param ... an arbitrary number of varied data type parameter(s) as for 'printf'.
     * @return The created message.
     */
    std::string LogAndReturn(log4cplus::LogLevel ll, const char* file, int line,
                             const char* format, ...)
    {
#ifdef I3_ONLINE
      boost::lock_guard<boost::mutex> g(mtx_);
#endif
      va_list vl;
      va_start(vl, format);
      vsnprintf(buffer_, BUFFER_SIZE, format, vl);
      va_end(vl);
      if(logger_.isEnabledFor(ll))
        logger_.log(ll, buffer_, file, line);
      
      return buffer_;
    }
  };
}

/**
 * @brief A global function that will return a I3Logging::Log4CPlusLogger for logging.
 *
 * If a particular object wants a different logger it should implement 
 * a 'get_logger()' method so that the logging macros find it rather than
 * this global function.
 */
I3Logging::Log4CPlusLogger get_logger();

// implmentation macro of macros visible to user (with the exception of log_fatal).
#define LOG_IMPL(LEVEL, format, ...)                                            \
  get_logger().Log(::log4cplus::LEVEL ## _LOG_LEVEL, __FILE__, __LINE__,        \
                   format, ##__VA_ARGS__)

#ifndef NDEBUG

/**
 * @brief log a message of the 'trace' priority.
 *
 * Takes arguments like 'printf'. This will be formatted by the logging system
 * so no need to specify things like line numbers. Also the logging system
 * appends a '\n' so no need to do that when using this macro.
 */
#  define log_trace(format, ...) LOG_IMPL(TRACE, format, ##__VA_ARGS__)
/**
 * @brief log a message of the 'debug' priority.
 *
 * Takes arguments like 'printf'. This will be formatted by the logging system
 * so no need to specify things like line numbers. Also the logging system
 * appends a '\n' so no need to do that when using this macro.
 */
#  define log_debug(format, ...) LOG_IMPL(DEBUG, format, ##__VA_ARGS__)
/**
 * @brief log a message of the 'info' priority.
 *
 * Takes arguments like 'printf'. This will be formatted by the logging system
 * so no need to specify things like line numbers. Also the logging system
 * appends a '\n' so no need to do that when using this macro.
 */
#  define log_info(format, ...) LOG_IMPL(INFO, format, ##__VA_ARGS__)

#else  //ifndef NDEBUG

#  define log_trace(format, ...)
#  define log_debug(format, ...)
#  define log_info(format, ...)

#endif  //ifndef NDEBUG

/**
 * @brief log a message of the 'warn' priority.
 *
 * Takes arguments like 'printf'. This will be formatted by the logging system
 * so no need to specify things like line numbers. Also the logging system
 * appends a '\n' so no need to do that when using this macro.
 */
#define log_warn(format, ...) LOG_IMPL(WARN, format, ##__VA_ARGS__)

/**
 * @brief log a message of the 'error' priority.
 *
 * Takes arguments like 'printf'. This will be formatted by the logging system
 * so no need to specify things like line numbers. Also the logging system
 * appends a '\n' so no need to do that when using this macro.
 */
#define log_error(format, ...) LOG_IMPL(ERROR, format, ##__VA_ARGS__)

/**
 * @brief log a message of the 'fatal' priority.
 *
 * Takes arguments like 'printf'. This will be formatted by the logging system
 * so no need to specify things like line numbers. Also the logging system
 * appends a '\n' so no need to do that when using this macro.
 * This guy also throws a fatal error
 */
#define log_fatal(format, ...)                                                  \
  throw std::runtime_error(get_logger().LogAndReturn(log4cplus::FATAL_LOG_LEVEL,\
                                                     __FILE__, __LINE__,        \
                                                     format, ##__VA_ARGS__))

/**
 * @brief sets a logger for your class
 *
 * This macro adds a function to the current scope called get_logger,
 * which the log_* functions use to get the logger they log to. The
 * idea is that if you want local control over your logger, you call
 * this in your class, like SET_LOGGER("I3LineFit"), and then you can
 * configure a logger called I3LineFit in your log4cplus.conf. The
 * function must be static to ensure it can be called from static functions.
 *
 * This function has to contain the call to ::get_logger() to ensure that 
 * the logging system has been configured before we getInstance of our logger.
 */
#define SET_LOGGER(X)                                                           \
  static I3Logging::Log4CPlusLogger get_logger()                                \
  {                                                                             \
    static I3Logging::Log4CPlusLogger global_logger(::get_logger());            \
    static I3Logging::Log4CPlusLogger logger(X);                                \
                                                                                \
    return logger;                                                              \
  }

#endif //ifndef ICETRAY_LOG4CPLUSLOGGING_H_INCLUDED
