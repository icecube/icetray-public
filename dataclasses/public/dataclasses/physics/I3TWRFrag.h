/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * 
 * @file I3TWRFrag.h
 * @version $Revision$
 * @date $Date$
 */
#ifndef I3TWRFRAG_H_INCLUDED
#define I3TWRFRAG_H_INCLUDED

// forward declarations


// header files

#include <vector>

#include <dataclasses/Utility.h>

// definitions


/** A fragment of the direct/raw digital readout of an Amanda OM using the TWR DAQ.
 * 
 * The digital readout is fragment into waveform elements that show some pulse activity,
 * to reduce the data acquired by a TWR.
 * Every fragment has a start bin that determines the position of the fragment within
 * the waveform relative to its start.
 */
class I3TWRFrag 
{
  private:
  unsigned int startBin_;
  std::vector<unsigned int> rawFADC_;

  public:
  /** Default constructor.
   */
  I3TWRFrag() : startBin_(0u){}
  /** Destructor.
   */
  ~I3TWRFrag();
  
  std::ostream& Print(std::ostream&) const;

  /** Returns the position of the fragment within the waveform.
   * 
   * @return The position.
   */
  unsigned int GetStartBin() const { return(startBin_); }
  /** Sets the position of the fragment within the waveform.
   * 
   * @param startBin The position.
   */
  void SetStartBin(unsigned int startBin) { startBin_ = startBin;}  
  /** Provides access to the waveform information of the fragment.
   *
   * @return The waveform information.
   */
  const std::vector<unsigned int>& GetWaveform() const { return(rawFADC_); }
  /** Provides access to the waveform information of the fragment.
   *
   * @return The waveform information.
   */
  std::vector<unsigned int>& GetWaveform() { return(rawFADC_); }

  private:
  friend class icecube::serialization::access;
  template<class Archive> void serialize(Archive& ar, unsigned version);
};

bool operator==(const I3TWRFrag& lhs, const I3TWRFrag& rhs);

std::ostream& operator<<(std::ostream&, const I3TWRFrag&);

static const unsigned i3twrfrag_version_ = 1;
I3_CLASS_VERSION(I3TWRFrag, i3twrfrag_version_);

I3_POINTER_TYPEDEFS(I3TWRFrag);

#endif /*I3TWRFRAG_H_INCLUDED*/


