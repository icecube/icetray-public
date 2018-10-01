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
#ifndef ICETRAY_PHYSICSBUFFER_H
#define ICETRAY_PHYSICSBUFFER_H

#include <icetray/I3Module.h>

#include <string>
#include <deque>

/**
 * This module buffers physics frames.  The important thing about a 

 */

class PhysicsBuffer : public I3Module
{
  // deque:  Double-Ended Queue
  std::deque<I3FramePtr> buffer_;
  unsigned desired_buffersize_;
  unsigned batchpush_n_;
  unsigned n_;

public:

  PhysicsBuffer (const I3Context& context);
  void Configure();
  void Physics(I3FramePtr frame);
  void Finish();
};

#endif 
