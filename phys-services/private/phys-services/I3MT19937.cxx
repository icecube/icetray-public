/**
 * @file I3MT19937.cxx
 * @copyright (c) 2017 The IceCube Collaboration
 * @author Kevin Meagher
 * @date December 2017
 */
#include "phys-services/I3MT19937.h"
#include "dataclasses/I3String.h"

I3MT19937::I3MT19937(){}

I3MT19937::I3MT19937(uint32_t seed)
{ 
  //set the initial state of the engine 
  engine_.seed(seed);
}

I3MT19937::I3MT19937(std::vector<uint32_t> seed_vector)
{ 
  //generate a seed sequence from the initilization vector
  std::seed_seq seed(seed_vector.begin(),seed_vector.end());
  //set the initial state of the engine
  engine_.seed(seed);
}

I3FrameObjectPtr I3MT19937::GetState() const
{
  std::stringstream ss;
  ss << engine_;
  return I3StringPtr(new I3String(ss.str()));
}

void I3MT19937::RestoreState(I3FrameObjectConstPtr vstate)
{
  I3StringConstPtr s=(boost::dynamic_pointer_cast<const I3String>(vstate));
  std::stringstream ss(s->value);
  ss >> engine_;
}

I3MT19937::~I3MT19937(){}

// Service Factory 

I3MT19937Factory::I3MT19937Factory(const I3Context& context) 
  : I3ServiceFactory(context)
{
  AddParameter("Seed",
	       "Seed vector to generate the initial state for the random number generator, "
	       "Must be a vector of unsigned ints",
	       seed_vector_);
  
  installServiceAs_ = I3DefaultName<I3RandomService>::value();
  AddParameter("InstallServiceAs",
	       "Install the random service at the following location",
	       installServiceAs_);
}

I3MT19937Factory::~I3MT19937Factory(){}

void I3MT19937Factory::Configure()
{
  GetParameter("Seed", seed_vector_);
  GetParameter("InstallServiceAs",installServiceAs_);
}

bool
I3MT19937Factory::InstallService(I3Context& services)
{
  if(!random_)
    random_ = I3RandomServicePtr(new I3MT19937(seed_vector_));
  return services.Put<I3RandomService>(installServiceAs_, random_);
}

I3_SERVICE_FACTORY(I3MT19937Factory);
