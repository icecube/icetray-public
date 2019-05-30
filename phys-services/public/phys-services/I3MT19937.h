/**
 * @file I3MT19937.h
 * @copyright (c) 2017 The IceCube Collaboration
 * @author Kevin Meagher
 * @date December 2017
 */

#ifndef I3MT19937_H
#define I3MT19937_H

#include "icetray/I3ServiceFactory.h"
#include "phys-services/I3StdRandomEngine.h"

/**
 * @class I3MT19937
 * @brief An implementation of the I3RandomService interface using the C++ 
 * random number engine for MT19937
 *  
 * This class is a wrapper for the standard C++11 
 * <a href="http://en.cppreference.com/w/cpp/numeric/random">random number 
 * generator</a>. It implements the 
 * <a href="https://en.wikipedia.org/wiki/Mersenne_Twister">Mersenne Twister</a>
 * algorithm implemenation MT19937. This is the most widely used algorithm to 
 * generate a string of random numbers for statistical analysis: it has a 
 * period of 2^19937-1 and is indpendent in up to 623 dimensions.
 *
 * Being a deterministic random number generator, it needs a seed to 
 * initilize the internal state.
 * There are three differnt constructors which initilize the internal state 
 * in different ways: the default constructor, a single integer seed, and
 * a vector of integers. The vector is can be used for distributed computing
 *
 * The distribuitons from the parent class I3RandomService are all defined:
 * Binonial, Exponential, Integer, Poisson, Uniform, and Gaussian.
 *
 * The internal state can be saved and restored as an I3String representing 
 * the internals state as a string of 624 integers
 */ 
class I3MT19937 : public I3StdRandomEngine<I3MT19937>
{
 public:
  /**
   * default constructor uses the default seed as defined by the MT19937 paper.
   * Equivelent to I3MT19937(5489)
   */
  I3MT19937();

  /**
   * Constructor initilizes the internal state of the random number generator 
   * using the state generating function defined in the MT19937 paper
   */
  I3MT19937(uint32_t seed);

  /**
   * Initilize random number generator with the specified seed vector.
   * This uses the c++11 seed sequence to initilize the internal state
   * of the random number generator. Similar seeds such as {0,0} and {0,1} 
   * will result in independent and uncorrelated random bit streams and 
   * are appropiate for distributed computing. For example passing 
   * {run_number,job_number} to each job submitted to a cluster will
   * result in independent random bit streams for each job.
   */
  explicit I3MT19937(std::vector<uint32_t> seed_vector);

  /**
   * destructor
   */
  virtual ~I3MT19937();
  
 /**
  * Get all information necessary to restore the internal
  * state of the generator.
  */
 virtual I3FrameObjectPtr GetState() const;
 
 /**
  * Restore the internal state of the generator
  */
 virtual void RestoreState(I3FrameObjectConstPtr state);
  
private:
  std::mt19937 engine_;
public:
  std::mt19937& engine() { return engine_; }
  const std::mt19937& engine() const { return engine_; }

  SET_LOGGER("I3MT19937");
};

I3_POINTER_TYPEDEFS(I3MT19937);

// Service Factory

class I3Context;
/**
 * @class I3MT19937Factory
 * @brief This class installs a I3MT19937 in the context
 *
 * I3MT19937 takes two parameters: <VAR>Seed</VAR>,
 * <VAR>InstallServiceAs</VAR>.
 */
class I3MT19937Factory : public I3ServiceFactory
{
 public:
  // Constructors and destructor
  I3MT19937Factory(const I3Context& context);
  virtual ~I3MT19937Factory();

  // public member functions
  /**
   * Installed this objects service into the specified context
   *
   * @param services the I3Context into which the service should be installed.
   * @return true if the services is successfully installed.
   */
  virtual bool InstallService(I3Context& services);

  /**
   * Configure service prior to installing it. 
   */
  virtual void Configure();

 private:
  // private constructors, destructor and assignment
  I3MT19937Factory
    (const I3MT19937Factory& rhs); // stop default
  I3MT19937Factory operator=
    (const I3MT19937Factory& rhs); // stop default

  /// initilization vector to create the seed for the random number generator
  std::vector<std::uint32_t> seed_vector_;
  /// name with which to install this service in the context
  std::string installServiceAs_;
  /// pointer to the random service
  I3RandomServicePtr random_;

  SET_LOGGER("I3MT19937Factory");
};

#endif //I319937_H
