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
#ifndef ICETRAY_DELETE_H_INCLUDED
#define ICETRAY_DELETE_H_INLCUDED

#include <fstream>
#include <string>
#include <set>
#include <icetray/I3Module.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Logging.h>

/**
 *  Copyies things from frame.  Has special privileges granted by I3Frame.
 */
class Copy : public I3Module
{
  Copy();
  Copy(const Copy&);

  Copy& operator=(const Copy&);

  std::vector<std::string> copy_keys_;

  void do_copy(I3FramePtr frame);

public:

  Copy(const I3Context& ctx);
  virtual ~Copy() { }

  void Configure();

  void Process();

  void Finish();

  SET_LOGGER("Copy");
};


#endif
