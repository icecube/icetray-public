/**
 *  $Id$
 *
 *  Copyright (C) 2018
 *  Jakob van Santen <jakob.van.santen@desy.de>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 */

#ifndef DATACLASSES_I3RecoPulseSeriesMapCombineByModule_H_INCLUDED
#define DATACLASSES_I3RecoPulseSeriesMapCombineByModule_H_INCLUDED

#include <functional>
#include <string>
#include "icetray/I3FrameObject.h"
#include "icetray/OMKey.h"
#include "icetray/I3Frame.h"
#include "icetray/serialization.h"
#include "dataclasses/physics/I3RecoPulse.h"

static const unsigned I3RecoPulseSeriesMapCombineByModule_version_ = 0;

class I3RecoPulseSeriesMapCombineByModule : public I3FrameObject {
public:
  /*
   * Combine pulses on multiple PMTs in the same module into a single series
   */
  I3RecoPulseSeriesMapCombineByModule(
    const std::string &pulses_key);
  I3RecoPulseSeriesMapCombineByModule();

  std::ostream& Print(std::ostream&) const override;
  
  I3RecoPulseSeriesMapConstPtr Apply(const I3Frame&) const;
  std::string GetSource() const { return pulses_key_; }

  bool operator==(const I3RecoPulseSeriesMapCombineByModule&) const;
  bool operator!=(const I3RecoPulseSeriesMapCombineByModule&) const;
private:
  std::string pulses_key_;
  mutable I3RecoPulseSeriesMapPtr combined_;

  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned version);

  SET_LOGGER("I3RecoPulseSeriesMapCombineByModule");
};

std::ostream& operator<<(std::ostream&, const I3RecoPulseSeriesMapCombineByModule&);

I3_CLASS_VERSION(I3RecoPulseSeriesMapCombineByModule,
  I3RecoPulseSeriesMapCombineByModule_version_);
I3_POINTER_TYPEDEFS(I3RecoPulseSeriesMapCombineByModule);

#endif // DATACLASSES_I3RecoPulseSeriesMapCombineByModule_H_INCLUDED