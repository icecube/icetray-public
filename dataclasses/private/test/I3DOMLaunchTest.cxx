/**
 * copyright  (C) 2007
 * the icecube collaboration
 *
 * Test cases to test the compression of ATWD and FADC waveforms
 *
 * @file    /dataclasses/private/test/I3DOMLaunchTest.cxx
 * @version $Revision$
 * @date    $Date$
 * @author  Martin Merck
 */
#include <I3Test.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <icetray/serialization.h>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>

#include <sstream>
#include <algorithm>
#include <iterator>
#include <limits> 

TEST_GROUP(I3DOMLaunch)

TEST(serializeStandardWave)
{
  I3DOMLaunch wave;
  std::vector<int>& atwd0 = wave.GetRawATWD(0);
  std::vector<int>& atwd1 = wave.GetRawATWD(1);
  std::vector<int>& atwd2 = wave.GetRawATWD(2);
  std::vector<int>& fadc = wave.GetRawFADC();
  
  for( int i=0; i< 128; i++) 
  {
    atwd0.push_back( 3 );               // a constant waveform
    atwd1.push_back( i%2 * 1023);       // a waveform oszilating between 0 and 1023
    atwd2.push_back( 511 - i%2 * 1023); // a waveform oszilating between -512 and 511
  }
  
  for( int i=0; i< 256; i++) 
  {
    fadc.push_back( 143 + rand() % 10 );  // a waveform with a baseline of 143 and random noise
	}
	
  // Build a binary stringtream and serialize the I3DOMLaunch
	std::ostringstream oss(std::ostringstream::binary);
	{
		icecube::archive::portable_binary_oarchive outAr( oss );
		outAr & make_nvp("Test", wave);;
	}
  
  // Deserialize a second I3DOMLaunch from the serialized stream for comparison
	I3DOMLaunch wave2;
	std::istringstream iss( oss.str(), std::istringstream::binary );
	{
		icecube::archive::portable_binary_iarchive inAr( iss );
		inAr & make_nvp("Test", wave2);
	}

  // Compare size and content of waveform and ensure that they are equal.
  ENSURE_EQUAL( wave.GetRawATWD(0).size(), wave2.GetRawATWD(0).size(), 
                "size of constant waveforms don't agree" ); 
  
  ENSURE( std::equal( wave.GetRawATWD(0).begin(), wave.GetRawATWD(0).end(), wave2.GetRawATWD(0).begin() ),
          "constant waveforms don't agree" );
  
  ENSURE_EQUAL( wave.GetRawATWD(1).size(), wave2.GetRawATWD(1).size(), 
                "size of unsigned oscillating waveforms don't agree" ); 
  ENSURE( std::equal( wave.GetRawATWD(1).begin(), wave.GetRawATWD(1).end(), wave2.GetRawATWD(1).begin() ),
          "unsigned oscillating waveforms don't agree" );
  
  ENSURE_EQUAL( wave.GetRawATWD(2).size(), wave2.GetRawATWD(2).size(), 
                "size of signed oscillating waveforms don't agree" ); 
  ENSURE( std::equal( wave.GetRawATWD(2).begin(), wave.GetRawATWD(2).end(), wave2.GetRawATWD(2).begin() ),
          "signed oscillating waveforms don't agree" );
  

  ENSURE_EQUAL( wave.GetRawATWD(3).size(), wave2.GetRawATWD(3).size(), 
                "size of empty waveforms don't agree" ); 
  ENSURE( std::equal( wave.GetRawATWD(3).begin(), wave.GetRawATWD(3).end(), wave2.GetRawATWD(0).begin() ),
          "empty waveforms don't agree" );

  ENSURE_EQUAL( wave.GetRawFADC().size(), wave2.GetRawFADC().size(), 
                "size of noisy waveforms don't agree" ); 
  ENSURE( std::equal( wave.GetRawFADC().begin(), wave.GetRawFADC().end(), wave2.GetRawFADC().begin() ),
          "noisy waveforms don't agree" );
  
}

TEST(serializeMaxIntegerWave)
{
  I3DOMLaunch wave;
  std::vector<int>& atwd0 = wave.GetRawATWD(0);
  
  for( int i=0; i< 128; i++) 
  {
    if( i%2 == 0 )
      atwd0.push_back( 0 );       
    else
      atwd0.push_back( std::numeric_limits<int>::max()/2 );
   }
  

  // Build a binary stringtream and serialize the I3DOMLaunch
  namespace io = boost::iostreams;
  typedef std::vector<char> buffer_t;
  typedef io::stream<io::back_insert_device<buffer_t > > sink_t;
  typedef io::stream<io::array_source> source_t;
  
  buffer_t buffer;
  {
    sink_t sink(buffer);
    icecube::archive::portable_binary_oarchive outAr(sink);
    outAr & make_nvp("wave", wave);
  }
    
  // Deserialize a second I3DOMLaunch from the serialized stream for comparison
  I3DOMLaunch wave2;
  source_t source(&*buffer.begin(), &*buffer.end());
  {
    icecube::archive::portable_binary_iarchive inAr(source);
    inAr >> wave2;
  }

}

// This testcase tests a bugfix for the condition where the
// compressed bitstram totally fills the last compressed byte.
TEST(serializeFullCompressed)
{
  I3DOMLaunch wave;
  std::vector<int>& atwd0 = wave.GetRawATWD(0);
  
  int vals[] = {
         142,142,142,141,142,142,149,156,154,146,143,142,141,139,141,139,
         141,143,150,146,144,150,154,146,142,141,140,138,139,139,139,138,
         138,138,138,138,139,138,139,139,140,139,140,141,140,139,139,139,
         139,140,140,140,140,139,140,140,140,139,140,140,140,140,141,140,
         141,140,140,140,142,140,141,140,140,140,142,139,140,140,140,139,
         140,140,140,140,140,139,140,139,140,140,139,139,142,141,140,141,
         142,140,142,141,142,140,142,140,140,139,140,139,142,140,142,140,
         142,141,142,141,142,142,142,142,142,142,142,141,142,141,142,142,
         140,141,140,142,142,142,142,142,142,141,141,141,140,141,142,142,
         142,140,141,141,141,141,141,142,142,142,142,141,142,141,142,142,
         140,141,142,142,142,140,142,142,142,140,140,141,142,142,142,140,
         142,142,142,141,142,142,142,142,142,141,142,142,142,142,142,142,
         142,139,142,141,142,141,142,141,142,142,143,142,142,142,142,142,
         142,142,142,142,142,142,142,141,142,142,142,141,142,142,142,142,
         142,142,142,142,142,142,142,142,142,142,141,142,142,141,140,140,
         142,142,142,141,141,142,142,141,142,141,142,140,141,141,142,142};
         
         
  for( int i=0; i< 256; i++) 
  {
      atwd0.push_back( vals[i] );       
  }
  
  // Build a binary stringtream and serialize the I3DOMLaunch
  std::ostringstream oss(std::ostringstream::binary);
  {
    icecube::archive::portable_binary_oarchive outAr( oss );
    outAr & make_nvp("Test", wave);;
  }
    
  // Deserialize a second I3DOMLaunch from the serialized stream for comparison
  I3DOMLaunch wave2;
  std::istringstream iss( oss.str(), std::istringstream::binary );
  {
    icecube::archive::portable_binary_iarchive inAr( iss );
    inAr & make_nvp("Test", wave2);
  }

  // Compare size and content of waveform and ensure that they are equal.
  ENSURE_EQUAL( wave.GetRawATWD(0).size(), wave2.GetRawATWD(0).size(), 
                "size of special waveforms don't agree" ); 
  
  ENSURE( std::equal( wave.GetRawATWD(0).begin(), wave.GetRawATWD(0).end(), wave2.GetRawATWD(0).begin() ),
          "special waveforms don't agree" );
  
}

TEST(SerializeRandomWave)
{
  
  for (int test=0; test<1000; test++)
    {
      I3DOMLaunch launch1;

      launch1.SetStartTime(rand());
      launch1.SetTriggerType(I3DOMLaunch::TriggerType(rand()%6));
      launch1.SetTriggerMode(I3DOMLaunch::TriggerMode(rand()%32));
      launch1.SetWhichATWD(I3DOMLaunch::ATWDselect(rand()%2+1));

      for( int i=0; i< 128; i++) 
	{
	  launch1.GetRawATWD(0).push_back( rand()%2048 );            
	  launch1.GetRawATWD(1).push_back( rand()%2048 );           
	  launch1.GetRawATWD(2).push_back( rand()%2048 );          
	}
  
      for( int i=0; i< 256; i++) 
	{
	  launch1.GetRawFADC().push_back( rand()%2048 );           
	}

      launch1.SetLCBit(rand()%2);
      launch1.SetIsPedestalSub(rand()%2);
      launch1.SetChargeStampHighestSample(rand());
	
      // Build a binary stringtream and serialize the I3DOMLaunch
      std::ostringstream oss(std::ostringstream::binary);
      {
	icecube::archive::portable_binary_oarchive outAr( oss );
	outAr & make_nvp("Test", launch1);;
      }
  
      // Deserialize a second I3DOMLaunch from the serialized stream for comparison
      I3DOMLaunch launch2;
      std::istringstream iss( oss.str(), std::istringstream::binary );
      {
	icecube::archive::portable_binary_iarchive inAr( iss );
	inAr & make_nvp("Test", launch2);
      }
      
      ENSURE(launch1==launch2);
    }
}

TEST(JustSomeSLCStuff)
{
  I3DOMLaunch mylaunch;
  mylaunch.SetRawATWDChargeStamp(1000);
  mylaunch.SetWhichATWDChargeStamp(1);

  ENSURE_EQUAL(1000u,mylaunch.GetRawATWDChargeStamp(),
	 "RawATWDChargeStamps do not agree");
  ENSURE_EQUAL(1u,mylaunch.GetWhichATWDChargeStamp(),
	 "WhichATWDChargeStamps do not agree");
}
