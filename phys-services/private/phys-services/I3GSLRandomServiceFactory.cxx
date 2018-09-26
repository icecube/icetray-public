/*
 * class: I3GSLRandomServiceFactory
 *
 * Version $Id$
 *
 * Date: 17 Feb 2004
 *
 * (c) IceCube Collaboration
 */

// Class header files

#include "phys-services/I3GSLRandomServiceFactory.h"
I3_SERVICE_FACTORY(I3GSLRandomServiceFactory);

// Other header files

#include "phys-services/I3GSLRandomService.h"

// Constructors

I3GSLRandomServiceFactory::I3GSLRandomServiceFactory(const I3Context& context) 
  : I3ServiceFactory(context),
  seed_(gsl_rng_default_seed),
  track_state_(true),
  random_()
{
	AddParameter("Seed","Seed for random number generator", seed_);
	AddParameter("TrackState","If true, count calls to the random number generator"
	             " so that its state can be saved and restored. This imposes a "
	             "small run-time overhead.", track_state_);

	installServiceAs_ = I3DefaultName<I3RandomService>::value();
	AddParameter("InstallServiceAs",
	             "Install the random service at the following location",
	             installServiceAs_);
}

// Destructors

I3GSLRandomServiceFactory::~I3GSLRandomServiceFactory()
{
}

// Member functions

bool
I3GSLRandomServiceFactory::InstallService(I3Context& services)
{
  if(!random_)
    random_ = I3RandomServicePtr(new I3GSLRandomService(seed_));
  return services.Put<I3RandomService>(installServiceAs_, random_);
}

void I3GSLRandomServiceFactory::Configure()
{
  GetParameter("Seed", seed_);
  GetParameter("InstallServiceAs",installServiceAs_);
}
