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
#ifndef ICETRAY_I3DSOREGISTRY_H_INCLUDED
#define ICETRAY_I3DSOREGISTRY_H_INCLUDED

#include <vector>
#include <string>
#include <boost/utility.hpp>

// simple class keeps track of what libs get loaded and in what order
class I3DSORegistry : boost::noncopyable
{
  // singleton
  I3DSORegistry() { }

  static I3DSORegistry& instance();

  static std::vector<std::string> dsos_;

public:

  static void register_dso(const std::string& lib);

  static std::vector<std::string> dsos();

};


#endif


