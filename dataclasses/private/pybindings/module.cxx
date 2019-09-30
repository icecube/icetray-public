//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <icetray/I3FrameObject.h>
#include <icetray/load_project.h>

using namespace boost::python;
namespace bp = boost::python;
#include <boost/preprocessor.hpp>

//
//  Add the class you are registering to this list of parenthesized typenames.
//  Don't forget to watch that the newlines are backslashed.
//  To register class Nick, add (Nick) to the list below, add
//  Nick.cxx to the list of i3_add_library out in CMakeLists.txt,
//  and create a file Nick.cxx that contains a function 
//    void register_Nick();
//  that does the boost.python registration for class Nick.
//
#define REGISTER_THESE_THINGS                                           \
  (I3Time)(I3Position)(I3Direction)(I3Geometry)(I3MCTree)               \
  (I3Particle)(I3RecoPulse)(I3LinearizedMCTree)(I3TimeWindow)           \
  (I3Waveform)(I3WaveformSeries)(I3WaveformSeriesMap)                   \
  (VectorI3WaveformStatusCompound)(I3DOMLaunch)(I3DOMLaunchSeries)      \
  (I3DOMLaunchSeriesMap)(I3EventHeader)                                 \
  (I3RecoHit)(I3FlasherInfo)(I3Matrix)(I3ParticleID)                    \
  (I3MCHit)(vectorI3MCHit)(mapOMKeyI3MCHit)(I3MCHitSeriesMap)           \
  (I3Calibration)(DroopedSPETemplate)(SPETemplate)            \
  (I3DOMCalibration)(I3VEMCalibration)(LinearFit)(QuadraticFit)         \
  (TauParam)(MapOMKeyI3DOMCalibration)(MapOMKeyI3VEMCalibration)        \
  (I3DetectorStatus)(I3DOMStatus)(I3TriggerStatus)(MapOMKeyI3DOMStatus) \
  (MapSubdetectorI3TriggerReadoutConfig)(MapTriggerKeyI3TriggerStatus)  \
  (I3DOMFunctions)(I3MapUnsignedUnsigned)(I3MapUShortUShort)            \
  (I3MapIntVectorInt)(I3MapKeyVectorDouble)(I3MapKeyDouble)             \
  (I3MapKeyUInt)(I3MapKeyVectorInt)(I3MapTriggerDouble)                 \
  (I3MapTriggerVectorUInt)(I3MapStringDouble)(I3MapStringInt)           \
  (I3MapStringBool)(I3MapStringVectorDouble)(I3MapStringStringDouble)   \
  (I3MapStringTestMapStringString)(I3MapTriggerUInt)                    \
  (I3Double)(I3String)(I3Constants)(I3RecoPulseSeriesMapMask)           \
  (I3RecoPulseSeriesMapUnion)(I3SuperDST)(TankKey)(I3Orientation)       \
  (ModuleKey)(I3ModuleGeo)(I3OMGeo)(I3TankGeo)(I3FilterResult)          \
  (I3MapI3ParticleID)(I3VectorChar)(I3VectorString)(I3VectorBool)       \
  (I3VectorOMKey)(I3VectorModuleKey)(I3VectorShort)(I3VectorTankKey)    \
  (I3VectorUShort)(I3VectorInt)(I3VectorUInt)(I3VectorInt64)            \
  (I3VectorUInt64)(I3VectorFloat)(I3VectorDouble)(I3VectorDoubleDouble) \
  (I3VectorI3Particle)(I3VectorParticleType)(I3VectorI3Position)        \
  (I3VectorPairDoubleDouble)(I3MCTreePhysicsLibrary)(I3VectorUtils)     \
  (SPEChargeDistribution)(I3TriggerHierarchy)(I3Trigger)(TriggerKey)    \
  (I3RecoPulseSeriesMapApplySPECorrection)(I3FlasherStatus)             \
  (I3FlasherStatusMap)(I3FlasherSubrunMap)                              \
  (I3RecoPulseSeriesMapCombineByModule)(I3UInt64)


#define I3_REGISTRATION_FN_DECL(r, data, t) void BOOST_PP_CAT(register_,t)();
#define I3_REGISTER(r, data, t) BOOST_PP_CAT(register_,t)();
BOOST_PP_SEQ_FOR_EACH(I3_REGISTRATION_FN_DECL, ~, REGISTER_THESE_THINGS)

I3_PYTHON_MODULE(dataclasses)
{
  //load_project("dataclasses", false);
  BOOST_PP_SEQ_FOR_EACH(I3_REGISTER, ~, REGISTER_THESE_THINGS);
}

