#include <icetray/I3SimpleLoggers.h>

void I3FileLogger::Log(I3LogLevel level, const std::string &unit,
		 const std::string &file, int line, const std::string &func,
		 const std::string &message)
{
    if (LogLevelForUnit(unit) > level)
		return;
    std::string trimmed_filename;
	size_t lastslash = file.rfind('/');
	if (lastslash != std::string::npos && TrimFileNames)
		trimmed_filename = file.substr(lastslash+1);
	else
		trimmed_filename = file;
    switch (level) {
		case I3LOG_TRACE:
			out << "TRACE";
			break;
		case I3LOG_DEBUG:
			out << "DEBUG";
			break;
		case I3LOG_INFO:
			out << "INFO";
			break;
		case I3LOG_NOTICE:
			out << "NOTICE";
			break;
		case I3LOG_WARN:
			out << "WARN";
			break;
		case I3LOG_ERROR:
			out << "ERROR";
			break;
		case I3LOG_FATAL:
			out << "FATAL";
			break;
		default:
			out << "UNKNOWN";
			break;
	}
    out << " (" << unit << "):" << message << " (" << trimmed_filename << ':'
    << line << " in " << func << ')' << std::endl;
}
