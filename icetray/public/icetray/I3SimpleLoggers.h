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

#ifndef ICETRAY_I3SIMPLELOGGERS_H_INCLUDED
#define ICETRAY_I3SIMPLELOGGERS_H_INCLUDED

#include <stdexcept>
#include <fstream>
#ifdef I3_ONLINE
#  include <boost/thread/locks.hpp>
#  include <boost/thread/shared_mutex.hpp>
#endif

#include <icetray/I3Logging.h>

class I3NullLogger : public I3Logger {
public:
	virtual void Log(I3LogLevel level, const std::string &unit,
	    const std::string &file, int line, const std::string &func,
	    const std::string &message) {};
};

class I3PrintfLogger : public I3Logger {
public:
	I3PrintfLogger(I3LogLevel default_level = I3DefaultLogLevel);
	virtual void Log(I3LogLevel level, const std::string &unit,
	    const std::string &file, int line, const std::string &func,
	    const std::string &message);

	bool TrimFileNames;
private:
	bool tty_;
};

/**
 * @brief Logger that generates log messages, which will be distributed by syslogd.
 *
 * @note
 * I3SyslogLogger is thread-safe, however system calls <VAR>openlog</VAR> and
 * <VAR>closelog</VAR> must not be used from any other program location.
 * Violations may result in an undefined behavior.
 */
class I3SyslogLogger : public I3Logger {
private:
#ifdef I3_ONLINE
        static boost::shared_mutex mtx;
#endif
        static bool isOpen;
        static std::string id;


        static bool IsOpen()
        {
#ifdef I3_ONLINE
            boost::shared_lock<boost::shared_mutex> lock(mtx);
#endif
            return(isOpen);
        }

        static void Openlog(const std::string& ident, bool doThrow);

public:
        /**
         * @brief Opens a connection to the system logger for all
         *        I3SyslogLogger instances.
         *
         * The connection is opened automatically during the first call
         * to I3SyslogLogger::Log - however, an empty identification string
         * is used in that case.
         * @param ident is prepended to every message, and is typically set
         * to the program name.
         * If <VAR>ident</VAR> is "", the default identification string used
         * in syslog messages will be the program name, taken from argv[0].
         * @throw std::logic_error in case of subsequent calls to <VAR>Open</VAR>.
         *
         * @note
         * I3SyslogLogger uses the LOG_USER log facility.
         * It writes directly to system console if there is an error
         * while sending to syslogd,
         * it opens the connection immediately
         * and not just before the first message is logged, and
         * it includes PID with each message.
         *
         * @attention
         * The connection to the system logger must not be opened twice.
         */
        static void Open(const std::string& ident);

        /**
         * @brief Constructor.
         *
         * @param default_level The log level that is assigned to any
         * logging unit (cf. I3Logger) that has no explicit log level set
         * (cf. I3Logger::SetLogLevelForUnit)
         * (optional; default value is <VAR>I3DefaultLogLevel</VAR>).
         */
        explicit I3SyslogLogger(I3LogLevel default_level = I3DefaultLogLevel);
        /**
         * @brief Destructor.
         */
        virtual ~I3SyslogLogger();
        virtual void Log(I3LogLevel level, const std::string &unit,
            const std::string &file, int line, const std::string &func,
            const std::string &message);
};

/**
 * A logger which writes messages to a file.
 */
class I3FileLogger : public I3Logger{
private:
	std::string path;
	std::ofstream out;
	bool TrimFileNames;
public:
	/**
	 * Construct a file logger. The chosen output file will be overwritten.
	 * @param path the filesystem path to which to write the log
	 */
	explicit I3FileLogger(std::string path):path(path),out(path.c_str()),TrimFileNames(true){
		if(!out)
			throw std::runtime_error("Failed to open "+path+" for logging");
	}
	void Log(I3LogLevel level, const std::string &unit,
			 const std::string &file, int line, const std::string &func,
			 const std::string &message);
};

#endif //ifndef ICETRAY_I3SIMPLELOGGERS_H_INCLUDED
