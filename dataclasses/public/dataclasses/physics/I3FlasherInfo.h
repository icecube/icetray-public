/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3FlasherInfo.h
 * @date $Date$
 */

#ifndef I3FLASHERINFO_H_INCLUDED
#define I3FLASHERINFO_H_INCLUDED

#include <vector>
#include "icetray/I3FrameObject.h"
#include "icetray/OMKey.h"
#include "dataclasses/I3Vector.h"

static const unsigned i3flasherinfo_version_ = 1;

/**
 * @brief This contains the data on which DOM flashed during flasher runs
 *
 * It holds the flashing DOMs OMKey, brightness level, mask (which LEDs fired)
 * firing time, and ATWD trace of the LED current pulse 
 * 
 * This will be changed into a I3Trigger derrived class eventually, once there
 * is sufficient funcionality to get Flasher settings somewhere.
 * Included here for use by flasher analysis people.  It can be "I3Bagged".
 *
 */
class I3FlasherInfo : public I3FrameObject
{

protected:

    /**  
     * This is the OM that flashed
     */

    OMKey flasherOM_; 
    /**
     *  Time (in nsec) in 25 nsec units, of the LED flash time.
     */

    double flashTime_;  

    /** 
     * These is the ATWD waveform of the LED current pulse, and the
     * ATWD time bin size, in nsec 
     */

    std::vector<int> rawATWD3_;
    double aTWDBinSize_;
    bool pedestalSubtractedATWD3_;

    /*  This holds the mask which showed which of the 12 LED(s) fired*/

    unsigned int mask_;

    /* This is the brightness level (1 to 127) */

     int LEDBrightness_;

     int width_;
     int rate_;

    
public:
 
    I3FlasherInfo()
    : flashTime_(0.0), 
      aTWDBinSize_(0),
      pedestalSubtractedATWD3_(false),
      mask_(0),
      LEDBrightness_(0),
      width_(0),
      rate_(0)
    { }

    ~I3FlasherInfo(){;}

    std::ostream& Print(std::ostream&) const override;

    /* return and set OMKey */

    OMKey GetFlashingOM() const { return flasherOM_; }
    void SetFlashingOM(OMKey flashOM) { flasherOM_ = flashOM; }
  
    /**
     * return and set flash  time
     */
    double GetFlashTime() const { return flashTime_; }
    void SetFlashTime(double flashtime) { flashTime_ = flashtime; }

    /**
     * return and set ATWD time bin size
     */
    double GetATWDBinSize() const {return aTWDBinSize_;}
    void SetATWDBinSize(double ATWDbinsize) {aTWDBinSize_ = ATWDbinsize;}

    /* return and set LEDbrightness */

    int GetLEDBrightness() const { return LEDBrightness_; }
    void SetLEDBrightness(int LEDint) { LEDBrightness_ = LEDint; }


    /* return and set mask - which LEDs are on */

    unsigned int GetMask() const { return mask_; }
    void SetMask(int LEDmask) { mask_ = LEDmask; }


      /* return and set width */
 
    unsigned int GetWidth() const { return width_; }
    void SetWidth(int width) { width_ = width; }


    /* return and set rate */
 
    unsigned int GetRate() const { return rate_; }
    void SetRate(int rate) { rate_ = rate; }

    /**
     * return raw ATWD3 waveform as a read-only object
     * ATWD3 holds the digitized LED current pulse; the others channels are empty
     */
    const std::vector<int>& GetRawATWD3() const {return rawATWD3_;}
  

 //   void SetPedestalIsSubtractedATWD3(bool subtracted) { pedestalSubtractedATWD3_ = subtracted; }

    
    /* is ATWD3 pedestal subtracted? */
   // bool PedestalSubtractedATWD3() { return pedestalSubtractedATWD3_; }

    /* sets the ATWD3 waveform */
    void SetRawATWD3(const std::vector<int>& ATWD3data) {rawATWD3_=ATWD3data;}


private:

    friend class icecube::serialization::access;

    template <class Archive> void serialize(Archive & ar, unsigned version);

};

std::ostream& operator<<(std::ostream&, const I3FlasherInfo&);

I3_POINTER_TYPEDEFS(I3FlasherInfo);
I3_CLASS_VERSION(I3FlasherInfo, i3flasherinfo_version_);

typedef I3Vector<I3FlasherInfo> I3FlasherInfoVect;
I3_POINTER_TYPEDEFS(I3FlasherInfoVect);
#endif //I3FLASHERINFO_H_INCLUDED


