/**
 *  $Id$
 *  Copyright (C) 2012
 *  Alex Olivas  <olivas@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *   @author olivas
 */

#ifndef I3CUTVALUESBASE_H
#define I3CUTVALUESBASE_H

#include <icetray/I3FrameObject.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/Utility.h>

class I3FrameObject;
template <typename Key, typename Value> struct I3Map;
class OMKey;
class I3Particle;
class I3RecoPulse;
class I3Geometry;

class I3CutValuesBase : public I3FrameObject {

 public:
  virtual void Calculate(const I3Particle& vertex,
                         const I3Geometry& geometry,
                         const I3Map< OMKey, std::vector< I3RecoPulse> >& pulsemap,
                         const double& begTWindow = I3Constants::dt_window_l,
                         const double& endTWindow = I3Constants::dt_window_h) = 0;

 private:
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);
};

I3_POINTER_TYPEDEFS(I3CutValuesBase);

#endif
