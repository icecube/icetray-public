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
#ifndef ICETRAY_I3PHYSICSUSAGE_H_INCLUDED
#define ICETRAY_I3PHYSICSUSAGE_H_INCLUDED

#include <string>
#include <icetray/I3PointerTypedefs.h>

struct I3PhysicsUsage
{
  double systime, usertime;
  unsigned ncall;
};

std::ostream& 
operator<<(std::ostream& os, const I3PhysicsUsage& ru);

I3_POINTER_TYPEDEFS(I3PhysicsUsage);

#endif


