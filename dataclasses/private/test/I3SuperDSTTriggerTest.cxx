#include <I3Test.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <dataclasses/payload/I3SuperDSTTrigger.h>
#include "dataclasses/payload/I3SuperDST.h"
#include "icetray/I3Units.h"

TEST_GROUP(I3SuperDSTTriggerTest);

TEST(i3objects_constructor)
{
  I3Trigger trigger = I3Trigger();
  I3DetectorStatus detectorstatus = I3DetectorStatus();
  detectorstatus.triggerStatus[trigger.GetTriggerKey()] = I3TriggerStatus();
  I3SuperDSTTrigger(trigger,detectorstatus);
}

TEST(i3objects_constructor_bad)
{
  I3Trigger trigger = I3Trigger();
  I3DetectorStatus detectorstatus = I3DetectorStatus();
  int caught = 0;
  try
  {
    I3SuperDSTTrigger(trigger,detectorstatus);
  }
  catch (const std::range_error& e)
  {
    caught = 1;
  }
  if (caught != 1)
    throw std::runtime_error("did not raise error like expected");
}

TEST(idx_constructor)
{
  uint32_t key_idx = 0;
  uint32_t startcode = 0;
  uint32_t lengthcode = 0;

  I3SuperDSTTrigger superdsttrigger(key_idx,startcode,lengthcode);
}

TEST(SetGetTimeI3SuperDSTTrigger)
{
  //we need of course the constructor of the class that owns the member functions that we want to test
  const uint32_t key_idx = 0;
  const uint32_t startcode = 0;
  const uint32_t lengthcode = 1;
  I3SuperDSTTrigger instOtherSuperDSTTrigger(key_idx,startcode,lengthcode);

  //We want to test if the startcode (gotten via DecoTime) passed in the constructor is equal to the one returned by GetTime
  I3SuperDST instSuperDST;
  const unsigned int version=1;
  const double &decodetime= instSuperDST.DecodeTime(startcode,version);
  
  I3SuperDSTTrigger instSuperDSTTrigger(key_idx,startcode,lengthcode);
 
  const double &get_starttime= instSuperDSTTrigger.GetTime();
  //Now what we want to test
  ENSURE_EQUAL(get_starttime,decodetime,
	       "I3SuperDSTTrigger::GetTime and I3SuperDST::DecodeTime are not equal");
 
}

TEST(LengthI3SuperDSTTrigger)
{
  //we need of course the constructor of the class that owns the member functions that we want to test
  const uint32_t key_idx = 0;
  const uint32_t startcode = 0;
  const uint32_t lengthcode = 1;
  I3SuperDSTTrigger instSuperDSTTrigger(key_idx,startcode,lengthcode);

  //We want to test if the lengthcode (gotten via DecoTime) passed in the constructor is equal to the one returned by GetLength
  I3SuperDST instSuperDST;
  const unsigned int version=1;
  const double &decodetime= instSuperDST.DecodeTime(lengthcode,version);

  const double &get_length= instSuperDSTTrigger.GetLength();
  //Now what we want to test
  ENSURE_EQUAL(get_length,decodetime,
	       "I3SuperDSTTrigger::GetLength and I3SuperDST::DecodeTime are not equal");
}

TEST(GetTriggerKeyI3SuperDSTTrigger)
{
  //we need of course the constructor of the class that owns the member functions that we want to test
  const uint32_t key_idx = 0;
  const uint32_t startcode = 0;
  const uint32_t lengthcode = 1;
  I3SuperDSTTrigger instSuperDSTTrigger(key_idx,startcode,lengthcode);

  //To demostrate that key_idx is equal to the one obtained via TriggerKey
  //We have to build a I3TriggerStatusMap, therefore, we need first the TriggerKey object 
  I3DetectorStatus instDetectorStatus;
  I3Trigger trigger;
  instDetectorStatus.triggerStatus[trigger.GetTriggerKey()] = I3TriggerStatus();
  const TriggerKey &get_triggerkey= instSuperDSTTrigger.GetTriggerKey(instDetectorStatus);

  //Now we go for I3TriggerStatusMap
  //Below is what appears in an inline function at I3SuperDSTTrigger.cxx
  I3TriggerStatusMap instTtriggerStatusMap;
  instTtriggerStatusMap[get_triggerkey]=I3TriggerStatus();
  I3TriggerStatusMap::iterator it = instTtriggerStatusMap.find(get_triggerkey);
  const uint32_t get_key_idx= (std::distance(instTtriggerStatusMap.begin(), it));

  //Now what we want to test
  ENSURE_EQUAL(key_idx,get_key_idx,
	       "I3SuperDSTTrigger::GetTriggerKey does not return the right value");
}

