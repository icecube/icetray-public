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
#ifndef ICETRAY_FRAMECHECK_H_INCLUDED
#define ICETRAY_FRAMECHECK_H_INCLUDED

#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>

#include <map>

/**
 * @brief An icetray module which will check whether a configurable
 * set of frame elements is (or is not) present on each stream.
 */
class FrameCheck : public I3Module
{
 public:
  FrameCheck(const I3Context& context);

  virtual ~FrameCheck();
  
  void Configure();

  void DAQ(I3FramePtr frame);

  void Physics(I3FramePtr frame);

  void DetectorStatus(I3FramePtr frame);

  void Calibration(I3FramePtr frame);

  void Geometry(I3FramePtr frame);
  
 private:

  void CheckFrameForKeys(I3FramePtr frame,
			 const I3Frame::Stream& stream);

  struct predicates {
    std::vector<std::string> has;
    std::vector<std::string> hasnt;
  };

  typedef std::map<I3Frame::Stream, predicates> streammap_t;


  streammap_t keys_;

};

#endif
