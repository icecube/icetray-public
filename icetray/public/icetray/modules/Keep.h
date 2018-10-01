/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Torsten Schmidt
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
#ifndef KEEP_H_INCLUDED
#define KEEP_H_INCLUDED

// forward declarations

class I3Context;

// header files

#include <set>
#include <string>
#include <vector>

#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>
#include <icetray/I3ConditionalModule.h>

// definitions


/** Clean up frame and keep only objects with names that match any some given keys.
 *
 * Keep supports one parameter:
 * <ul>
 * <li><VAR>Keys</VAR> (keep frame objects with names that match any of these keys
 * (default value is "I3Calibration", "I3DetectorStatus", "I3Geometry",
 * "DrivingTime", "I3EventHeader")).
 * </ul>
 */
class Keep : public I3ConditionalModule
{
 public:
  /** Constructor.
   *
   * @param context the I3Context object containing this objects services.
   */
  Keep(const I3Context& context);
  /** Destructor.
   */
  virtual ~Keep();
  void Configure();
  void Process();

 private:
  std::vector<std::string> keysParam_;
  std::set<std::string> keys_;


  // private copy constructors and assignment
  Keep(const Keep&);
  Keep& operator=(const Keep&);


  // logging
  SET_LOGGER ("Keep");
};

#endif /*KEEP_H_INCLUDED*/
