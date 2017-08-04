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

TEST_GROUP(I3LoggingTest);

TEST(one)
{
  log_trace("here's a trace message");
  log_debug("here's a debug message");
  log_info("here's an info message");
  log_notice("here's a notice message");
  log_warn("here's a warn message");
  log_error("here's an error message");
  try {
    log_fatal("here's a fatal message");
    FAIL("log_fatal() should throw");
  } catch (std::exception& e) {
    // we should be here
  }
}


