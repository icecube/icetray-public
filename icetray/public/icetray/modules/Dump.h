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
#ifndef ICETRAY_MODULES_DUMP_H_INCLUDED
#define ICETRAY_MODULES_DUMP_H_INCLUDED

class I3Context;

#include <icetray/I3Frame.h>
#include <icetray/I3ConditionalModule.h>

class Dump : public I3ConditionalModule
{
 public:
  Dump(const I3Context& context);
  virtual ~Dump();
  void Configure();
  void Physics(I3FramePtr frame);
  void DetectorStatus(I3FramePtr frame);
  void Geometry(I3FramePtr frame);
  void Calibration(I3FramePtr frame);

 private:
  unsigned int physicsPrescale_;
  unsigned long long int frameCount_;
  
  Dump(const Dump&);
  Dump& operator=(const Dump&);
};

#endif
