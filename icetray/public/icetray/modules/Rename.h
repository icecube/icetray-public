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
#ifndef ICETRAY_RENAME_H_INCLUDED
#define ICETRAY_RENAME_H_INLCUDED

#include <fstream>
#include <string>
#include <set>
#include <icetray/I3Module.h>
#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Logging.h>

/**
 *  Renames things in the frame.  Has special privileges granted by
 *  I3Frame.
 */
class Rename : public I3Module
{
  Rename();
  Rename(const Rename&);

  Rename& operator=(const Rename&);

  /** A vector of things to rename.  Goes from_1, to_1, from_2,
      to_2... from_n, to_n.  Obviously must have an even number of
      elements.
  */
  std::vector<std::string> rename_keys_;

  void do_rename(I3FramePtr frame);

public:

  Rename(const I3Context& ctx);

  virtual ~Rename() { }

  void Configure();

  void Process();

  void Finish();

  SET_LOGGER("Rename");
};


#endif
