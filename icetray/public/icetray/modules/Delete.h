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
 *  Deletes things from frame.  Has special privileges granted by I3Frame.
 */
class Delete : public I3Module
{
  Delete();
  Delete(const Delete&);

  Delete& operator=(const Delete&);

  std::vector<std::string> delete_keys_;

  void do_delete(I3FramePtr frame);

  public:

  Delete(const I3Context& ctx);

  virtual ~Delete() { }

  void Configure();

  void Process();

  void Finish();

  SET_LOGGER("Delete");
};


#endif
