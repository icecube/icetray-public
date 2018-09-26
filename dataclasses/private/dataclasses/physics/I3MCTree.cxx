/**
 * @file I3MCTree.cxx
 * @brief Implementation for I3MCTree
 * 
 * copyright (C) 2013 the icecube collaboration
 * 
 * $Id$
 * @version $Revision$
 * @date $Date$
 */

#include <cassert>

#include <icetray/serialization.h>
#include "dataclasses/physics/I3MCTree.h"

namespace TreeBase {
template<>
std::ostream& Tree<I3Particle,I3ParticleID>::Print(std::ostream& s) const{
  s << "[I3MCTree:\n";
  I3Position pos;
  for(Tree::const_iterator iter=cbegin(),end=cend(); iter!=end; iter++){
    for(unsigned int d=0; d<depth(*iter)+1; d++)
      s << "  ";
    s << iter->GetMinorID() << " " << iter->GetTypeString() << " ";
    pos = iter->GetPos();
    s << "(" << pos.GetX()/I3Units::m << "m, ";
    s << pos.GetY()/I3Units::m << "m, ";
    s << pos.GetZ()/I3Units::m << "m) ";
    s << "(" << iter->GetZenith()/I3Units::degree << "deg, ";
    s << iter->GetAzimuth()/I3Units::degree << "deg) ";
    s << iter->GetTime()/I3Units::ns << "ns ";
    s << iter->GetEnergy()/I3Units::GeV << "GeV ";
    s << iter->GetLength()/I3Units::m << "m\n";
  }
  s << ']';
  return(s);
}
} //namespace TreeBase

I3_SERIALIZABLE(I3MCTree);
