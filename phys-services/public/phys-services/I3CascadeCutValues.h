/**
    copyright  (C) 2005
    the icecube collaboration
    $Id$
    @author mdagost
*/

#ifndef I3CASCADECUTVALUES_H
#define I3CASCADECUTVALUES_H

#include <phys-services/I3CutValuesBase.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/I3Position.h>

template <typename Key, typename Value> struct I3Map;
class OMKey;
class I3Particle;
class I3RecoPulse;
class I3Geometry;


/**
 * @brief A class to store the basic hit information for cascades from the event
 */
class I3CascadeCutValues : public I3CutValuesBase
{
 public:
  int Nchan;
  int Nhit;
  int N_1hit;
  int Nstring;
  int Ndir;
  int Nearly;
  int Nlate;
  I3Position cog;

  I3CascadeCutValues() :
    Nchan(-1),
    Nhit(-1),
    N_1hit(-1),
    Nstring(-1),
    Ndir(-1),
    Nearly(-1),
    Nlate(-1){}

    void Calculate(const I3Particle& vertex,
                   const I3Geometry& geometry,
                   const I3Map< OMKey, std::vector< I3RecoPulse> >& pulsemap,
                   const double& begTWindow = I3Constants::dt_window_l,
                   const double& endTWindow = I3Constants::dt_window_h) override;

  virtual ~I3CascadeCutValues();
  
  std::ostream& Print(std::ostream&) const override;

 private:

  friend class icecube::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

};

std::ostream& operator<<(std::ostream&, const I3CascadeCutValues&);

I3_POINTER_TYPEDEFS(I3CascadeCutValues);

#endif
