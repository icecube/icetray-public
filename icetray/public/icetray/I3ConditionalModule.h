/**
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Phil Roth           <proth@icecube.umd.edu>
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
#ifndef ICETRAY_I3CONDITIONALMODULE_H_INCLUDED
#define ICETRAY_I3CONDITIONALMODULE_H_INCLUDED

#include "icetray/I3Module.h"
#include "icetray/I3IcePick.h"

/**
 * @brief This class is meant to be a wedge between an I3Module and any module
 * that wants to take advantage of conditional execution functionality.  That 
 * functionality resides here because the very general I3Module class doesn't
 * want to get too specialized.
 *
 */

class I3ConditionalModule : public I3Module
{
 public:
  I3ConditionalModule(const I3Context& context);
  ~I3ConditionalModule();

  /**
   * @brief The function where frames are sent to IcePicks installed in the context
   * in order to determine whether the module should run.
   */
  bool ShouldDoProcess(I3FramePtr frame);

  bool ShouldProcess(I3FramePtr frame) 
  { 
    log_fatal("%s", "\nThis function does nothing, don't use it."
	      "ShouldProcess is now calculated by I3Module");
    return true; 
  }

  SET_LOGGER("I3ConditionalModule");

 protected:
  /**
   * @brief Simple method that allows derived modules to access information about 
   * how many times their Physics() function has been executed.
   */
  unsigned GetTimesExecuted() { return nexecuted_; };

  /**
   * @brief Simple method that allows derived modules to access information about 
   * how many times their Physics() function has been skipped due to conditional
   * execution.
   */
  unsigned GetTimesSkipped() { return nskipped_; };

  void Configure_();
  
 private:

  I3IcePickPtr pick_;
  boost::python::object if_;

  unsigned nexecuted_;
  unsigned nskipped_;
  bool use_if_;
  bool use_pick_;
};

#endif
