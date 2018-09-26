#include "phys-services/I3GSLRandomService.h"
#include <cstring>
#include <gsl/gsl_version.h>

I3GSLRandomService::I3GSLRandomService():
track_state(true)
{
  if(track_state)
    construct_counted(r);
  else
    construct(r);
}

I3GSLRandomService::I3GSLRandomService(unsigned long int seed, bool track_state):
track_state(track_state)
{
  if(track_state)
    construct_counted(r);
  else
    construct(r);
  gsl_rng_set(r, seed);
  if(track_state)
    gsl_rng_set(((gsl_rng_wrapper_state*)r->state)->rng,
                ((gsl_rng_wrapper_state*)r->state)->seed);
}

I3GSLRandomService::~I3GSLRandomService(){
  if(track_state)
    gsl_rng_free(((gsl_rng_wrapper_state*)r->state)->rng);
  gsl_rng_free(r);
}

int I3GSLRandomService::Binomial(int ntot, double prob)
{
  return gsl_ran_binomial(r,prob,ntot);
}

double I3GSLRandomService::Exp(double tau)
{
  return gsl_ran_exponential(r,tau);
}

unsigned int I3GSLRandomService::Integer(unsigned int imax)
{
  return (unsigned int)Uniform(imax);
}

int I3GSLRandomService::Poisson(double mean)
{
  return gsl_ran_poisson(r,mean);
}

double I3GSLRandomService::PoissonD(double mean)
{
  return gsl_ran_poisson(r,mean);
}

double I3GSLRandomService::Uniform(double x1)
{
  return gsl_ran_flat(r,0.0,x1);
}

double I3GSLRandomService::Uniform(double x1, double x2)
{
  return gsl_ran_flat(r,x1,x2);
}

double I3GSLRandomService::Gaus(double mean,double stddev)
{
  return mean + gsl_ran_gaussian(r,stddev);
}

struct I3GSLRandomServiceState : public I3FrameObject {
  std::string gsl_version_;
  std::string rng_type_;
  unsigned long int seed_;
  uint64_t icalls_;
  uint64_t dcalls_;
  
  friend class icecube::serialization::access;
  template <typename Archive>
  void serialize(Archive &ar, unsigned version)
  {
    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar & make_nvp("GSLversion", gsl_version_);
    ar & make_nvp("RNGtype", rng_type_);
    ar & make_nvp("Seed", seed_);
    ar & make_nvp("IntegerCalls", icalls_);
    ar & make_nvp("DoubleCalls", dcalls_);
  }
  
  std::ostream& Print(std::ostream& os) const override{
    os << "[I3GSLRandomServiceState\n"
       << "  GSLversion: " << gsl_version_ << '\n'
       << "  RNGtype: " << rng_type_ << '\n'
       << "  Seed: " << seed_ << '\n'
       << "  IntegerCalls: " << icalls_ << '\n'
       << "  DoubleCalls: " << dcalls_ << "\n]";
    return(os);
  }
};

I3_CLASS_VERSION(I3GSLRandomServiceState, 0);
I3_SERIALIZABLE(I3GSLRandomServiceState);

I3FrameObjectPtr I3GSLRandomService::GetState() const
{
  if(!track_state)
    log_fatal("Call to I3GSLRandomService::GetState() on a random service which"
              " was not told to track RNG state");
  boost::shared_ptr<I3GSLRandomServiceState> state(new I3GSLRandomServiceState);
  state->gsl_version_=GSL_VERSION;
  gsl_rng_wrapper_state* rstate=(gsl_rng_wrapper_state*)r->state;
  state->rng_type_=gsl_rng_name(rstate->rng);
  state->seed_=rstate->seed;
  state->icalls_=rstate->icalls;
  state->dcalls_=rstate->dcalls;
  return state;
}

void I3GSLRandomService::RestoreState(I3FrameObjectConstPtr vstate)
{
  if(!track_state)
    log_fatal("Call to I3GSLRandomService::RestoreState() on a random service "
              "which was not told to track RNG state");
  
  boost::shared_ptr<const I3GSLRandomServiceState> state;
  if (!(state = boost::dynamic_pointer_cast<const I3GSLRandomServiceState>(vstate)))
    log_fatal("The provided state is not an I3GSLRandomServiceState!");
  
  std::string current_gsl_version=GSL_VERSION;
  if(current_gsl_version!=state->gsl_version_)
    log_fatal_stream("Cowardly refusing to restore an RNG state from a different GSL version\n"
                     << "    Current version: " << current_gsl_version << '\n'
                     << "    Stored state version: " << state->gsl_version_);
  
  gsl_rng_wrapper_state* rstate=(gsl_rng_wrapper_state*)r->state;
  
  std::string current_rng_type=gsl_rng_name(rstate->rng);
  if(current_rng_type!=state->rng_type_)
    log_fatal_stream("Cannot restore state from an RNG of a different type\n"
                     << "    Current RNG type: " << current_rng_type << '\n'
                     << "    Stored state type: " << state->rng_type_);
  
  if(state->seed_!=rstate->seed || rstate->icalls>state->icalls_
     || rstate->dcalls>state->dcalls_){
    gsl_rng_free(rstate->rng);
    gsl_rng_free(r);
    construct_counted(r);
    rstate=(gsl_rng_wrapper_state*)r->state; //need to refresh this pointer!
    gsl_rng_set(r, state->seed_);
    gsl_rng_set(rstate->rng, rstate->seed);
  }
  
  while(rstate->icalls<state->icalls_)
    gsl_rng_get(r);
  while(rstate->dcalls<state->dcalls_)
    gsl_rng_uniform(r);
}

inline void I3GSLRandomService::construct(gsl_rng*& r)
{
  gsl_rng_env_setup();
  r = gsl_rng_alloc(gsl_rng_default);
}

inline void I3GSLRandomService::construct_counted(gsl_rng*& r)
{
  gsl_rng_env_setup();
  r = gsl_rng_alloc(&gsl_rng_counting_wrapper);
  //finish setting up the rng state since GSL bugs make it unsafe for
  //gsl_wrapper_set to do so.
  gsl_rng*& inner_r=((gsl_rng_wrapper_state*)r->state)->rng;
  construct(inner_r);
  gsl_rng_set(inner_r, ((gsl_rng_wrapper_state*)r->state)->seed);
}

//initializes the RNG with a seed
void gsl_wrapper_set(void* vstate, unsigned long int s)
{
  gsl_rng_wrapper_state* state = (gsl_rng_wrapper_state*) vstate;
  state->seed=s;
  state->icalls=0;
  state->dcalls=0;
  
  //This is the obvious thing to do, but a bug in GSL versions <1.16 causes it
  //to fail horribly. Instead, as long as those versions must be supported we
  //just leave the seed in state->seed and wait patiently for I3GSLRandomService
  //to allocate the interior rng if necessary and set the seed on it directly.
  /*if(!state->rng)
    I3GSLRandomService::construct(state->rng);
  gsl_rng_set(state->rng, s);*/
}

//gets one random integer
unsigned long gsl_wrapper_get(void* vstate)
{
  gsl_rng_wrapper_state* state = (gsl_rng_wrapper_state*) vstate;
  state->icalls++;
  return(gsl_rng_get(state->rng));
}

//gets one random double
double gsl_wrapper_get_double(void* vstate)
{
  gsl_rng_wrapper_state* state = (gsl_rng_wrapper_state*) vstate;
  state->dcalls++;
  return(gsl_rng_uniform(state->rng));
}

//register our shim with the GSL infrastructure
const gsl_rng_type gsl_rng_counting_wrapper =
{
  "counting_wrapper",            /* name */
  0xffffffffUL,     	         /* RAND_MAX */
  0,                	         /* RAND_MIN */
  sizeof(gsl_rng_wrapper_state), /* size of state */
  &gsl_wrapper_set,              /* initialisation */
  &gsl_wrapper_get,              /* get integer RN */
  &gsl_wrapper_get_double        /* get double RN */
};