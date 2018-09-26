#ifndef I3DOMSTATUS_H_INCLUDED
#define I3DOMSTATUS_H_INCLUDED

#include "dataclasses/Utility.h"
#include <icetray/OMKey.h>
#include <serialization/version.hpp>

static const unsigned i3domstatus_version_ = 6;

class I3DOMStatus 
{
public:
  I3DOMStatus():
    trigMode(UnknownTrigMode),
       lcMode(UnknownLCMode),
       txMode(UnknownLCMode),
       lcSpan(INT_MAX),
       lcWindowPre(NAN),
       lcWindowPost(NAN),
       pmtHV(NAN),
       speThreshold(NAN),
       dacTriggerBias0(NAN),
       dacTriggerBias1(NAN),
       dacFADCRef(NAN),
       mpeThreshold(NAN),
       nBinsATWD0(0),
       nBinsATWD1(0),
       nBinsATWD2(0),
       nBinsATWD3(0),
       nBinsFADC(0),
       statusATWDa(Unknown),
       statusATWDb(Unknown),
       statusFADC(Unknown),
       deltaCompress(Unknown),
       fbState(Unknown),
       fbBrightness(INT_MAX),
       fbLength(INT_MAX),
       fbDelay(INT_MAX),
       fbMask(INT_MAX),
       fbRate(INT_MAX),
       domGainType(UnknownGainType),
       cableType(UnknownCableType),
       SLCActive(false)
  {};

  ~I3DOMStatus();
  
  bool operator==(const I3DOMStatus& rhs) const
  {
    return (trigMode == rhs.trigMode &&
        lcMode == rhs.lcMode &&
        txMode == rhs.txMode &&
        lcWindowPre == rhs.lcWindowPre &&
        lcWindowPost == rhs.lcWindowPost &&
        lcSpan == rhs.lcSpan &&
        statusATWDa == rhs.statusATWDa &&
        statusATWDb == rhs.statusATWDb &&
        statusFADC == rhs.statusFADC &&
        pmtHV == rhs.pmtHV &&
        speThreshold == rhs.speThreshold &&
        dacTriggerBias0 == rhs.dacTriggerBias0 &&
        dacTriggerBias1 == rhs.dacTriggerBias1 &&
        dacFADCRef == rhs.dacFADCRef &&
        nBinsATWD0 == rhs.nBinsATWD0 &&
        nBinsATWD1 == rhs.nBinsATWD1 &&
        nBinsATWD2 == rhs.nBinsATWD2 &&
        nBinsATWD3 == rhs.nBinsATWD3 &&
        nBinsFADC == rhs.nBinsFADC &&
        deltaCompress == rhs.deltaCompress &&
        fbState == rhs.fbState &&
        fbBrightness == rhs.fbBrightness &&
        fbLength == rhs.fbLength &&
        fbDelay == rhs.fbDelay &&
        fbMask == rhs.fbMask &&
        fbRate == rhs.fbRate &&
        domGainType == rhs.domGainType &&
        cableType == rhs.cableType &&
        SLCActive == rhs.SLCActive &&
        mpeThreshold == rhs.mpeThreshold);
  }
  bool operator!=(const I3DOMStatus& rhs) const
  {
    return !operator==(rhs);
  }
  
  /**
   * The different kinds of triggers that can be used for the dom
   * The standard data-taking operation is SPE
   */
  enum TrigMode { UnknownTrigMode = -1 , TestPattern = 0 , CPU = 1, SPE = 2, Flasher=3, MPE=4 };
  /**  
   * This controls how far the local coincidence signal is sent and received
   *   Turns out you can not encode SLC active or not here.  
   *   IF SoftLC is set here, then ONLY SLC hits are to be expected.
   *    There is a SLCActive bool now in the this struct.
   *
   *  Each dom has two LC settings:
   *    lcMode - how the dom is configured to listen for LC signals
   *    txMode - how the dom is configured to generate LC signals
   */
  enum LCMode {UnknownLCMode = -1, LCOff = 0, UpOrDown = 1, Up = 2, Down = 3, UpAndDown =4, SoftLC=5};
  /**
   * There is also provision to turn on or off various settings in the 
   * DOM
   */
  enum OnOff {Unknown = -1, Off = 0, On = 1};

 /**
   * IceTop DOMs can be a "high gain" or a "low gain" variety
   *    all InIce DOMs are "high gain"
   */
  enum DOMGain {UnknownGainType = -1, High = 0, Low = 1};

    /**
     * Whether the DOM has a terminated ("A" or "T") or unterminated ("B" or "U") cable.
     * Perhaps this should go in I3OMGeo? Maybe not, since that class could deal with 
     * AMANDA OMs and this only with IceCube DOMs. Perhaps a bool would work here, but I'm
     * not sure what the story is with IceTop DOMs. For now keep the "don't know" option.
     */
    enum CableType 
    {
	UnknownCableType = -1,
	Terminated = 0,
	Unterminated = 1
    };
    
   
  /**
   * DOM internal trigger mode
   */
  TrigMode trigMode;
  
  /**
   * LC RX mode:  hard, soft, none, etc:  how this dom listens for LC info
   */
  LCMode lcMode;

  /**
   * LC TX mode:  hard, soft, none, etc:  how this dom broadcasts  LC info
   */
  LCMode txMode;
  
  /**
   * Number of nearest neighbors to consider in LC search.(1,2,3 or 4)
   */
  unsigned int lcSpan;

  /**
   *  Local coincidence window size, before ATWD launch window
   */
  double lcWindowPre;

  /**
   * Local coincidence window size, after ATWD launch window
   */
  double lcWindowPost;
  
  /**
   * Real operating PMT HV (Volts)
   */
  double pmtHV;

  /**
   *  Single PE (SPE) discriminator level. 
   */
  double speThreshold;
  /**
   * the Pedestal voltage of the analog front end.
   */
  double fePedestal;
  
  /**
   * Sets ATWD0 sampling speed. Higher DAC setting is faster sample rate.
   */
  double dacTriggerBias0;

  /**
   * Sets ATWD1 sampling speed. Higher DAC setting is faster sample rate.
   */
  double dacTriggerBias1;

  /**
   * FADC_DAC_REF - reference setting for FADC baseline
   */
  double dacFADCRef;

  /**
   *  Multiple PE (MPE) discriminator level. 
   */
  double mpeThreshold;
  
  /**
   * NBins per Engineering readout.  Keep in mind, delta-compressed data automatically adjusts number of
   *    readouts per Launch based on waveform size, but if present, should always be 128 samples.
   */
  unsigned int nBinsATWD0;
  unsigned int nBinsATWD1;
  unsigned int nBinsATWD2;
  unsigned int nBinsATWD3;
  unsigned int nBinsFADC;
  
  OnOff statusATWDa;
  OnOff statusATWDb;
  OnOff statusFADC;

  /**
   *  IS the dom configured to make delta-compression readouts?
   */
  OnOff deltaCompress;
    
  /**
   * Flasher board state and flasher settings
   */
  OnOff fbState;

  unsigned int fbBrightness;
  unsigned int fbLength;
  unsigned int fbDelay;
  unsigned int fbMask;
  unsigned int fbRate;
  /**
   * Is this DOM a high gain or low gain
   */
  DOMGain domGainType;

     /**
     * Cable type: unknown, terminated, unterminated
     */
    CableType cableType;

  /**
   *  SLCActive - bool to indicate if SLC readouts are active 
   *     as well as the HLC readouts described above.
   */
  bool SLCActive;

private:
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, const unsigned version);
};

typedef std::map<OMKey, I3DOMStatus> I3DOMStatusMap;
I3_POINTER_TYPEDEFS(I3DOMStatusMap);

I3_POINTER_TYPEDEFS(I3DOMStatus);
I3_CLASS_VERSION(I3DOMStatus, i3domstatus_version_);

#endif //I3DOMSTATUS_H_INCLUDED

