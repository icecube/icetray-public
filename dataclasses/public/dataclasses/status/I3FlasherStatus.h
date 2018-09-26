/**
 * copyright  (C) 2017
 * the icecube collaboration
 * @version $Id$
 * @file I3FlasherStatus.h
 * @date $Date$
 */

#ifndef I3FLASHERSTATUS_H_INCLUDED
#define I3FLASHERSTATUS_H_INCLUDED

#include <icetray/I3FrameObject.h>
#include <icetray/OMKey.h>
#include <dataclasses/I3Map.h>

/**
 * @brief Flasher configuration settings for a particular DOM during
 * a particular subrun.
 */
class I3FlasherStatus : public I3FrameObject {
    
  public:
 
    I3FlasherStatus() : brightness(0), window(0),
                        rate(0), mask(0), delay(0) { }
    ~I3FlasherStatus() { }
    
    unsigned brightness;
    unsigned window;
    unsigned rate;
    /* flashing LED mask */
    unsigned mask;
    unsigned delay;

  private:

    friend class icecube::serialization::access;

    template <class Archive> void serialize(Archive & ar, unsigned version);

};

I3_POINTER_TYPEDEFS(I3FlasherStatus);

// DOMs with no entries in the map can be assumed to not be flashing
typedef I3Map<OMKey, I3FlasherStatus> I3FlasherStatusMap;
I3_POINTER_TYPEDEFS(I3FlasherStatusMap);

// Subruns with no entries in the map can be assumed to have no flashing DOMs
typedef I3Map<unsigned, I3FlasherStatusMap> I3FlasherSubrunMap;
I3_POINTER_TYPEDEFS(I3FlasherSubrunMap);

static const unsigned i3flasherstatus_version_ = 1;
I3_CLASS_VERSION(I3FlasherStatus, i3flasherstatus_version_);

std::ostream& operator<<(std::ostream& oss, const I3FlasherStatus& s);
std::ostream& operator<<(std::ostream& oss, const I3FlasherStatusMap& map);
std::ostream& operator<<(std::ostream& oss, const I3FlasherSubrunMap& map);

#endif // I3FLASHERSTATUS_H_INCLUDED


