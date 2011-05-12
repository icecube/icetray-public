/**
 *  $Id$
 *  
 *  Copyright (C) 2003-2008
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

#include <icetray/I3Module.h>
#include <icetray/I3Context.h>

I3Context::I3Context()
{
}

I3Context::~I3Context()
{
}

void
I3Context::dump() const
{
  ostringstream oss;
  oss << "[ I3Context @ " << this << "\n";
  for (map_t::const_iterator iter = map_.begin();
       iter != map_.end();
       iter++)
    {
      oss << "   '" << iter->first << " ==> " << I3::name_of(typeid(iter->second)) << "\n";
    }
  oss << "]\n";
  log_trace("%s", oss.str().c_str());
}

