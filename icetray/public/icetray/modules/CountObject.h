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
#ifndef ICETRAY_COUNTOBJECT_H_INCLUDED
#define ICETRAY_COUNTOBJECT_H_INCLUDED

#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>

#include <map>

/**
 * @brief An icetray module which will count the number of
 * frame objects at some location and verify against 
 * some expected number passed as an argument.
 */
class CountObject : public I3Module
{
 public:
  CountObject(const I3Context& context);

  virtual ~CountObject();
  
  void Configure();

  void Physics(I3FramePtr);
  void Calibration(I3FramePtr);
  void DetectorStatus(I3FramePtr);
  void Geometry(I3FramePtr);

  void Finish();
  
 private:

  std::string where_;
  unsigned expected_;
  unsigned seen_;
};

#endif
