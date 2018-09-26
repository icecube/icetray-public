/*
 * class: I3SPRNGRandomService
 *
 * Version $Id$
 *
 * Date: 2 Apr 2005
 *
 * (c) IceCube Collaboration
 */

// Class header files
#include "phys-services/I3SPRNGRandomService.h"
#include "phys-services/gsl-sprng.h" 
#include <string>
#include <fstream>
#include <cassert>
#include <sys/stat.h>

using namespace std;
 
struct I3SPRNGRandomServiceState : public I3FrameObject {
	int seed_, nstreams_, streamnum_;
	uint64_t icalls_, dcalls_;
	
	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, unsigned version)
	{
		ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
		ar & make_nvp("Seed", seed_);
		ar & make_nvp("NStreams", nstreams_);
		ar & make_nvp("StreamNum", streamnum_);
		ar & make_nvp("IntegerCalls", icalls_);
		ar & make_nvp("DoubleCalls", dcalls_);
	}
  
  std::ostream& Print(std::ostream& os) const override{
    os << "[I3SPRNGRandomServiceState\n"
       << "  Seed: " << seed_ << '\n'
       << "  NStreams: " << nstreams_ << '\n'
       << "  StreamNum: " << streamnum_ << '\n'
       << "  IntegerCalls: " << icalls_ << '\n'
       << "  DoubleCalls: " << dcalls_ << "\n]";
    return(os);
  }
};

I3_CLASS_VERSION(I3SPRNGRandomServiceState, 0);
I3_SERIALIZABLE(I3SPRNGRandomServiceState);

I3SPRNGRandomService::I3SPRNGRandomService() : seed_(0), streamnum_(1), nstreams_(0)
{
  gsl_rng_env_setup();
  rng_ = gsl_sprng_init(seed_, nstreams_, streamnum_);
}


I3SPRNGRandomService::I3SPRNGRandomService(
				int seed,   		
				int nstreams,  // number of rng streams  (nstreams > streamnum)
				int streamnum, // stream number for this process
				string infile, // where state from prev. run is stored
				string outfile // where state for next run will be stored
				): instatefile_(infile),outstatefile_(outfile),
				   seed_(seed), streamnum_(streamnum), nstreams_(nstreams)
{
  char buffer[MAX_PACKED_LENGTH];
  int size;
  struct stat istat;

  // do some configuration checks before trying to install
  if ( !(nstreams > streamnum) ) {
     log_fatal("SPRNG: Number of RNG streams must be greater than the stream number");
  }
  assert(streamnum<nstreams);
  assert(streamnum>=0);
  gsl_rng_env_setup();

  if (!instatefile_.empty()) { 
     log_debug("checking saved input RNG state %s", instatefile_.c_str());

    // check saved rng state
    if (stat(instatefile_.c_str(), &istat)) { 
       log_fatal("SPRNG: Input RNG state file '%s' cannot be read!!!", instatefile_.c_str());
    }
  	ifstream in(instatefile_.c_str()); 
  	in.read((char*) &size,sizeof(int));  // read size of array
  	in.read(buffer,size);		// read array
  	// read the number of calls that it took to get from
  	// the seed to the stored state, if they are present 
  	uint64_t icalls(0), dcalls(0);
  	if (in.read((char*)&icalls, sizeof(uint64_t)).fail())
  		icalls = 0;
  	if (in.read((char*)&dcalls, sizeof(uint64_t)).fail())
  		dcalls = 0;
  	in.close();

  	rng_ = gsl_sprng_init(seed_, nstreams_, streamnum_, buffer, icalls, dcalls);
  } else {
  	rng_ = gsl_sprng_init(seed_, nstreams_, streamnum_);
  }
  
  if (!rng_)
      log_fatal("Failed to allocate random number generator!");
}

I3SPRNGRandomService::~I3SPRNGRandomService()
{
   if ( !outstatefile_.empty() ) { //save rng state to file
	   int size;
	   char *bytes;
	   
	   ofstream out(outstatefile_.c_str()); 
	   size = gsl_sprng_pack(rng_, &bytes);
	   out.write((char*) &size,sizeof(int)); // write the size of array
	   out.write(bytes,size); // write array
	   // write the number of calls that it took to get from
	   // the seed to the current state
	   gsl_sprng_stream *stream = (gsl_sprng_stream*)(rng_->state);
	   out.write((char*)&(stream->icalls),sizeof(uint64_t));
	   out.write((char*)&(stream->dcalls),sizeof(uint64_t));
	   out.close(); 
	   free(bytes);
   }
   gsl_sprng_free(rng_);
}

int I3SPRNGRandomService::Binomial(int ntot, double prob)
{
  return gsl_ran_binomial(rng_,prob,ntot);
}

double I3SPRNGRandomService::Exp(double tau)
{
  return gsl_ran_exponential(rng_,tau);
}

unsigned int I3SPRNGRandomService::Integer(unsigned int imax)
{
  return (unsigned int)Uniform(imax);
}

int I3SPRNGRandomService::Poisson(double mean)
{
  return gsl_ran_poisson(rng_,mean);
}

double I3SPRNGRandomService::PoissonD(double mean)
{
  return gsl_ran_poisson(rng_,mean);
}

double I3SPRNGRandomService::Uniform(double x1)
{
  return gsl_ran_flat(rng_,0.0,x1);
}

double I3SPRNGRandomService::Uniform(double x1, double x2)
{
  return gsl_ran_flat(rng_,x1,x2);
}

double I3SPRNGRandomService::Gaus(double mean,double stddev)
{
  return mean + gsl_ran_gaussian(rng_,stddev);
}

I3FrameObjectPtr I3SPRNGRandomService::GetState() const
{
	boost::shared_ptr<I3SPRNGRandomServiceState> state(new I3SPRNGRandomServiceState);
	gsl_sprng_stream *stream = (gsl_sprng_stream*)(rng_->state);
	
	state->seed_ = seed_;
	state->nstreams_ = nstreams_;
	state->streamnum_ = streamnum_;
	state->icalls_ = stream->icalls;
	state->dcalls_ = stream->dcalls;
	
	return state;
}

void I3SPRNGRandomService::RestoreState(I3FrameObjectConstPtr vstate)
{
	boost::shared_ptr<const I3SPRNGRandomServiceState> state;
	if (!(state = boost::dynamic_pointer_cast<const I3SPRNGRandomServiceState>(vstate)))
		log_fatal("The provided state is not an I3SPRNGRandomServiceState!");
	
	gsl_sprng_stream *stream = (gsl_sprng_stream*)(rng_->state);
	
	// If the current configuration is from a different stream or our sequence
	// has passed the point of the stored state, destroy the RNG and start over.
	if (state->seed_ != seed_ || state->nstreams_ != nstreams_ || state->streamnum_ != streamnum_
	    || stream->icalls > state->icalls_ || stream->dcalls > state->dcalls_) {
		 seed_ = state->seed_;
		 nstreams_ = state->nstreams_;
		 streamnum_ = state->streamnum_;
		 gsl_sprng_free(rng_);
		 rng_ = gsl_sprng_init(seed_, nstreams_, streamnum_);
		 stream = (gsl_sprng_stream*)(rng_->state);
	}
	
	// Draw and discard variates until we reach the requested point
	// in the sequence.
	while (stream->icalls < state->icalls_) {
		isprng(stream->streamptr);
		stream->icalls++;
	}
	while (stream->dcalls < state->dcalls_) {
		sprng(stream->streamptr);
		stream->dcalls++;
	}
}
