/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3RecoPulse.h
 * @date $Date$
 */

#ifndef I3RECOPULSE_H_INCLUDED
#define I3RECOPULSE_H_INCLUDED

#include "dataclasses/Utility.h"
#include "dataclasses/I3Vector.h"
#include "icetray/OMKey.h"
#include "dataclasses/I3Map.h"

/**
 * @brief A storage class for extracted pulses from a feature extractor
 * A readout independent representation of a waveform feature or Analog
 *  readout.
 */
static const unsigned i3recopulse_version_ = 3;

class I3RecoPulse 
{
  double time_;
  float charge_;
  float width_;
  typedef uint8_t flags_t;
  flags_t flags_;

  public:
  enum PulseFlags {
	LC = (1 << 0),
	ATWD = (1 << 1),
	FADC = (1 << 2),
  };

  /**
   * Construct a pulse with all properties initialized to Not-a-Number.
   */
  I3RecoPulse() : time_(NAN), charge_(NAN), width_(NAN), flags_(0) {}

  /**
   * Print a string representation of this pulse
   */
  std::ostream& Print(std::ostream&) const;
  
  /**
   * @brief Get the flag values pertaining to this pulse, as a bitwise 
   *        combination.
   */
  flags_t GetFlags() const { return flags_; } 

  /**
   * @brief Set the flag values pertaining to this pulse, as a bitwise
   *        combination.
   */
  void SetFlags(flags_t flags) { flags_ = flags; } 

  /**
   * @brief Get the start time of the pulse.
   */
  double GetTime() const {return time_;}

  /**
   * @brief Set the start time of the pulse.
   */
  void SetTime(double time) {time_ = time;}

  /**
   * @brief Get the amplitude of the pulse in units of ideally amplified
   *        photoelectrons.
   */
  float GetCharge() const {return charge_;}

  /**
   * @brief Set the amplitude of the pulse in units of ideally amplified
   *        photoelectrons.
   */
  void SetCharge(float charge) {charge_ = charge;}

  /**
   * @brief The time between this pulse and the subsequent basis function
   *        used in the pulse unfolding.
   *
   * This quantity can be used to approximate the time interval within which the
   * pulse charged was observed as [ GetTime() , GetTime() + GetWidth() ]. This
   * approximation becomes dubious, however, when the charge density is high
   * (such that several adjacent basis functions in the unfolding were assigned
   * nonzero amplitudes).
   */
  float GetWidth() const {return width_;}

  /**
   * @brief Store the time between this pulse and the subsequent basis function
   *        used in the pulse unfolding.
   */
  void SetWidth(float width) {width_ = width;}

  ~I3RecoPulse();

  bool operator==(const I3RecoPulse& rhs) const;
  bool operator!=(const I3RecoPulse& rhs) const { return !operator==(rhs); }

  private:
  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);
};

I3_POINTER_TYPEDEFS(I3RecoPulse);
I3_CLASS_VERSION(I3RecoPulse, i3recopulse_version_);

typedef std::vector<I3RecoPulse> I3RecoPulseSeries;
typedef I3Map<OMKey, I3RecoPulseSeries> I3RecoPulseSeriesMap;
typedef I3Map<OMKey, I3RecoPulse> I3RecoPulseMap;

std::ostream& operator<<(std::ostream& oss, const I3RecoPulse& p);

I3_POINTER_TYPEDEFS(I3RecoPulseSeries);
I3_POINTER_TYPEDEFS(I3RecoPulseSeriesMap);
I3_POINTER_TYPEDEFS(I3RecoPulseMap);

/*
 * Specialize I3Frame::Get() to turn convert various objects
 * in the frame into I3RecoPulseSeriesMaps.
 */

// need to hide this from ROOT
#ifndef __CINT__
#include "icetray/I3Frame.h"

template <>
I3RecoPulseSeriesMapConstPtr
I3Frame::Get(const std::string& name, void*, void*) const;
#endif //__CINT__

#endif //I3RECOPULSE_H_INCLUDED


