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
#ifndef ICETRAY_I3FRAMEOBJECT_H_INCLUDED
#define ICETRAY_I3FRAMEOBJECT_H_INCLUDED

#include <icetray/I3PointerTypedefs.h>

#ifdef __CINT__
#include <zlib.h>
#endif
/**
   This is the mother of all classes that can live in the I3Frame.
   Exists strictly to provide a common base class for objects that
   live in the frame
*/
class I3FrameObject
{

public:

  // doesn't do anything.  Just for RTTI purposes.
  virtual ~I3FrameObject();

  template <class Archive>
  void
  serialize(Archive & ar, unsigned version);

};

I3_POINTER_TYPEDEFS(I3FrameObject);

#endif
