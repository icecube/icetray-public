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
#include <icetray/I3DSORegistry.h>

using namespace std;

// meyers singleton access
I3DSORegistry&
I3DSORegistry::instance()
{
  static I3DSORegistry registry;
  return registry;
}

/// append dso to list of dsos iff it isnt there already
void
I3DSORegistry::register_dso(const string& lib)
{
  if (dsos_.empty())
    dsos_.push_back(lib);
  else if (dsos_.back() != lib)
    dsos_.push_back(lib);
}

vector<string>
I3DSORegistry::dsos() 
{
  return dsos_;
}

vector<string> I3DSORegistry::dsos_;

