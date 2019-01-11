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

#include <icetray/I3Module.h>

struct TrashCan : public I3Module
{
  TrashCan(const I3Context& context) : I3Module(context) { }
  void Physics(I3FramePtr frame) { }
  void Monitor(I3FramePtr frame) { }
  void TimeCal(I3FramePtr frame) { }
  void Geometry(I3FramePtr frame) { }
  void Calibration(I3FramePtr frame) { }
  void DetectorStatus(I3FramePtr frame) { }
  void OtherStops(I3FramePtr frame) { }
};

I3_MODULE(TrashCan);

