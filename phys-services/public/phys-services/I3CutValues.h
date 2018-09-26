/**
    copyright  (C) 2005
    the icecube collaboration
    $Id$
    @author dule
*/

#ifndef I3CUTVALUES_H
#define I3CUTVALUES_H

#include <vector>
#include <phys-services/I3CutValuesBase.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/I3Position.h>

template <typename Key, typename Value> struct I3Map;
class OMKey;
class I3Particle;
class I3RecoPulse;
class I3Geometry;

/**
 * @brief A class to store the basic hit information from the event
 */
class I3CutValues : public I3CutValuesBase
{
 public:
  int Nchan;
  int Nhit;
  int Nstring;
  int Ndir;
  double Ldir;
  double Sdir;
  double Sall;
  I3Position cog;

  I3CutValues() :
    Nchan(-1),
    Nhit(-1),
    Nstring(-1),
    Ndir(-1),
    Ldir(NAN),
    Sdir(NAN),
    Sall(NAN){}

  void Calculate(const I3Particle& track,
                 const I3Geometry& geometry,
                 const I3Map< OMKey, std::vector< I3RecoPulse> >& pulsemap,
                 const double& begTWindow = I3Constants::dt_window_l,
                 const double& endTWindow = I3Constants::dt_window_h) override;

  virtual ~I3CutValues();
  
  std::ostream& Print(std::ostream&) const override;

 private:

  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

};

std::ostream& operator<<(std::ostream&, const I3CutValues&);

I3_POINTER_TYPEDEFS(I3CutValues);

#endif
