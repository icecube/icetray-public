
/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 *
 * @file I3DOMLaunch.h
 * @version $Revision$
 * @date $Date$
 */

#ifndef I3DOMLAUNCH_H_INCLUDED
#define I3DOMLAUNCH_H_INCLUDED

#include <vector>
#include <dataclasses/I3Map.h>
#include <icetray/OMKey.h>

/**
 * @brief The direct (digital) readout of an IceCube DOM
 * 
 * The full digital readout of an IceCube DOM consists of 1-4
 * short but fine-grained ATWD readouts, and a long but coarse fADC
 * readout, all beginning at the same time. The fADC is fixed at 40
 * MHz, while the sampling rate of the ATWDs is adjustable and is
 * determined by the DOM calibrator. There is also a 'coarse charge stamp'
 * containing the 3 largest samples out of the first 16 fADC samples
 */
static const unsigned i3domlaunch_version_ = 5;

/**
 * List the names of enumeration members defined in this file
 * here. These can be used for e.g. pybindings, which require
 * the names of the enumeration members to be known. This list
 * should be updated whenver members or new enums are added to
 * the class.
 */
#define I3DOMLAUNCH_H_I3DOMLaunch_ATWDselect                        \
    (ATWDa)(ATWDb)
#define I3DOMLAUNCH_H_I3DOMLaunch_TriggerType                       \
    (TEST_PATTERN)(CPU_REQUESTED)(SPE_DISCRIMINATOR_TRIGGER)        \
    (FLASHER_BOARD_TRIGGER)(MPE_DISCRIMINATOR_TRIGGER)(ONBOARD_LED)
#define I3DOMLAUNCH_H_I3DOMLaunch_TriggerMode                       \
    (UNDEFINED)(REQUIRE_BOTH)(FR_RUN)(LC_LOWER)(LC_UPPER)           \
    (UNKNOWN_MODE)(SLC_READOUT)(MIN_BIAS)(LAST_TRIGGER_SITUATION)

class I3DOMLaunch 
{
public:  
    enum ATWDselect
    {
      UNKNOWN_CHIP = 0,
      ATWDa = 1,
      ATWDb = 2
    };
    
    enum TriggerType
    {
      TEST_PATTERN = 0,
      CPU_REQUESTED = 1,
      SPE_DISCRIMINATOR_TRIGGER = 2,
      FLASHER_BOARD_TRIGGER = 3,
      MPE_DISCRIMINATOR_TRIGGER = 4,
      ONBOARD_LED = 5
    };
    
    enum TriggerMode
    {
      UNDEFINED = 0,
      /** Set if signals from both upper and lower DOMs are required
       *  to satisfy local coincidence */
      REQUIRE_BOTH = 1L << 0,
      /** Set if flasher board run is in the progress */
      FR_RUN = 1L << 1,
      /** Set if local coincidence requirement is met, and
       *  LC has been configured to be received by the lower DOM */
      LC_LOWER = 1L << 2,
      /** Set if local coincidence requirement is met, and
       *  LC has been configured to be received by the upper DOM */
      LC_UPPER = 1L << 3,
      /** in case of an invalid trigger setting, the UNKNOWN_MODE is set and
       *  the test pattern trigger is used */
      UNKNOWN_MODE = 1L << 4,
      // first five bits come originally from the engineering format
      // keep three additional bits reserved
      // and use the upper eight bits for artificial modes like SLC
      /** Set if the DOM triggered with SLC condition met */
      SLC_READOUT = 1L << 8,
      /** Set if a launch is triggered by a min bias hit
       * (q.v. IceTop calibration/monitoring) */
      MIN_BIAS = 1L << 9,
      /** limit this enum on a 16 bit value range */
      LAST_TRIGGER_SITUATION = 1 << 16
    };

private:
    /**  
     * This is the time (in nsec) in 25 nsec units, of the DOM clock 
     * which launches the ATWD (launch is synchronized to the first clock 
     * transition AFTER the discriminator fires 
     */
    double startTime_;  

    /** 
     * Raw ATWD channel 0 to 3
     */
    std::vector<std::vector<int> > rawATWD_;

    /** 
     * This holds the 40 MHz FADC data 
     */
    std::vector<int> rawFADC_;

    /**
     * Raw course charge stamp
     * These values are already unpacked to full 10-bit numbers
     */
    std::vector<int> rawChargeStamp_;

    /**  
     * This represents the trigger type
     */
    TriggerType trigger_;
    
    /**
     * This signals special trigger circumstances
     */
    TriggerMode mode_;

    /**  
     * This tells which ATWD in the DOM was used
     */
    ATWDselect whichATWD_;

    /**
     *  chargeStampHighestSample runs from 1-16, and points to the highest fADC
     *  sample in the first 16 samples; this is from the charge stamp
     */
    unsigned int chargeStampHighestSample_;

    /*  chargeStampRange_ has been deleted - on the rare occassions anyone cares, 
     *  they can see if any of the samples are above 512  */

    /**
     * Places to store the IceTop-style SLC radouts
     *   rawATWDChargeStamp_ : Sum of all 128 channels in selected ATWD, after pedestal sub.
     */
    unsigned int rawATWDChargeStamp_;

    /**
     * Places to store the IceTop-style SLC radouts
     *   whichATWDChargeStamp_ : which of the 3 ATWD channels was used in sum
     */
    unsigned int whichATWDChargeStamp_;

    /** 
     * This holds the local coincidence bit
     */
    bool localCoincidence_;

    /**
     * pedestal_ is a bool that tells you if the pedestal_ has or has not
     * been subtracted from the waveforms. This is only for delta-compressed
     * waveforms. True means pedestal has been subtracted. Flase means it
     * has not been subtracted.
     */
    bool pedestal_;
  

public:
    /**
     * Default constructor.
     */
    I3DOMLaunch();

    /**
     * Destructor.
     */
    ~I3DOMLaunch();
  
    /**
     * Return ATWD/FADC launch time.
     */
    double GetStartTime() const { return startTime_; }

    /**
     * Set ATWD/FADC launch time.
     */
    void SetStartTime(double starttime) { startTime_ = starttime; }
    
    /**
     * Return the trigger type/why this launch was recorded
     * (SPE_DISCRIMINATOR_TRIGGER is the standard 'physics data').
     */
    TriggerType GetTriggerType() const { return trigger_; }
    
    /**
     * Specify the trigger type/why this launch was recorded.
     */
    void SetTriggerType(TriggerType trigger) { trigger_ = trigger; }

    /**
     * Signals the special trigger circumstances. 
     *
     * Note: In testdomapp, the current FPGA implementation in the DOM, there is
     * no information available to indicate which neighbouring DOM
     * caused the LC requirement to be met. Rather, modes LC_UPPER and LC_LOWER
     * reflect how the DOM was configured at the time this launch
     * was recorded. So if signals from both upper and lower DOMs are required
     * to satisfy local coincidence, LC_UPPER and LC_LOWER will be set.
     */
    TriggerMode GetTriggerMode() const { return mode_; }
    
    /**
     * Sets the special trigger circumstances.
     */
    void SetTriggerMode(TriggerMode situation) { mode_ = situation; }

    /**
     * Return which ATWD.
     */
    ATWDselect GetWhichATWD() const { return whichATWD_; }

    /**
     * Set which ATWD.
     */
    void SetWhichATWD(ATWDselect WhichATWD) { whichATWD_ = WhichATWD; }

    /**
     * Return raw ATWD by channel number
     */
    const std::vector<int>& GetRawATWD(unsigned int channel) const
    {
      if(channel >= rawATWD_.size())
        log_fatal("Accessing channel %u in a launch with %zu channels.", channel, rawATWD_.size());
      return rawATWD_[channel];
    }

    std::vector<int>& GetRawATWD(unsigned int channel)
    {
      if(channel >= rawATWD_.size())
        log_fatal("Accessing channel %u in a launch with %zu channels.", channel, rawATWD_.size());
      return rawATWD_[channel];
    }

    const std::vector<std::vector<int> >& GetRawATWDs() const{ return rawATWD_; }
    std::vector<std::vector<int> >& GetRawATWDs() { return rawATWD_; }

    void SetRawATWD( std::vector<std::vector<int> >& v ){ rawATWD_ = v; }

    /**
     * Return raw FADC waveform.
     */
    const std::vector<int>& GetRawFADC() const { return rawFADC_; }
    std::vector<int>& GetRawFADC() { return rawFADC_; }

    void SetRawFADC( std::vector<int>& v){ rawFADC_ = v; }

    /**
     * Return local coincidence bit.
     */
    bool GetLCBit() const { return localCoincidence_; }
  
    /**
     * Set the local coincidence bit.
     */
    void SetLCBit(bool LCBit) {localCoincidence_=LCBit;}
  
    /**
    * return Pedestal bool.
    */
   bool GetIsPedestalSub() const { return pedestal_; }
  
    /**
    * Set the pedestal bool
    */
   void SetIsPedestalSub(bool Pedestal) {pedestal_ = Pedestal;}
  
  /** 
     * Return the raw charge stamp.
     */
    const std::vector<int>& GetRawChargeStamp() const { return rawChargeStamp_; }
    std::vector<int>& GetRawChargeStamp() { return rawChargeStamp_; }

    /**
     * Return charge stamp highest sample
     */
    unsigned int GetChargeStampHighestSample() const { return chargeStampHighestSample_; }

    /** 
     * Set the charge stamp highest sample
     */
    void SetChargeStampHighestSample(unsigned int highsample)
    {
      chargeStampHighestSample_ = highsample;
    };
    /**
     * Return/Set ATWDCharge stamp, used for IceTop style SLC readout
     */
    unsigned int GetRawATWDChargeStamp() const { return rawATWDChargeStamp_; }
    void SetRawATWDChargeStamp(unsigned int chargesum)
    {
      rawATWDChargeStamp_ = chargesum;
    }
    /**
     * Return/Set ATWDCharge channel (which ATWD was used), used for IceTop style SLC readout
     */
    unsigned int GetWhichATWDChargeStamp() const { return whichATWDChargeStamp_; }
    void SetWhichATWDChargeStamp(unsigned int channelid)
    {
      whichATWDChargeStamp_ = channelid;
    }

    std::ostream& Print(std::ostream&) const;
    
private:
    friend class icecube::serialization::access;
	
    template <class Archive> void save(Archive & ar, unsigned version) const;
    template <class Archive> void load(Archive & ar, unsigned version);
	I3_SERIALIZATION_SPLIT_MEMBER();
};

I3_CLASS_VERSION(I3DOMLaunch, i3domlaunch_version_);
I3_POINTER_TYPEDEFS(I3DOMLaunch);

typedef std::vector<I3DOMLaunch> I3DOMLaunchSeries;
typedef I3Map<OMKey, I3DOMLaunchSeries> I3DOMLaunchSeriesMap;

I3_POINTER_TYPEDEFS(I3DOMLaunchSeries);
I3_POINTER_TYPEDEFS(I3DOMLaunchSeriesMap);

bool operator==(const I3DOMLaunch& lhs, const I3DOMLaunch& rhs);
std::ostream& operator<<(std::ostream&, const I3DOMLaunch&);

/**
 * Bit operators to combine different trigger modes.
 * 
 * As an example:
 * One might set I3DOMLaunch::LC_LOWER | I3DOMLaunch::LC_UPPER, if the local
 * coincidence condition for the lower and upper DOM is met at the same time.
 */
inline I3DOMLaunch::TriggerMode
operator|(I3DOMLaunch::TriggerMode a, I3DOMLaunch::TriggerMode b)
{
  return I3DOMLaunch::TriggerMode(static_cast<int>(a) | static_cast<int>(b));
}


inline I3DOMLaunch::TriggerMode
operator&(I3DOMLaunch::TriggerMode a, I3DOMLaunch::TriggerMode b)
{
  return I3DOMLaunch::TriggerMode(static_cast<int>(a) & static_cast<int>(b));
}


inline I3DOMLaunch::TriggerMode
operator^(I3DOMLaunch::TriggerMode a, I3DOMLaunch::TriggerMode b)
{
  return I3DOMLaunch::TriggerMode(static_cast<int>(a) ^ static_cast<int>(b));
}


inline I3DOMLaunch::TriggerMode&
operator|=(I3DOMLaunch::TriggerMode& a, I3DOMLaunch::TriggerMode b)
{
  return a = a | b;
}


inline I3DOMLaunch::TriggerMode&
operator&=(I3DOMLaunch::TriggerMode& a, I3DOMLaunch::TriggerMode b)
{
  return a = a & b;
}


inline I3DOMLaunch::TriggerMode&
operator^=(I3DOMLaunch::TriggerMode& a, I3DOMLaunch::TriggerMode b)
{
  return a = a ^ b;
}


inline I3DOMLaunch::TriggerMode
operator~(I3DOMLaunch::TriggerMode a)
{
  return I3DOMLaunch::TriggerMode(~static_cast<int>(a));
}

std::ostream& operator<<(std::ostream& oss, const I3DOMLaunch& d);


#endif // I3DOMLAUNCH_H_INCLUDED
