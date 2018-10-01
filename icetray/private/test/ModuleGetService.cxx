/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @version $Revision$
 * @date $Date$
 * @author <a href="http://icecube.umd.edu/pretz">John Pretz</a>
 *
 * This tests getting services by various means through module parameters.
 */

#include <I3Test.h>

#include "icetray/I3Tray.h"
#include "icetray/I3ServiceFactory.h"

#include <set>

TEST_GROUP(ModuleGetService);

static int service_sentinel = 5;

class IntegerService : public I3ServiceFactory {
	public:
		IntegerService(const I3Context &c) : I3ServiceFactory(c) {}
		bool InstallService(I3Context& services) {
			boost::shared_ptr<int> service(new
			    int(service_sentinel));
			services.Put(service, "int");
			return true;
		}
};

// a module to check for whether or not the service exists
class ServiceTest : public I3Module {
	public:
		ServiceTest(const I3Context& context) : I3Module(context) {
			AddParameter("Int", "Integer to test", -1);
		}

		void Configure() {
			int value;
			GetParameter("Int", value);
			ENSURE(service_sentinel == value);
		}
		
		void Process() {
			RequestSuspension();
		}
};

I3_MODULE(ServiceTest);
I3_SERVICE_FACTORY(IntegerService);

TEST(getting_services_in_modules)
{
  I3Tray tray;

  tray.AddService("IntegerService");
  tray.AddModule("ServiceTest")("Int", service_sentinel);
  tray.AddModule("ServiceTest")("Int", "int");
    
  tray.Execute();
    
}

