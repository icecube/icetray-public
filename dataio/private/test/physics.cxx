/**
   copyright  (C) 2004
   the icecube collaboration
   $Id$

   @version $Revision$
   @date $Date$

*/

#include <I3Test.h>
#include <fstream>
#include <icetray/serialization.h>

#include <icetray/I3Logging.h>

#include "dataclasses/physics/I3AMANDAAnalogReadout.h"
#include "dataclasses/physics/I3RecoHit.h"
#include "dataclasses/physics/I3DOMLaunch.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/physics/I3EventHeader.h"
#include "dataclasses/physics/I3TWRFrag.h"
#include "dataclasses/physics/I3FlasherInfo.h"
#include "dataclasses/physics/I3TWRLaunch.h"
#include "dataclasses/physics/I3MCHit.h"
#include "dataclasses/physics/I3Trigger.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
#include "dataclasses/physics/I3Waveform.h"
#include "dataclasses/physics/I3Particle.h"

#include "serialization-test.h"

#include <boost/preprocessor.hpp>

using namespace icecube::archive;
using namespace std;

TEST_GROUP(physics);

#define TEST_THESE				\
  (I3AMANDAAnalogReadoutMap)			\
  (I3RecoHitSeriesMap)				\
  (I3DOMLaunchSeriesMap)			\
  (I3RecoPulseSeriesMap)			\
  (I3EventHeader)				\
  (I3FlasherInfo)				\
  (I3TWRLaunchSeriesMap)			\
  (I3MCHitSeriesMap)				\
  (I3TriggerHierarchy)				\
  (I3WaveformSeriesMap)				\
  (I3MapStringDouble)				\
  (I3MapStringInt)				\
  (I3MapStringBool)				\
  (I3VectorString)				\
  (I3VectorDouble)				\
  (I3VectorInt)					\
  (I3VectorBool)				\
  (I3Particle)

#define NON_I3FO_ITEMS				\
  (I3WaveformSeries)				\
  (I3MCHitSeries)				\
  (I3RecoHitSeries)				\
  (I3DOMLaunchSeries)				\
  (I3RecoPulseSeries)				\
  (I3AMANDAAnalogReadout)			\
  (I3DOMLaunch)					\
  (I3MCHit)					\
  (I3RecoHit)					\
  (I3RecoPulse)					\
  (I3ResoPulse)					\
  (I3Trigger)					\
  (I3TWRLaunch)					\
  (I3Waveform)					\
  (I3Trigger)					\
  (I3Waveform)					\
  (I3TWRFrag) 
 
#define SERIALIZATION_TEST(r,data,t) SERIALIZE(t)

BOOST_PP_SEQ_FOR_EACH(SERIALIZATION_TEST, ~, TEST_THESE);

