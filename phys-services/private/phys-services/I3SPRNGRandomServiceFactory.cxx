/*
 * class: I3SPRNGRandomServiceFactory
 *
 * Version $Id$
 *
 * Date: 2 Apr 2005
 *
 * (c) IceCube Collaboration
 */

#include "phys-services/I3SPRNGRandomServiceFactory.h"
I3_SERVICE_FACTORY(I3SPRNGRandomServiceFactory);

#include "phys-services/I3SPRNGRandomService.h"
#include <sys/stat.h>

I3SPRNGRandomServiceFactory::I3SPRNGRandomServiceFactory(const I3Context& context)
  : I3ServiceFactory(context),
  seed_(0),
  nstreams_(1),
  streamnum_(0),
  instatefile_(),
  outstatefile_()
{
  AddParameter("Seed","Seed for random number generator.  NB : Use the same seed for different jobs. ",seed_);

  AddParameter("NStreams","Number of streams (i.e. N jobs) used in cluster",nstreams_);

  AddParameter("StreamNum","Thread number for this generator (i.e. job number)",streamnum_);

  AddParameter("inStateFile","If set, load saved state from file",instatefile_);

  AddParameter("outStateFile","If set, save state to file",outstatefile_);

  installServiceAs_ = I3DefaultName<I3RandomService>::value();
  AddParameter("InstallServiceAs",
               "Install the random service at the following location",
               installServiceAs_);
}

I3SPRNGRandomServiceFactory::~I3SPRNGRandomServiceFactory()
{
}

bool
I3SPRNGRandomServiceFactory::InstallService(I3Context& services)
{
  if (!random_)  {
	random_ = I3RandomServicePtr(
				new I3SPRNGRandomService(seed_, nstreams_,
						streamnum_, instatefile_, outstatefile_));
  }

  return services.Put<I3RandomService>(installServiceAs_, random_);
}

void I3SPRNGRandomServiceFactory::Configure()
{
  struct stat istat;

  GetParameter("Seed",seed_);
  GetParameter("NStreams",nstreams_);
  GetParameter("StreamNum",streamnum_);
  GetParameter("inStateFile",instatefile_);
  GetParameter("outStateFile",outstatefile_);
  GetParameter("InstallServiceAs",installServiceAs_);

  // do some configuration checks before trying to install
  if ( !(nstreams_ > streamnum_) ) {
     log_fatal("SPRNG: Number of RNG streams must be greater than the stream number");
  }

  // check rng state
  if (!instatefile_.empty()) { 
     log_debug("checking saved input RNG state %s", instatefile_.c_str());

     if (stat(instatefile_.c_str(), &istat)) { 
       log_fatal("SPRNG: Input RNG state file '%s' cannot be read!!!", instatefile_.c_str());
     }
  }
}
/* eof */
