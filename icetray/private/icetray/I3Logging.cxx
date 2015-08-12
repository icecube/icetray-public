//
// $Id$
//
// @author troy d. straszheim
//
#include <cstdio>
#include <cstdarg>

#include "icetray/I3Logging.h"
#include "icetray/I3SimpleLoggers.h"

#ifdef I3_ONLINE
#  include <boost/thread/locks.hpp>
#  include <boost/thread/mutex.hpp>


   static boost::mutex icetray_global_logger_mtx;
#endif
static I3LoggerPtr icetray_global_logger;

I3LoggerPtr
GetIcetrayLogger()
{
#ifdef I3_ONLINE
        // shared_ptr copy constructor is atomic
        I3LoggerPtr retVal(icetray_global_logger);
        if(!retVal)
        {
                boost::lock_guard<boost::mutex> lock(icetray_global_logger_mtx);
#endif
                if (!icetray_global_logger)
                        icetray_global_logger = I3LoggerPtr(new I3PrintfLogger);

                return icetray_global_logger;
#ifdef I3_ONLINE
        }

        return retVal;
#endif
}

void
SetIcetrayLogger(I3LoggerPtr logger)
{
#ifdef I3_ONLINE
        // shared_ptr assignment is atomic
        // but we might still run into a race condition,
        // that is logger is overwritten by a I3PrintfLogger
        boost::lock_guard<boost::mutex> lock(icetray_global_logger_mtx);
#endif
        icetray_global_logger = logger;
}

std::string
I3LoggingStringF(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	int messagesize = vsnprintf(NULL, 0, format, args);
	char log_message[messagesize + 1];

	va_start(args, format);
	vsprintf(log_message, format, args);

	return std::string(log_message);
}

I3Logger::I3Logger(I3LogLevel default_level) :
    default_log_level_(default_level) {}
I3Logger::~I3Logger() {}

I3LogLevel
I3Logger::LogLevelForUnit(const std::string &unit)
{
#ifdef I3_ONLINE
        boost::shared_lock<boost::shared_mutex> lock(mtx_);
#endif
	std::map<std::string, I3LogLevel>::const_iterator iter =
	    log_levels_.find(unit);
	if (iter == log_levels_.end())
		return default_log_level_;

	return iter->second;
}

void
I3Logger::SetLogLevelForUnit(const std::string &unit, I3LogLevel level)
{
#ifdef I3_ONLINE
        boost::unique_lock<boost::shared_mutex> lock(mtx_);
#endif
	log_levels_[unit] = level;
}

void
I3Logger::SetLogLevel(I3LogLevel level)
{
#ifdef I3_ONLINE
        boost::unique_lock<boost::shared_mutex> lock(mtx_);
#endif
	default_log_level_ = level;
}

I3BasicLogger::I3BasicLogger(I3LogLevel level)
    : I3Logger(level) {}

void
I3BasicLogger::Log(I3LogLevel level, const std::string &unit,
    const std::string &file, int line, const std::string &func,
    const std::string &message)
{
	const char *log_description;

	if (LogLevelForUnit(unit) > level)
		return;

	switch (level) {
	case I3LOG_TRACE:
		log_description = "TRACE";
		break;
	case I3LOG_DEBUG:
		log_description = "DEBUG";
		break;
	case I3LOG_INFO:
		log_description = "INFO";
		break;
        case I3LOG_NOTICE:
                log_description = "NOTICE";
                break;
	case I3LOG_WARN:
		log_description = "WARN";
		break;
	case I3LOG_ERROR:
		log_description = "ERROR";
		break;
	case I3LOG_FATAL:
		log_description = "FATAL";
		break;
	default:
		log_description = "UNKNOWN";
		break;
	}

	int messagesize = snprintf(NULL, 0, "%s (%s): %s (%s:%d in %s)",
	    log_description, unit.c_str(), message.c_str(), file.c_str(), line,
	    func.c_str());
	char log_message[messagesize + 1];
	sprintf(log_message, "%s (%s): %s (%s:%d in %s)", log_description,
	    unit.c_str(), message.c_str(), file.c_str(), line, func.c_str());

	BasicLog(log_message);
}

void
i3_clogger(I3LogLevel level, const char *unit, const char *file, int line,
    const char *func, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	int messagesize = vsnprintf(NULL, 0, format, args);
	char log_message[messagesize + 1];

	va_start(args, format);
	vsprintf(log_message, format, args);

	GetIcetrayLogger()->Log(level, unit, file, line, func, log_message);
}

