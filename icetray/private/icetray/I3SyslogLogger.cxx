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
#include <syslog.h>
#include <cstdio>

#include <boost/filesystem/path.hpp>

#include "icetray/I3Logging.h"
#include "icetray/I3SimpleLoggers.h"

using namespace std;

#ifdef I3_ONLINE
  boost::shared_mutex I3SyslogLogger::mtx;
#endif
bool I3SyslogLogger::isOpen = false;
std::string I3SyslogLogger::id;


void I3SyslogLogger::Open(const std::string& ident)
{
  Openlog(ident, true);
}


void I3SyslogLogger::Openlog(const std::string& ident, bool doThrow)
{
#ifdef I3_ONLINE
  boost::upgrade_lock<boost::shared_mutex> lock(mtx);
#endif
  if(!isOpen)
  {
#ifdef I3_ONLINE
    boost::upgrade_to_unique_lock<boost::shared_mutex> exclusive(lock);
#endif
    id = ident;
    openlog(id.empty() ? NULL : id.c_str(),
            LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
    isOpen = true;
  }
  else if(doThrow)
    throw logic_error("subsequent call to open the system logger");
}


I3SyslogLogger::I3SyslogLogger(I3LogLevel level)
  : I3Logger(level)
{
}


I3SyslogLogger::~I3SyslogLogger()
{
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
    case I3LOG_TRACE:
      prio_c_str = "TRACE";
      prio = LOG_DEBUG;
      break;
    case I3LOG_DEBUG:
      prio_c_str = "DEBUG";
      prio = LOG_DEBUG;
      break;
    case I3LOG_INFO:
      prio_c_str = "INFO";
      prio = LOG_INFO;
      break;
    case I3LOG_NOTICE:
      prio_c_str = "NOTICE";
      prio = LOG_NOTICE;
      break;
    case I3LOG_WARN:
      prio_c_str = "WARN";
      prio = LOG_WARNING;
      break;
    case I3LOG_ERROR:
      prio_c_str = "ERROR";
      prio = LOG_ERR;
      break;
    case I3LOG_FATAL:
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

  // syslog() is thread-safe for linux, but openlog() + syslog() too?
  // doesn't matter here, since openlog is always called here before any call
  // to syslog
  if(!IsOpen()) Openlog("", false);
  syslog(prio, "%s", log_message);
}
