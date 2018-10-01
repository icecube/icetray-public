/**
 *  $Id$
 *  
 *  Copyright (C) 2007
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
#include <icetray/I3FrameObject.h>
#include <icetray/Utility.h>

#include <cxxabi.h>

I3FrameObject::~I3FrameObject() { }

template <class Archive>
void
I3FrameObject::serialize(Archive & ar, unsigned version)
{
}

I3_BASIC_SERIALIZABLE(I3FrameObject);

I3_SERIALIZATION_SHARED_PTR(I3FrameObject);

std::ostream& I3FrameObject::Print(std::ostream& os) const
{
  const char* mangled=typeid(*this).name();
  int status = 0;
  char* demangled = abi::__cxa_demangle(mangled, 0, 0, &status);
  os << "[I3FrameObject: " << (status==0 ? stlfilt(demangled) : mangled) << "]";
  free(demangled);
  return(os);
}
