/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Phil Roth <proth@icecube.umd.edu>
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
#include "icetray/I3ConditionalModule.h"

#include "icetray/I3Context.h"
#include "icetray/impl.h"



I3ConditionalModule::I3ConditionalModule(const I3Context& context) :
  I3Module(context)
{
  i3_log("%s", __PRETTY_FUNCTION__);

  AddParameter("IcePickServiceKey",
	       "Key for an IcePick in the context that this module should check "
	       "before processing physics frames.",
	       "");

  AddParameter("If",
	       "A python function... if this returns something that evaluates to True,"
	       " Module runs, else it doesn't",
	       if_);
  i3_log("if_=%p", if_.ptr());
}

I3ConditionalModule::~I3ConditionalModule() { }

void I3ConditionalModule::Configure_()
{
  i3_log("%s", __PRETTY_FUNCTION__);
  boost::python::object configured_if_;
  GetParameter("If", configured_if_);

  i3_log("configured_if_=%p", configured_if_.ptr());
  if (if_.ptr() != configured_if_.ptr()) // user set the parameter to something
    {
      i3_log("user passed us something");
      if (!PyCallable_Check(configured_if_.ptr()))
        log_fatal("'If' parameter to module %s must be a callable object", GetName().c_str());
      else
      {
        i3_log("user passed us something and it is a PyFunction");
        if_ = configured_if_;
        use_if_ = true;
      }
    }
  else // got nothing from user
    use_if_ = false;

  boost::python::object obj;

  i3_log("(%s) Configuring the Conditional Module stuff.",GetName().c_str());
  std::string pickKey;
  GetParameter("IcePickServiceKey",pickKey);
  use_pick_ = false;
  if(pickKey != "")
    {
      if (use_if_)
        log_fatal("Please specify either IcePickServiceKey or If, but not both");
      i3_log("Looking for pick %s in the context.",pickKey.c_str());
      pick_ = GetContext().Get<I3IcePickPtr>(pickKey);
      if(!pick_)
        log_fatal("IcePick %s was not found in the context.  "
		  "Did you install it?",pickKey.c_str());
      else
        use_pick_ = true;
    }

  // bounce this guy back to I3Module, which will in turn bounce to whoever derives
  // from us
  I3Module::Configure_(); // this also adds a default OutBox
}

bool I3ConditionalModule::ShouldDoProcess(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);

  // For all frame types except DAQ and Physics, the answer is always yes
  if (frame->GetStop() != I3Frame::DAQ && frame->GetStop() != I3Frame::Physics)
    return true;

  i3_log("use_if_=%d", use_if_);
  if (use_if_)
    {
      boost::python::object rv = if_(frame);
      bool flag = boost::python::extract<bool>(rv);
      if (flag)
        ++nexecuted_;
      else
        ++nskipped_;

      i3_log("ShouldDoProcess == %d", flag);
      return flag;
    }

  if(use_pick_)
    {
      i3_log("Sending frame to our IcePick.");
      bool flag = pick_->SelectFrameInterface(*frame);
      if (flag)
        ++nexecuted_;
      else
        ++nskipped_;
      i3_log("ShouldDoPhysics == %d", flag);
      return flag;
    }
  i3_log("%s", "No conditional execution.");
  ++nexecuted_;
  i3_log("%s", "ShouldDoPhysics == true");
  return true;
}

