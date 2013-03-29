/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3SyslogLogger.cxx
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
#include <icetray/I3SimpleLoggers.h>

#include <syslog.h>
#include <boost/filesystem/path.hpp>

#include <icetray/I3Logging.h>


using namespace std;


I3SyslogLogger::I3SyslogLogger(I3LogLevel level)
  : I3Logger(level)
{
  openlog(NULL, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
}


I3SyslogLogger::~I3SyslogLogger()
{
  closelog();
}


void
I3SyslogLogger::Log(I3LogLevel level, const string& unit,
                    const string& file, int line, const string& func,
                    const string& message)
{
  int prio;
  const char* prio_c_str;

  if(LogLevelForUnit(unit) > level) return;

  switch(level)
  {
#pragma push_macro("LOG_TRACE")
#undef LOG_TRACE
    case LOG_TRACE:
#pragma pop_macro("LOG_TRACE")
      prio_c_str = "TRACE";
      prio = LOG_DEBUG;
      break;
#pragma push_macro("LOG_DEBUG")
#undef LOG_DEBUG
      case LOG_DEBUG:
#pragma pop_macro("LOG_DEBUG")
      prio_c_str = "DEBUG";
      prio = LOG_DEBUG;
      break;
#pragma push_macro("LOG_INFO")
#undef LOG_INFO
    case LOG_INFO:
#pragma pop_macro("LOG_INFO")
      prio_c_str = "INFO";
      prio = LOG_INFO;
      break;
#pragma push_macro("LOG_NOTICE")
#undef LOG_NOTICE
    case LOG_NOTICE:
#pragma pop_macro("LOG_NOTICE")
      prio_c_str = "NOTICE";
      prio = LOG_NOTICE;
     break;
#pragma push_macro("LOG_WARN")
#undef LOG_WARN
    case LOG_WARN:
#pragma pop_macro("LOG_WARN")
      prio_c_str = "WARN";
      prio = LOG_WARNING;
      break;
#pragma push_macro("LOG_ERROR")
#undef LOG_ERROR
    case LOG_ERROR:
#pragma pop_macro("LOG_ERROR")
      prio_c_str = "ERROR";
      prio = LOG_ERR;
      break;
#pragma push_macro("LOG_FATAL")
#undef LOG_FATAL
    case LOG_FATAL:
#pragma pop_macro("LOG_FATAL")
      prio_c_str = "FATAL";
      prio = LOG_CRIT;
      break;
    default:
      prio_c_str = "UNKNOWN";
      prio = LOG_DEBUG;
      break;
  }

  int messagesize = snprintf(NULL, 0, "%s (%s): %s (%s:%d in %s)",
                             prio_c_str,
                             unit.c_str(),
                             message.c_str(),
                             boost::filesystem::path(file).filename().c_str(),
                             line,
                             func.c_str());

  char log_message[messagesize + 1];
  sprintf(log_message,"%s (%s): %s (%s:%d in %s)",
          prio_c_str,
          unit.c_str(),
          message.c_str(),
          boost::filesystem::path(file).filename().c_str(),
          line,
          func.c_str());
  syslog(prio, "%s", log_message);
}
