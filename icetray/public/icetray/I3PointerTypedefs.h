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
#ifndef ICETRAY_I3POINTERTYPEDEFS_H_INCLUDED
#define ICETRAY_I3POINTERTYPEDEFS_H_INCLUDED

// for generating pointer typedefs in dataclass structures.
// pointer to const object and pointer to object
#define I3_POINTER_TYPEDEFS(C)			\
  typedef shared_ptr<C> C##Ptr;			\
  typedef shared_ptr<const C> C##ConstPtr;

#define I3_FORWARD_DECLARATION(C)  class C;	I3_POINTER_TYPEDEFS(C)

#endif
