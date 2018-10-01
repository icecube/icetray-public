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
#ifndef ICETRAY_I3INT_H_INCLUDED
#define ICETRAY_I3INT_H_INCLUDED

#include <icetray/IcetrayFwd.h>
#include <icetray/I3FrameObject.h>

//
//  Dumb-struct holders of PODS should *not* automatically convert
//  to/from the underlying type lest people think that it is necessary
//  to use, say, vector<I3Double> instead of just vector<double>.
//  They are instead holders of values, with member "value".
//
struct I3Int : public I3FrameObject
{
  int value;

  I3Int();
  I3Int(int i);
  I3Int(const I3Int&);
  I3Int& operator=(const I3Int&);

  template <typename Archive>
    void serialize(Archive&,unsigned);

};

I3_POINTER_TYPEDEFS(I3Int);

#endif
