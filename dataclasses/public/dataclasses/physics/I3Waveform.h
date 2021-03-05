/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3Waveform.h
 * @version $Revision$
 * @date $Date$
*/
#ifndef I3_WAVEFORM_H_INCLUDED
#define I3_WAVEFORM_H_INCLUDED

#include <utility>
#include <vector>
#include <dataclasses/I3Map.h>
#include <dataclasses/ScintKey.h>
#include <icetray/OMKey.h>

/**
 * List the names of enumeration members defined in this file
 * here. These can be used for e.g. pybindings, which require
 * the names of the enumeration members to be known. This list
 * should be updated whenever members or new enums are added to
 * the class.
 */
#define I3WAVEFORM_H_I3Waveform_Source (ATWD)(FADC)(TWR_ELECTRICAL)(TWR_OPTICAL)(ETC)(SLC)(SiPM)
#define I3WAVEFORM_H_I3Waveform_Status (VIRGINAL)(COMBINED)(SATURATED)(UNDERSHOT)

static const unsigned i3waveform_version_ = 3;
class I3Waveform 
{
 public:

  enum Source
  {
    ATWD = 0,
    FADC = 1,
    TWR_ELECTRICAL = 2,
    TWR_OPTICAL = 3,
    ETC = 4,
    SLC = 5,
    SiPM = 6
  };

#ifndef __CINT__
  union SourceCompound {
    struct {
      #if BYTE_ORDER == BIG_ENDIAN
      uint8_t slop   : 3;
      uint8_t hlc    : 1;
      uint8_t id     : 1;
      uint8_t source : 3;
      #else
      uint8_t source : 3; /* Source ID */
      uint8_t id     : 1; /* Source index (e.g. ATWDa/ATWDb) */
      uint8_t hlc    : 1; /* Readout type (e.g. SLC/HLC) */
      uint8_t slop   : 3; /* Unused space */
      #endif
    } fields;
    uint8_t bits;

    SourceCompound(Source source = ATWD, unsigned id = 0) 
    {
      fields.source = source;
      fields.id = (id > 0);
      fields.slop = 0;
    }
  } __attribute((packed));
#endif // __CINT__
  
  /** Describes possible artifacts within the data.
   * 
   * The waveform is a hardware independent representation of the data acquired.
   * Nevertheless, it can carry artifacts due to hardware imperfections.
   * 
   * Saturation is an example, which is hard to recognize, since the waveform is
   * a vector of doubles. Of course, it is still possible to recognize saturation
   * using some more or less fancy algorithm, but the module converting the hardware
   * dependent data into hardware independent data can recognize artifacts much easier.
   * It should record this information using this enumeration.
   * 
   * If the DOM calibrator combines the ATWD channels, it should call bins that
   * saturate even in the lowest amplified channel SATURATED, bins that saturate
   * only in some channels COMBINED and bins that do not saturate in the highest
   * amplified channel VIRGINAL. Bins that have drooped below the bias voltage
   * should be marked as UNDERSHOT. These will be combined by GetStatus() in cases
   * where the waveform is clipped on both sides, e.g. a droopy FADC waveform that
   * saturates at the beginning and undershoots in the middle will be marked
   * SATURATED | UNDERSHOT.
   *
   * CHANGED in I3Waveform version 3: SHADY => COMBINED, ADULTERATED => SATURATED
   */
   enum Status {
     VIRGINAL = 0,
     COMBINED  = (1 << 1), /* NB: 1 is sometimes used as a flag by other modules. */
     SATURATED = (1 << 2),
     UNDERSHOT = (1 << 3)
   };
  
  class StatusCompound
  {
   private:
    std::pair<unsigned long long int, unsigned long long int>
      interval_;
    Status status_;
    int8_t channel_;
  
   public:
    StatusCompound() : interval_(std::make_pair(0, 0)), status_(SATURATED), channel_(-1) {}
    
    ~StatusCompound();
    
    std::ostream& Print(std::ostream&) const;
    
    const std::pair<unsigned long long int, unsigned long long int>&
    GetInterval() const { return interval_; }
    
    std::pair<unsigned long long int, unsigned long long int>&
    GetInterval() { return interval_; }
    
    Status GetStatus() const { return status_; }
    
    void SetStatus(Status status) { status_ = status; }

    int8_t GetChannel() const { return channel_; }

    void SetChannel(int8_t channel) { channel_ = channel; }
    
    bool operator==(const StatusCompound& rhs) const
    {
      return status_ == rhs.status_
	&& interval_ == rhs.interval_;
    }
   private:
    friend class icecube::serialization::access;
    template<class Archive> void save(Archive& ar, unsigned version) const;
    template<class Archive> void load(Archive& ar, unsigned version);
    I3_SERIALIZATION_SPLIT_MEMBER();
  };

  /**
   * Returns a summary of a given waveform/status information.
   * 
   * @return ADULTERATED/SHADY if any included status compound is ADULTERATED/SHADY,
   * or VIRGINAL.
   */
  static unsigned GetStatus(const std::vector<StatusCompound>& waveformInfo);
  
  unsigned GetStatus() const;
  int GetChannel() const;

 private:
  double startTime_;
  double binWidth_;
  std::vector<double> waveform_;
  std::vector<StatusCompound> waveformInfo_;
#ifndef __CINT__
  SourceCompound source_;
  // Source source_;
#endif // __CINT__
  
 public:
  I3Waveform() :  startTime_(0), binWidth_(0), source_(ATWD) {}
  
  ~I3Waveform();
  
  std::ostream& Print(std::ostream&) const;
  
  double GetStartTime() const {return startTime_;}

  void SetStartTime(double startTime) {startTime_ = startTime;}

  double GetBinWidth() const {return binWidth_;}

  void SetBinWidth(double binWidth) {binWidth_ = binWidth;}

  const std::vector<double>& GetWaveform() const {return waveform_;}

  std::vector<double>& GetWaveform() {return waveform_;}

  void SetWaveform(const std::vector<double>& waveform) {waveform_ = waveform;}

  /**
   * Returns a status information for this waveform.
   * 
   * @return A collection of status compounds.
   * A status compound consists of an interval and a status information.
   * It describes the status of all waveform bins with indices
   * [GetInterval().first, GetInterval().second). If there are waveform bins not
   * described by a status compound, these bins are assumed to have a status equal
   * to VIRGINAL, e. g. GetWaveformInformation().empty() equal true means, all
   * waveform bins are good.
   */
  const std::vector<StatusCompound>& GetWaveformInformation() const
    {return waveformInfo_;}
  
  /**
   * Returns a status information for this waveform.
   * 
   * @return A collection of status compounds.
   * A status compound consists of an interval and a status information.
   * It describes the status of all waveform bins with indices
   * [GetInterval().first, GetInterval().second). If there are waveform bins not
   * described by a status compound, these bins are assumed to have a status equal
   * to VIRGINAL, e. g. GetWaveformInformation().empty() equal true means, all
   * waveform bins are good.
   */
  std::vector<StatusCompound>& GetWaveformInformation() {return waveformInfo_;}
  
  /**
   * Set the status information for this waveform.
   * 
   * Note: This method was added since there is a set method for the waveforms, too.
   * One might want to use the non-const get methods instead.
   * 
   * @param info The status information.
   */
  void SetWaveformInformation(const std::vector<StatusCompound>& info)
    {waveformInfo_ = info;}

  /**
   * Returns the source flag. This is a legacy function to maintain compatibility
   * with old code. It returns I3Waveform::SLC for SLC waveforms and the
   * digitizer for HLC waveforms.
   *
   * To get more detailed information use IsHLC and GetDigitizer instead.
   */
  Source GetSource() const { return IsSLC() ? I3Waveform::SLC : GetDigitizer(); }

  bool IsHLC() const { return (bool)(source_.fields.hlc); }
  bool IsSLC() const { return !(bool)(source_.fields.hlc); }

  void SetHLC(bool hlc) { source_.fields.hlc = hlc; }

  /// Returns the digitizer (ATWD or FADC)
  Source GetDigitizer() const { return (Source)(unsigned)(source_.fields.source); }

  /// Set the digitizer (ATWD or FADC). Do not set to SLC.
  void SetDigitizer(Source source) { source_.fields.source = source; }


  /**
   * Get the source index for this waveform (e.g. the ATWD chip ID)
   */
  unsigned GetSourceIndex() const { return source_.fields.id; }

  void SetSourceIndex(unsigned num) { source_.fields.id = (num > 0); }

 private:
  friend class icecube::serialization::access;
  template<class Archive> void save(Archive& ar, unsigned version) const;
  template<class Archive> void load(Archive& ar, unsigned version);
  I3_SERIALIZATION_SPLIT_MEMBER();
};

bool operator==(const I3Waveform& lhs, const I3Waveform& rhs);
std::ostream& operator<<(std::ostream& oss, const I3Waveform& wf);
std::ostream& operator<<(std::ostream& oss, const I3Waveform::StatusCompound& wf);

typedef std::vector<I3Waveform> I3WaveformSeries;
typedef I3Map<OMKey, I3WaveformSeries> I3WaveformSeriesMap;
typedef I3Map<ScintKey, I3WaveformSeries> I3ScintWaveformSeriesMap;

I3_CLASS_VERSION(I3Waveform, i3waveform_version_);
I3_CLASS_VERSION(I3Waveform::StatusCompound, i3waveform_version_);
I3_POINTER_TYPEDEFS(I3Waveform);
I3_POINTER_TYPEDEFS(I3WaveformSeries);
I3_POINTER_TYPEDEFS(I3WaveformSeriesMap);
I3_POINTER_TYPEDEFS(I3ScintWaveformSeriesMap);

#endif // I3_WAVEFORM_H_INCLUDED
