/**
 * @file I3StdRandomEngine.h
 * @copyright Copyright (c) 2017 The IceCube Collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 * @author Kevin Meagher
 * @date December 2017
 */

#ifndef I3STDRANDOMENGINE_H
#define I3STDRANDOMENGINE_H

#include <random>
#include "dataclasses/I3String.h"
#include "phys-services/I3RandomService.h"

/**
 * @class I3StdRandomEngine
 * @brief An implementation of the I3RandomService interface using C++
 * random number engines
 *
 * The distribuitons from the parent class I3RandomService are all defined:
 * Binonial, Exponential, Integer, Poisson, Uniform, and Gaussian.
 *
 */
template <class Base>
class I3StdRandomEngine : public I3RandomService
{
 public:
  /**
   * Generate a number drawn from a binomial distribution
   */
  virtual int Binomial(int ntot, double prob)
  {
    std::binomial_distribution<int> d(ntot, prob);
    return d(static_cast<Base*>(this)->engine());
  }

  /**
   * Generate a number from an Exponential distribution
   */
  virtual double Exp(double tau)
  {
    std::exponential_distribution<double> d(1./tau);
    return d(static_cast<Base*>(this)->engine());
  }

  /**
   * Generate an integer drawn uniformly from [0,imax)
   */
  virtual unsigned int Integer(unsigned int imax)
  {
    std::uniform_int_distribution<unsigned int> d(0, std::min(imax, imax-1));
    return d(static_cast<Base*>(this)->engine());
  }

  /**
   * Generate an integer drawn uniformly from amongst all 32-bit integers, that is [0, 4294967295]
   */
  virtual uint32_t Integer32()
  {
    std::uniform_int_distribution<unsigned int> d(0,0xFFFFFFFF);
    return d(static_cast<Base*>(this)->engine());
  }

  /**
   * Generate an integer drawn from a Poisson distribution
   */
  virtual int Poisson(double mean)
  {
    std::poisson_distribution<int> d(mean);
    return d(static_cast<Base*>(this)->engine());
  }

  /**
   * Generate a number drawn from a Poisson distribution, return a double
   */
  virtual double PoissonD(double mean)
  {
    return Poisson(mean);
  }

  /**
   * Generate a double drawn from a uniform distribution [0,x)
   */
  virtual double Uniform(double x = 1)
  {
    std::uniform_real_distribution<double> d(0, x);
    return d(static_cast<Base*>(this)->engine());
  }

  /**
   * Generate a double drawn from a uniform distribution [x1,x2)
   */
  virtual double Uniform(double x1, double x2)
  {
    std::uniform_real_distribution<double> d(x1, x2);
    return d(static_cast<Base*>(this)->engine());
  }

  /**
   * Generate a double drawn from a Gaussian distribution with given
   * mean and standard deviation
   */
  virtual double Gaus(double mean, double stddev)
  {
    std::normal_distribution<double> d(mean,stddev);
    return d(static_cast<Base*>(this)->engine());
  }
};

#endif //I3STDRANDOMENGINE_H
