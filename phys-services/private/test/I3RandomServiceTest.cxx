/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author pretz
*/

#include <I3Test.h>

#include "phys-services/I3RandomService.h"
#include "phys-services/I3MT19937.h"
#include "phys-services/I3GSLRandomService.h"
#include "phys-services/I3SPRNGRandomService.h"

#include "boost/random/uniform_real.hpp"
#include "boost/random/lognormal_distribution.hpp"
#include "boost/filesystem.hpp"

#include <string>
#include <vector>
using std::string;
using std::cout;
using std::endl;
using std::vector;

// tests a random number service that it has the expected mean 
// and std dev.  Theoretically it is possible for a random number
// to fail this test and still be working, so I put healthy tolerances
// on it.
TEST_GROUP(I3RandomServiceTest);

namespace randomServiceTest{
void testMeanStddev(vector<double>& values,
		    double expected_average,
		    double expected_stddev)
{
  double sum = 0;
  for(unsigned int i = 0 ; i < values.size() ; i++)
    {
      sum += values[i];
    }
  double measured_average = sum / (double)values.size();
  ENSURE_DISTANCE(measured_average,expected_average,0.01,"testing mean");
  
  vector<double> differences;
  differences.resize(values.size());
  
  for(unsigned int i = 0 ; i < values.size() ; i++)
    {
      differences[i] = 
	(values[i] - measured_average) * 
	(values[i] - measured_average);
      //	cout<<differences[i]<<"\n";
    }
  
  sum = 0;
  
  for(unsigned int i = 0; i < values.size() ; i++)
    {
      sum += differences[i];
    }
  
  double measured_stddev = sqrt(sum / (double) differences.size());
  ENSURE_DISTANCE(measured_stddev,
		  expected_stddev,
		  0.01,
		  "testing stdev");
  
  
}

template <int samples,class Random>
void testUniform(Random& r)
{
  vector<double> values;
  values.resize(samples);
  for(int i = 0 ; i< samples ; i++)
    values[i] = r.Uniform();
  
  // mean 0.5 and stdev = sqrt( <x^2> - <x> ^ 2);
  testMeanStddev(values,0.5,sqrt(0.3333 - 0.5 * 0.5));
}

template <int samples,class Random>
void testExp(Random& r)
{
  vector<double> values;
  values.resize(samples);
  for(int i = 0; i< samples ; i++)
    values[i] = r.Exp(1.0);
  testMeanStddev(values,1.0,1.0);
}

template <int samples,class Random>
void testGaussian(Random& random)
{
  vector<double> values;
  values.resize(samples);
  for(int i = 0 ; i<samples ; i++)
    values[i] = random.Gaus(5,1.0);
  testMeanStddev(values,5,1.0);
}


template <int samples,class Random>
void testRandomService(Random& random)
{
  testUniform<samples,Random>(random);
  testExp<samples,Random>(random);
  testGaussian<samples,Random>(random);
}


template <int samples,class Random>
void testIndependence(Random& r1, Random& r2)
{
  double mean1=0.5;
  double mean2=0.5;
  double covariance=0;
  double sigma1=0;
  double sigma2=0;
  double correlation;

  for(int i = 0 ; i< samples ; i++) {
    covariance += (r1.Uniform()-mean1)*(r2.Uniform()-mean2);
    sigma1 += pow(r1.Uniform()-mean1,2);
    sigma2 += pow(r2.Uniform()-mean2,2);
  }
  correlation = covariance/sqrt(sigma1*sigma2);
  ENSURE_DISTANCE(correlation, 0.0, 0.01, "testing correlation");
}
  
void testStateRestoration(I3RandomService& random, unsigned int samples=16384)
{
  for(unsigned int i=0; i<samples; i++)
    random.Integer(65536);
  boost::shared_ptr<I3FrameObject> state=random.GetState();
  std::vector<unsigned int> saved;
  for(unsigned int i=0; i<samples; i++)
    saved.push_back(random.Integer(65536));
  //----
  random.RestoreState(state);
  for(unsigned int i=0; i<samples; i++){
    unsigned int next=random.Integer(65536);
    ENSURE(next==saved[i],"Random stream after state restoration must match");
  }
}
}

TEST(I3MT19937Test)
{
  I3MT19937 random1;
  I3MT19937 random2(666);
  randomServiceTest::testRandomService<100000,I3MT19937>(random1);
  randomServiceTest::testRandomService<100000,I3MT19937>(random2);
}

TEST(I3GSLRandomService)
{
  I3GSLRandomService random1;
  I3GSLRandomService random2(666);
  randomServiceTest::testRandomService<100000,I3GSLRandomService>(random1);
  randomServiceTest::testRandomService<100000,I3GSLRandomService>(random2);
  randomServiceTest::testStateRestoration(random1);
  randomServiceTest::testStateRestoration(random2);
}

#ifdef I3_USE_SPRNG
TEST(I3SPRNGRandomService)
{
  {
  int nstreams = 2;
  int streamnum = 0;

  I3SPRNGRandomService random1(666,nstreams,streamnum,"","sprngsaved");
  randomServiceTest::testRandomService<100000,I3SPRNGRandomService>(random1);

  /*I3SPRNGRandomService random2(666,nstreams,streamnum,"sprngsaved","");*/
  /*randomServiceTest::testRandomService<100000,I3SPRNGRandomService>(random2);*/

  I3SPRNGRandomService random3(666,nstreams,1,string(),string());

  randomServiceTest::testIndependence<1000000,I3SPRNGRandomService>(random1,random3);
  /*randomServiceTest::testIndependence<1000000,I3SPRNGRandomService>(random1,random2);*/
  
  randomServiceTest::testStateRestoration(random1);
  randomServiceTest::testStateRestoration(random3);

  }
  boost::filesystem::remove("sprngsaved");
}
#endif

TEST(BoostDistributionCompatibility)
{
  const unsigned int samples=10000000;
  vector<double> values(samples);
  
  I3GSLRandomService random1;
  
  boost::uniform_real<double> unidist(0.0,1.0);
  for(unsigned int i = 0 ; i< samples ; i++)
    values[i] = unidist(random1);
  // mean 0.5 and stdev = sqrt( <x^2> - <x> ^ 2);
  randomServiceTest::testMeanStddev(values,0.5,sqrt(0.3333 - 0.5 * 0.5));
  
#if BOOST_VERSION>=104300
  //problems in boost 1.38 definitely make this test fail, and while the
  //documentation is unclear, it appears that 1.43 may be the first version
  //which works correctly
  boost::lognormal_distribution<double> lndist(/*mean*/5.8,/*stddev*/7.9);
  for(unsigned int i = 0 ; i< samples ; i++)
    values[i] = lndist(random1);
  randomServiceTest::testMeanStddev(values,5.8,7.9);
#endif
}
