/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author troy d. straszheim <troy@resophonic.com>
*/

#include <I3Test.h>
#include <icetray/I3Logging.h>

#include <string>
using std::string;
using std::cout;
using std::endl;

// the "0" ensures that this test is run first (ascii sort order).  A
// nasty hack.  I know.  But these order-of-initialization tests are
// nasty to begin with.

TEST_GROUP(I3LoggingTest3);

TEST(one)
{
  log_trace("here's a trace message");
  log_debug("here's a debug message");
  log_info("here's an info message");
  log_warn("here's a warn message");
  log_error("here's an error message");
  try {
    log_fatal("here's a fatal message");
  } catch (...) { }
}

struct I3GetLoggerTester 
{
  void say_something() 
  {
    log_trace("here's a trace message inside I3GetLoggerTester");
    log_debug("here's a debug message inside I3GetLoggerTester");
    log_info("here's an info message inside I3GetLoggerTester");
    log_warn("here's a warn message inside I3GetLoggerTester");
    log_error("here's an error message inside I3GetLoggerTester");
    try {
      log_fatal("here's a fatal message inside I3GetLoggerTester");
    } catch (...) { }
  }
  SET_LOGGER("I3GetLoggerTester");
};

// verifies that if the first object to log something is 
// via SET_LOGGER, the logging system is propery configured.
TEST(0_SET_LOGGER_logging_first)
{
  I3GetLoggerTester t;
  t.say_something();
}


