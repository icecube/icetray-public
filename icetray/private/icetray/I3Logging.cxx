//
// $Id$
//
// @author troy d. straszheim
//
#include <icetray/I3Logging.h>

#if !defined(I3_PRINTF_LOGGING) && !defined(I3_PYTHON_LOGGING)

#include <string>
using std::string;
#include <iostream>
using std::cerr;

#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/logger.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/ndc.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/configurator.h>

#include <log4cplus/streams.h>
#include <log4cplus/spi/factory.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/helpers/stringhelper.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

//
//  Used only in this file to announce configuration steps
//
#define LOG(MSG) ::log4cplus::Logger::getRoot().log(::log4cplus::DEBUG_LOG_LEVEL, \
                                                    MSG,__FILE__,__LINE__);

namespace I3Logging
{
  class IceTrayAppender : public log4cplus::Appender
  {
   public:
    // Ctors
    IceTrayAppender(bool immediateFlush = false,
                    log4cplus::LogLevel errorLevel = log4cplus::ERROR_LOG_LEVEL)
      : immediateFlush_(immediateFlush), errorLevel_(errorLevel)
    {}

    IceTrayAppender(const log4cplus::helpers::Properties properties)
      : log4cplus::Appender(properties),
        immediateFlush_(false), errorLevel_(log4cplus::ERROR_LOG_LEVEL)
    {
      if (properties.exists( LOG4CPLUS_TEXT("ImmediateFlush") ))
      {
        log4cplus::tstring tmp =
          properties.getProperty( LOG4CPLUS_TEXT("ImmediateFlush") );
        immediateFlush_ =
          (log4cplus::helpers::toLower(tmp) == LOG4CPLUS_TEXT("true"));
      }
      if (properties.exists( LOG4CPLUS_TEXT("ErrorLevel") ))
      {
        log4cplus::tstring tmp =
          properties.getProperty( LOG4CPLUS_TEXT("ErrorLevel") );
        errorLevel_ = log4cplus::getLogLevelManager().fromString(tmp);
      }
    }

    // Dtor
    virtual ~IceTrayAppender() { destructorImpl(); }

    // Methods
    virtual void close()
    {
      getLogLog().debug(LOG4CPLUS_TEXT("Entering IceTrayAppender::close().."));
      closed = true;
    }

   protected:
    // From log4cplus::ConsoleAppender:
    // Normally, append() methods do not need to be locked since they are
    // called by doAppend() which performs the locking.  However, this locks
    // on the LogLog instance, so we don't have multiple threads writing to
    // tcout and tcerr
    void append(const log4cplus::spi::InternalLoggingEvent& event)
    {
      LOG4CPLUS_BEGIN_SYNCHRONIZE_ON_MUTEX( getLogLog().mutex )
        log4cplus::tostream& output =
          (event.getLogLevel() >= errorLevel_ ? log4cplus::tcerr : log4cplus::tcout);
        layout->formatAndAppend(output, event);
        if (immediateFlush_) output.flush();
      LOG4CPLUS_END_SYNCHRONIZE_ON_MUTEX ;
    }

    /**
     * Immediate flush means that the underlying output stream
     * will be flushed at the end of each append operation.
     */
    bool immediateFlush_;

    /**
     * Minimum logging level that goes to stderr.
     */
    log4cplus::LogLevel errorLevel_;
  };

  class IceTrayAppenderFactory : public log4cplus::spi::AppenderFactory
  {
   public:
    log4cplus::SharedAppenderPtr
    createObject(const log4cplus::helpers::Properties& props)
    {
      return log4cplus::SharedAppenderPtr(new I3Logging::IceTrayAppender(props));
    }

    log4cplus::tstring getTypeName()
    {
      return LOG4CPLUS_TEXT("I3Logging::IceTrayAppender");
    }
  };

  void failsafe_configuration()
  {
    log4cplus::helpers::Properties properties;

    // here's what you get if you use all-default configuration
    //properties.setProperty("log4cplus.appender.default", "log4cplus::ConsoleAppender");
    properties.setProperty("log4cplus.appender.default", "I3Logging::IceTrayAppender");
    properties.setProperty("log4cplus.appender.default.layout", "log4cplus::PatternLayout");
    properties.setProperty("log4cplus.appender.default.layout.ConversionPattern", "%F:%L: %m%n");
    //properties.setProperty("log4cplus.logger.default", "DEBUG, default");

    // no appender for root logger. it duplicates output.
    properties.setProperty("log4cplus.rootLogger", "WARN, default");

    log4cplus::PropertyConfigurator configurator(properties);
    configurator.configure();
    std::cerr << "Default logging configured (no log4cplus.conf is present).  Default threshold is WARN.\n";
  }

  void file_configuration(const string &config_fname)
  {
    log4cplus::PropertyConfigurator propertyconfigurator(config_fname);
    propertyconfigurator.configure();
    std::cerr << "Logging configured from file " << config_fname << "\n";
  }

  Configurator::Configurator()
  {
    // register our appender
    log4cplus::spi::AppenderFactoryRegistry& reg = log4cplus::spi::getAppenderFactoryRegistry();
    std::auto_ptr<log4cplus::spi::AppenderFactory> icetrayAppenderFactory(new I3Logging::IceTrayAppenderFactory());
    reg.put(icetrayAppenderFactory);

    // try the config file that I3LOGGING_CONFIG is pointing to
    const char* buffer = getenv("I3LOGGING_CONFIG");
    if (buffer)
    {
      fs::path propertyFile(buffer);
      if (!fs::exists(propertyFile) || fs::is_directory(propertyFile))
      {
        std::cerr << "Environment variable I3LOGGING_CONFIG which should point\n"
                  << "to a log4cplus configuration file points to a directory or\n"
                  << "something nonexistent.\n"
                  << "value is \"" << buffer << "\".\n"
                  << "Going to default configuration...\n";
        failsafe_configuration();
        return;
      }
      file_configuration(propertyFile.string());
      return;
    }

    // try the local config file
    {
      fs::path propertyFile("./log4cplus.conf");
      if (fs::exists(propertyFile) && !fs::is_directory(propertyFile))
      {
        file_configuration(propertyFile.string());
        return;
      }
    }

    // try the config file within the workspace
    buffer = getenv("I3_BUILD");
    if (buffer)
    {
      fs::path propertyFile(buffer);
      propertyFile /= "log4cplus.conf";
      if (fs::exists(propertyFile) && !fs::is_directory(propertyFile))
      {
        file_configuration(propertyFile.string());
        return;
      }
    }

    failsafe_configuration();
    LOG("Logging configured.");
  }

#ifdef I3_ONLINE
  boost::mutex Log4CPlusLogger::mtx_;
#endif
  char Log4CPlusLogger::buffer_[Log4CPlusLogger::BUFFER_SIZE];
}

I3Logging::Log4CPlusLogger get_logger()
{
  // this gets constructed only the first time this function is called
  static I3Logging::Configurator configurator;

  static I3Logging::Log4CPlusLogger globalLogger;

  return globalLogger;
}

extern "C" {
  void i3_clog_trace(const char* loggername, const char* msg)
  {
    log4cplus::Logger theLogger_(log4cplus::Logger::getInstance(loggername));
    theLogger_.log(::log4cplus::TRACE_LOG_LEVEL, msg);
  };
  void i3_clog_debug(const char* loggername, const char* msg)
  {
    log4cplus::Logger theLogger_(log4cplus::Logger::getInstance(loggername));
    theLogger_.log(::log4cplus::DEBUG_LOG_LEVEL, msg);
  };
  void i3_clog_info(const char* loggername, const char* msg)
  {
    log4cplus::Logger theLogger_(log4cplus::Logger::getInstance(loggername));
    theLogger_.log(::log4cplus::INFO_LOG_LEVEL, msg);
  };
  void i3_clog_warn(const char* loggername, const char* msg)
  {
    log4cplus::Logger theLogger_(log4cplus::Logger::getInstance(loggername));
    theLogger_.log(::log4cplus::WARN_LOG_LEVEL, msg);
  };
  void i3_clog_error(const char* loggername, const char* msg)
  {
    log4cplus::Logger theLogger_(log4cplus::Logger::getInstance(loggername));
    theLogger_.log(::log4cplus::ERROR_LOG_LEVEL, msg);
  };
  void i3_clog_fatal(const char* loggername, const char* msg)
  {
    log4cplus::Logger theLogger_(log4cplus::Logger::getInstance(loggername));
    theLogger_.log(::log4cplus::FATAL_LOG_LEVEL, msg);
  };
}

#else //if !defined(I3_PRINTF_LOGGING) && !defined(I3_PYTHON_LOGGING)

extern "C" {
  void i3_clog_trace(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_debug(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_info(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_warn(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_error(const char* loggername, const char* msg)
  {
    printf(msg);
  };
  void i3_clog_fatal(const char* loggername, const char* msg)
  {
    printf(msg);
  };
}

#endif //if !defined(I3_PRINTF_LOGGING) && !defined(I3_PYTHON_LOGGING)
