/**
 *    $Id$
 *
 *    Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
 *    and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    
 *    THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *    SUCH DAMAGE.
 *    
 *    SPDX-License-Identifier: BSD-2-Clause
 *    
 */

#ifndef ICETRAY_I3LOGGING_H_INCLUDED
#define ICETRAY_I3LOGGING_H_INCLUDED

#include <signal.h>
#ifdef I3_ONLINE
#  include <boost/thread/shared_mutex.hpp>
#endif

typedef enum {
	I3LOG_TRACE,
	I3LOG_DEBUG,
	I3LOG_INFO,
	I3LOG_NOTICE,
	I3LOG_WARN,
	I3LOG_ERROR,
	I3LOG_FATAL
} I3LogLevel;

#if defined(__cplusplus)

#ifdef NDEBUG
const I3LogLevel I3DefaultLogLevel = I3LOG_NOTICE;
#else
const I3LogLevel I3DefaultLogLevel = I3LOG_INFO;
#endif

#include <icetray/I3PointerTypedefs.h>
#include <stdexcept>
#include <map>
#include <sstream>

class I3Logger {
public:
	I3Logger(I3LogLevel default_level = I3DefaultLogLevel);
	virtual ~I3Logger();

	virtual void Log(I3LogLevel level, const std::string &unit,
	    const std::string &file, int line, const std::string &func,
	    const std::string &message) = 0;

	virtual I3LogLevel LogLevelForUnit(const std::string &unit);
	virtual void SetLogLevelForUnit(const std::string &unit,
	    I3LogLevel level);

	virtual void SetLogLevel(I3LogLevel level);
private:
#ifdef I3_ONLINE
        boost::shared_mutex mtx_;
#endif
	std::map<std::string, I3LogLevel> log_levels_;
	I3LogLevel default_log_level_;
};

class I3BasicLogger : public I3Logger {
public:
	I3BasicLogger(I3LogLevel default_level = I3DefaultLogLevel);

	virtual void Log(I3LogLevel level, const std::string &unit,
	    const std::string &file, int line, const std::string &func,
	    const std::string &message);
	virtual void BasicLog(const std::string &string) = 0;
};

I3_POINTER_TYPEDEFS(I3Logger);

// Root logger. If class/namespace has a method with the same name in scope,
// it can have its own logger.
I3LoggerPtr GetIcetrayLogger();
void SetIcetrayLogger(I3LoggerPtr);

std::string I3LoggingStringF(const char *format, ...)
    __attribute__((__format__ (__printf__, 1, 2)));
#define I3_LOGGER(level, id, file, line, func, format, ...) \
    GetIcetrayLogger()->Log(level, id, file, line, func, \
    I3LoggingStringF(format, ##__VA_ARGS__))

#define I3_HIFREQ_LOGGER(level, id, file, line, func, format, ...) \
    do { if(GetIcetrayLogger()->LogLevelForUnit(id)<=level) \
    GetIcetrayLogger()->Log(level, id, file, line, func, \
    I3LoggingStringF(format, ##__VA_ARGS__)); \
    } while (0)

#define I3_STREAM_LOGGER(level, id, file, line, func, msg, epilogue) \
    do { std::ostringstream _i3_str_logger_str; _i3_str_logger_str << msg; GetIcetrayLogger()->Log(level, \
    id, file, line, func, _i3_str_logger_str.str()); epilogue } while (0)

#define I3_HIFREQ_STREAM_LOGGER(level, id, file, line, func, msg, epilogue) \
    do { if(GetIcetrayLogger()->LogLevelForUnit(id)<=level) { \
    std::ostringstream _i3_str_logger_str; _i3_str_logger_str << msg; GetIcetrayLogger()->Log(level, \
    id, file, line, func, _i3_str_logger_str.str()); epilogue } } while (0)

extern "C" {
#endif // __cplusplus
void i3_clogger(I3LogLevel level, const char *unit, const char *file,
    int line, const char *func, const char *format, ...)
    __attribute__((__format__ (__printf__, 6, 7)));
#ifdef __cplusplus
}
#else
#define I3_LOGGER i3_clogger
#endif

#define SET_LOGGER(X) \
	static inline const char *__icetray_logger_id(void) { return X; }

// Set default logger in global namespace
#ifdef PROJECT
#define I3LOGSTR1(X) #X
#define I3LOGSTR(X) I3LOGSTR1(X)
SET_LOGGER(I3LOGSTR(PROJECT));
#undef I3LOGSTR
#undef I3LOGSTR1
#else
SET_LOGGER("Unknown");
#endif

#define log_custom(level, format, ...) I3_LOGGER(level, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_custom_unit(level, unit, format, ...) I3_LOGGER(level, \
    unit, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)

#ifndef I3_COMPILE_OUT_VERBOSE_LOGGING
#define log_trace(format, ...) I3_HIFREQ_LOGGER(I3LOG_TRACE, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_debug(format, ...) I3_HIFREQ_LOGGER(I3LOG_DEBUG, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#ifdef __cplusplus
#define log_trace_stream(msg) I3_HIFREQ_STREAM_LOGGER(I3LOG_TRACE, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_debug_stream(msg) I3_HIFREQ_STREAM_LOGGER(I3LOG_DEBUG, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#endif
#else
#define log_trace(format, ...)
#define log_debug(format, ...)
#define log_trace_stream(msg)
#define log_debug_stream(msg)
#endif

#define log_info(format, ...) I3_LOGGER(I3LOG_INFO, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_notice(format, ...) I3_LOGGER(I3LOG_NOTICE, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_warn(format, ...) I3_LOGGER(I3LOG_WARN, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_error(format, ...) I3_LOGGER(I3LOG_ERROR, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)

#ifdef __cplusplus
#define log_fatal(format, ...) I3_LOGGER(I3LOG_FATAL, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__), throw std::runtime_error(I3LoggingStringF(format, \
    ##__VA_ARGS__) + " (in " + __PRETTY_FUNCTION__ + ")")
#define log_info_stream(msg) I3_STREAM_LOGGER(I3LOG_INFO, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_notice_stream(msg) I3_STREAM_LOGGER(I3LOG_NOTICE, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_warn_stream(msg) I3_STREAM_LOGGER(I3LOG_WARN, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_error_stream(msg) I3_STREAM_LOGGER(I3LOG_ERROR, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_fatal_stream(msg) I3_STREAM_LOGGER(I3LOG_FATAL, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, \
    throw std::runtime_error(_i3_str_logger_str.str() + " (in " + __PRETTY_FUNCTION__ + ")");)
#else
#define log_fatal(format, ...) I3_LOGGER(I3LOG_FATAL, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__), kill(getpid(), SIGABRT)
#endif

#define i3_assert(cond) do{ if(!(cond)) log_fatal("Assertion failed: %s", #cond); } while(0)
#ifdef NDEBUG
#define i3_debug_assert(cond) do{ /*nothing*/ } while(0)
#else
#define i3_debug_assert(cond) do{ if(!(cond)) log_fatal("Assertion failed: %s", #cond); } while(0)
#endif

#endif //ifndef ICETRAY_I3LOGGING_H_INCLUDED
