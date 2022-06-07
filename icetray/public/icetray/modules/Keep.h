/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Torsten Schmidt
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
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
  std::set<I3Frame::Stream> streams_;
  
  // private copy constructors and assignment
  Keep(const Keep&);
  Keep& operator=(const Keep&);


  // logging
  SET_LOGGER ("Keep");
};

#endif /*KEEP_H_INCLUDED*/
