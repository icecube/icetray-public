/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @brief An implementation of the I3RandomService interface.
 *
 * Uses the gsl library for the random numbers
 *
 * @version $Revision$
 * @date $Date$
 * @author pretz
 */

#ifndef I3GSLRANDOMSERVICE_H
#define I3GSLRANDOMSERVICE_H

#include "phys-services/I3RandomService.h"

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_test.h>

/**
 * This is (the state for) a shim which allows us to count the calls to the
 * RNG, but otherwise hands all work off to the real GSL implmentations.
 */
typedef struct
{
  unsigned long int seed;
  uint64_t icalls;
  uint64_t dcalls;
  gsl_rng* rng;
} gsl_rng_wrapper_state;

extern const gsl_rng_type gsl_rng_counting_wrapper;

class I3GSLRandomService : public I3RandomService{
 public:
  /**
   * default constructor
   */
  I3GSLRandomService();

  /**
   * constructor
   */
  explicit I3GSLRandomService(unsigned long int seed, bool track_state=true);

  /**
   * destructor
   */
  virtual ~I3GSLRandomService();

  /**
   * a number drawn from a binomial distribution
   */
  virtual int Binomial(int ntot, double prob);

  /**
   * A number from an Exponential distribution
   */
  virtual double Exp(double tau);

  /**
   * An integer drawn uniformly from [0,imax)
   */
  virtual unsigned int Integer(unsigned int imax);

  /**
   * An integer drawn from a Poisson distribution
   */
  virtual int Poisson(double mean);

  /**
   * A number drawn from a Poisson distribution, as a double
   */
  virtual double PoissonD(double mean);

  /**
   * a double drawn from a uniform distribution (0,x1)
   */
  virtual double Uniform(double x1 = 1);

  /**
   * a double drawn from a uniform distribution (x1,x2)
   */
  virtual double Uniform(double x1, double x2);

  /**
   * a double drawn from a Gaussian distribution with given
   * mean and standard deviation
   */
  virtual double Gaus(double mean, double stddev);
  
  /**
   * get all information necessary to restore the internal
   * state of the generator
   */
  virtual I3FrameObjectPtr GetState() const;
  
  /**
   * restore the internal state of the generator
   */
  virtual void RestoreState(I3FrameObjectConstPtr state);

  
 private:

  // private copy constructors and assignment
  I3GSLRandomService(const I3GSLRandomService& );
  I3GSLRandomService operator=(const I3GSLRandomService& );

  /**
   * Helper function which constructs our preferred GSL RNG.
   */
  static void construct(gsl_rng*& r);
  /**
   * Helper function which constructs a GSL RNG wrapped with the counting shim
   */
  static void construct_counted(gsl_rng*& r);

  gsl_rng* r;
  bool track_state;

  SET_LOGGER("I3GSLRandomService");

  //let gsl_wrapper_set use construct()
  friend void gsl_wrapper_set(void* vstate, unsigned long int s);
};

I3_POINTER_TYPEDEFS(I3GSLRandomService);

#endif //I3GSLRANDOMSERVICE_H
