/**
 * copyright  (C) 2004
 * the icecube collaboration
 * @version $Id$
 * @file I3RecoHit.h
 * @date $Date$
 */

#ifndef I3RECOHIT_H_INCLUDED
#define I3RECOHIT_H_INCLUDED

#include "dataclasses/Utility.h"
#include "dataclasses/I3Vector.h"
#include "dataclasses/I3Map.h"
#include "icetray/OMKey.h"


/**
 * @brief A base class for reconstruted hits.  
 *
 * Carries no information beyond 'hit'
 * but can be sub-classed as part of a hit series reconstruction.
 *
 */
static const unsigned i3recohit_version_ = 1;

class I3RecoHit 
{
 public:
  I3RecoHit() : time_(NAN),hitID_(-1),sourceIndex_(-1){}

  ~I3RecoHit();
  
  std::ostream& Print(std::ostream&) const;

  double GetTime() const { return time_; }

  void SetTime(double time) { time_ = time; }

  int GetID() const { return hitID_; }

  void SetID(const int hitid) { hitID_ = hitid; }
/**
 *  GetSourceIndex - The Source Index can be used to keep track
 *    of which rawReadout index (from readout vector, like DOMLaunchSeries,
 *    that gave rise to this pariticular hit.  Index value defaults to -1.
 */
  int GetSourceIndex() const { return sourceIndex_; }
/**
 *  SetSourceIndex - The Source Index can be used to keep track
 *    of which rawReadout index (from readout vector, like DOMLaunchSeries,
 *    that gave rise to this pariticular hit.  Index value defaults to -1.
 */
  void SetSourceIndex(const int srcid) { sourceIndex_ = srcid; }

  bool operator==(const I3RecoHit& rhs) const ;

 private:

  double time_;

  int hitID_;

  int sourceIndex_;

  friend class icecube::serialization::access;

  template <class Archive> void serialize(Archive & ar, unsigned version);
};

std::ostream& operator<<(std::ostream&, const I3RecoHit&);

I3_POINTER_TYPEDEFS(I3RecoHit);
I3_CLASS_VERSION(I3RecoHit, i3recohit_version_);

typedef std::vector<I3RecoHit> I3RecoHitSeries;
typedef I3Map<OMKey, I3RecoHitSeries> I3RecoHitSeriesMap;

I3_POINTER_TYPEDEFS(I3RecoHitSeries);
I3_POINTER_TYPEDEFS(I3RecoHitSeriesMap);

#endif //I3RECOHIT_H_INCLUDED


