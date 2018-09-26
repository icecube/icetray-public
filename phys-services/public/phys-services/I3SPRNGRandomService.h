#ifndef I3SPRNGRANDOMSERVICE_H
#define I3SPRNGRANDOMSERVICE_H

#include "phys-services/I3RandomService.h"

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_test.h>

#include <string>


/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @brief SPRNG Implementation of the I3RandomService interface.
 * This implementation uses a combination of SPRNG and GSL to generate
 * statistically independent streams of pseudo-random number distributions.
 * See gsl-sprng.h for more details.
 *
 * NB : It's important that you use the same seed for different jobs.  Set
 * nstreams to the number of jobs and use a different streamnum for each job.
 * Otherwise you'll get correlations between the RNG streams.  I know this is
 * counterintuitive, but this is how SPRNG works.
 * 
 * The code for this class is based on John Pretz's implementation of
 * I3GSLRandomService.
 *
 * @version $Revision$
 * @date $Date$
 * @author juancarlos
 *
 * @todo Add ability to save state of rng after run is complete
 *  SPRNG has the functions:
 *
 *      int pack_sprng(char *bytes); // returns size of bytes
 *      void unpack_sprng(char bytes[MAX_PACKED_LENGTH]);
 *
 *  which can be used to save and retrieve the state of an rng
 */

class I3SPRNGRandomService : public I3RandomService{
 public:
  /**
   * default constructor
   */
  I3SPRNGRandomService();

  /**
   * constructors
   */
  I3SPRNGRandomService(int seed, int nstreams, int streamnum,
		       std::string instatefile=std::string(),
		       std::string outstatefile=std::string());

  /**
   * destructor
   */
  virtual ~I3SPRNGRandomService();

  /**
   * Binomial distribution
   */
  virtual int Binomial(int ntot, double prob);

  // As with John Pretz's GSL implementation, I have left this out for now.
  /* virtual double BreitWigner(double mean = 0, double gamma = 1)=0; */
  
  /**
   * Exponential distribution
   */
  virtual double Exp(double tau);

  /**
   * Uniform int distribution with range [0,imax)
   */
  virtual unsigned int Integer(unsigned int imax);

  /**
   * int Poisson distribution
   */
  virtual int Poisson(double mean);

  /**
   * double Poisson distribution
   */
  virtual double PoissonD(double mean);

  /**
   * double uniform distribution with range (0,x1)
   */
  virtual double Uniform(double x1 = 1);

  /**
   * double  uniform distribution with range (x1,x2)
   */
  virtual double Uniform(double x1, double x2);

  /**
   * double Gaussian distribution given mean and StdD
   */
  virtual double Gaus(double mean, double stddev);
   
  virtual I3FrameObjectPtr GetState() const;
  virtual void RestoreState(I3FrameObjectConstPtr state);

 private:

  // private copy constructors and assignment
  I3SPRNGRandomService(const I3SPRNGRandomService& );
  I3SPRNGRandomService operator=(const I3SPRNGRandomService& );

  gsl_rng* rng_;
  std::string instatefile_;
  std::string outstatefile_;
  
  int seed_;
  int streamnum_;
  int nstreams_;

  SET_LOGGER("I3SPRNGRandomService");

};

I3_POINTER_TYPEDEFS(I3SPRNGRandomService);

#endif // I3SPRNGRANDOMSERVICE_H
