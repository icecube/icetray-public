/**
 * Copyright  (C) 2004 the icecube collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 * $Id$
 *
 * @version $Revision$
 * @date $Date$
 * @author <a href="http://icecube.umd.edu/pretz">John Pretz</a>
 *
 * This is a test which tests that the services are available to modules during
 * the call of the module's destructor
 */

#include <I3Test.h>

#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"


TEST_GROUP(InstallService);

// something that installs this service with just one instance for
// every module
struct NameTestService : public I3ServiceFactory
{
  NameTestService(const I3Context& context)
    : I3ServiceFactory(context) { }

  bool InstallService(I3Context& s)
  {
    ENSURE_EQUAL(GetName(), "TESTNAME");
    return true;
  }
};

I3_SERVICE_FACTORY(NameTestService);

TEST(name)
{
  I3Tray tray;
  tray.AddService<NameTestService>("TESTNAME");

  tray.AddModule("BottomlessSource", "bs");

  tray.Execute(10);
}

TEST(check_service_installed)
{
  I3Tray tray;
  ENSURE(!tray.HasService("TESTNAME"),
               "The service should not be found before it is installed");
  tray.AddService<NameTestService>("TESTNAME");
  ENSURE(tray.HasService("TESTNAME"),
               "The service should be found after it is installed");
}