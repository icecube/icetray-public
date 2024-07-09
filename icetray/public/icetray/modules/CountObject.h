/**
 *  $Id$
 *
 *  Copyright (C) 2007 Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Copyright (C) 2007 the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 *  SPDX-License-Identifier: BSD-2-Clause
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
