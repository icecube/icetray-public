//
// $Id$
//
// @author troy d. straszheim
//
#include <cstdio>

#include <icetray/I3Logging.h>
#include <icetray/I3SimpleLoggers.h>

I3PrintfLogger::I3PrintfLogger(I3LogLevel level)
    : I3Logger(level), TrimFileNames(true)
{
	tty_ = isatty(STDERR_FILENO);
}

void
I3PrintfLogger::Log(I3LogLevel level, const std::string &unit,
    const std::string &file, int line, const std::string &func,
    const std::string &message)
{
	const char *log_description;
	const char *log_prolog = "", *file_prolog = "", *log_epilog = "";

	if (LogLevelForUnit(unit) > level)
		return;

	if (tty_) {
		log_prolog = "\x1b[1m";
		file_prolog = "\x1b[1m";
		log_epilog = "\x1b[0m";
	}

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
		if (tty_)
			log_prolog = "\x1b[1;31m";
		break;
	case I3LOG_FATAL:
		log_description = "FATAL";
		if (tty_)
			log_prolog = "\x1b[1;31m";
		break;
	default:
		log_description = "UNKNOWN";
		break;
	}

	std::string trimmed_filename;
	size_t lastslash = file.rfind('/');
	if (lastslash != std::string::npos && TrimFileNames)
		trimmed_filename = file.substr(lastslash+1);
	else
		trimmed_filename = file;

	int messagesize = snprintf(NULL, 0,
	    "%s%s (%s):%s %s (%s%s:%d%s in %s%s%s)\n",
	    log_prolog, log_description, unit.c_str(), log_epilog,
	    message.c_str(), file_prolog, trimmed_filename.c_str(), line,
	    log_epilog, file_prolog, func.c_str(), log_epilog);

	char log_message[messagesize + 1];

	sprintf(log_message, "%s%s (%s):%s %s (%s%s:%d%s in %s%s%s)\n",
	    log_prolog, log_description, unit.c_str(), log_epilog,
	    message.c_str(), file_prolog, trimmed_filename.c_str(), line,
	    log_epilog, file_prolog, func.c_str(), log_epilog);

	fputs(log_message, stderr);
}

