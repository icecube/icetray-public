/**
 *  Copyright (C) 2012 Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
 *  Copyright (C) 2012 the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  SPDX-License-Identifier: BSD-2-Clause
 */

#include <boost/python.hpp>
#include <boost/preprocessor.hpp>
#include <icetray/python/gil_holder.hpp>

#include <icetray/I3Logging.h>
#include <icetray/I3SimpleLoggers.h>

using namespace boost::python;

class I3LoggerWrapper : public I3Logger, public wrapper<I3Logger> {
public:
	void Log(I3LogLevel level, const std::string &unit,
	    const std::string &file, int line, const std::string &func,
	    const std::string &log_message)
	{
		detail::gil_holder gil;
		detail::restore_exceptions restore;
		try {
			if (override f = this->get_override("log")) {
				f(level, unit, file, line, func, log_message);
			} else {
				RAISE(NotImplementedError, "I3LoggerBase subclasses must implement log()");
			}
		} catch (error_already_set &e) {
			restore.set_raised_exception();
			throw;
		}
	}

	I3LogLevel LogLevelForUnit(const std::string &unit)
	{
		detail::gil_holder gil;
		if (override f = this->get_override("get_level_for_unit")) {
			return f(unit);
		} else {
			return I3Logger::LogLevelForUnit(unit);
		}
	}

	void SetLogLevelForUnit(const std::string &unit, I3LogLevel level)
	{
		detail::gil_holder gil;
		if (override f = this->get_override("set_level_for_unit")) {
			f(unit, level);
		} else {
			I3Logger::SetLogLevelForUnit(unit, level);
		}
	}

	void SetLogLevel(I3LogLevel level)
	{
		detail::gil_holder gil;
		if (override f = this->get_override("set_level")) {
			f(level);
		} else {
			I3Logger::SetLogLevel(level);
		}
	}
};

static I3LogLevel
GlobalLogLevelForUnit(const std::string &unit)
{
	return GetIcetrayLogger()->LogLevelForUnit(unit);
}

void register_I3Logging()
{

	enum_<I3LogLevel>("I3LogLevel")
		.value("LOG_TRACE",  I3LOG_TRACE)
		.value("LOG_DEBUG",  I3LOG_DEBUG)
		.value("LOG_INFO",   I3LOG_INFO)
                .value("LOG_NOTICE", I3LOG_NOTICE)
		.value("LOG_WARN",   I3LOG_WARN)
		.value("LOG_ERROR",  I3LOG_ERROR)
		.value("LOG_FATAL",  I3LOG_FATAL)
	;

	class_<I3LoggerWrapper, boost::shared_ptr<I3LoggerWrapper>, boost::noncopyable>
	    ("I3Logger", "Logging base class")
		.add_static_property("global_logger", &GetIcetrayLogger, &SetIcetrayLogger)
		.def("log", &I3Logger::Log)
		.def("get_level_for_unit", &I3Logger::LogLevelForUnit)
		.def("set_level_for_unit", &I3Logger::SetLogLevelForUnit)
		.def("set_level", &I3Logger::SetLogLevel)
	;
	// explicitly register abstract base class ptr. if the base were concrete,
	// we could have done this implicitly by specifying
	// boost::shared_ptr<I3Logger> as the second template arg to
	// class_ above.
	register_ptr_to_python< boost::shared_ptr<I3Logger> >();

	class_<I3NullLogger, bases<I3Logger>, boost::shared_ptr<I3NullLogger>, boost::noncopyable>("I3NullLogger", "Logger that does not log. Useful if you don't want log messages");
	class_<I3PrintfLogger, bases<I3Logger>, boost::shared_ptr<I3PrintfLogger>, boost::noncopyable>("I3PrintfLogger", "Logger that prints error messages to stderr (in color, if stderr is a tty).", init<optional<I3LogLevel> >())
		.def_readwrite("trim_file_names", &I3PrintfLogger::TrimFileNames)
	;
	class_<I3SyslogLogger, bases<I3Logger>, boost::shared_ptr<I3SyslogLogger>, boost::noncopyable>("I3SyslogLogger", "Logger that generates log messages, which will be distributed by syslogd.", init<optional<I3LogLevel> >())
                .def("open", &I3SyslogLogger::Open)
                .staticmethod("open")
        ;


	def("get_log_level_for_unit", &GlobalLogLevelForUnit);

}
