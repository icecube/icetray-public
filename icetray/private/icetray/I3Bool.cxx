/**
 *  $Id$
 *  
 *  Copyright (C) 2008
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */
#include <icetray/serialization.h>
#include <icetray/I3Bool.h>

I3Bool::I3Bool() : value(false) { }

I3Bool::I3Bool(bool b) : value(b) { }

I3Bool::I3Bool(const I3Bool& rhs) : value(rhs.value) { }

I3Bool& 
I3Bool::operator=(const I3Bool& rhs)
{
  // careful of self-assignment
  if (this != &rhs)
    value = rhs.value;

  return *this;
}

template <class Archive>
void
I3Bool::serialize(Archive& ar,unsigned)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("value", value);
}

I3_SERIALIZABLE(I3Bool);

