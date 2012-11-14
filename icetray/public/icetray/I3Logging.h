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

#ifndef ICETRAY_I3LOGGING_H_INCLUDED
#define ICETRAY_I3LOGGING_H_INCLUDED

typedef enum {
	LOG_TRACE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
} I3LogLevel;

const I3LogLevel I3DefaultLogLevel = LOG_INFO;

#if defined(__cplusplus)

#include <icetray/I3PointerTypedefs.h>
#include <stdexcept>
#include <map>

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

#define I3_STREAM_LOGGER(level, id, file, line, func, msg, epilogue) \
    { std::ostringstream s; s << msg; GetIcetrayLogger()->Log(level, \
    id, file, line, func, s.str()); epilogue }

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
SET_LOGGER("Unknown");

#ifndef NDEBUG
#define log_trace(format, ...) I3_LOGGER(LOG_TRACE, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_debug(format, ...) I3_LOGGER(LOG_DEBUG, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_info(format, ...) I3_LOGGER(LOG_INFO, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#ifdef __cplusplus
#define log_trace_stream(msg) I3_STREAM_LOGGER(LOG_TRACE, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_debug_stream(msg) I3_STREAM_LOGGER(LOG_DEBUG, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_info_stream(msg) I3_STREAM_LOGGER(LOG_INFO, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#endif
#else
#define log_trace(format, ...)
#define log_debug(format, ...)
#define log_info(format, ...)
#define log_trace_stream(msg)
#define log_debug_stream(msg)
#define log_info_stream(msg)
#endif

#define log_warn(format, ...) I3_LOGGER(LOG_WARN, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)
#define log_error(format, ...) I3_LOGGER(LOG_ERROR, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__)

#ifdef __cplusplus
#define log_fatal(format, ...) I3_LOGGER(LOG_FATAL, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__), throw std::runtime_error(I3LoggingStringF(format, \
    ##__VA_ARGS__) + " (in " + __PRETTY_FUNCTION__ + ")")
#define log_warn_stream(msg) I3_STREAM_LOGGER(LOG_WARN, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_error_stream(msg) I3_STREAM_LOGGER(LOG_ERROR, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, )
#define log_fatal_stream(msg) I3_STREAM_LOGGER(LOG_FATAL, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, \
    throw std::runtime_error(s.str() + " (in " + __PRETTY_FUNCTION__ + ")");)
#else
#define log_fatal(format, ...) I3_LOGGER(LOG_FATAL, \
    __icetray_logger_id(), __FILE__, __LINE__, __PRETTY_FUNCTION__, format, \
    ##__VA_ARGS__), exit(1)
#endif

#endif //ifndef ICETRAY_I3LOGGING_H_INCLUDED
