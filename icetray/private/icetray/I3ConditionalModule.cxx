/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Phil Roth <proth@icecube.umd.edu>
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
#include "icetray/I3ConditionalModule.h"

#include "icetray/I3Context.h"
#include "icetray/impl.h"



I3ConditionalModule::I3ConditionalModule(const I3Context& context) :
  I3Module(context),
  pickKey_("")
{
  i3_log("%s", __PRETTY_FUNCTION__);
  configured_ = false;

  AddParameter("IcePickServiceKey",
	       "Key for an IcePick in the context that this module should check "
	       "before processing physics frames.",
	       pickKey_);

  if_ = boost::python::object("<some python function>");
  AddParameter("If",
	       "A python function... if this returns something that evaluates to True,"
	       " Module runs, else it doesn't",
	       if_);
  i3_log("if_=%p", if_.ptr());

  AddOutBox("OutBox");
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
      if (!PyFunction_Check(configured_if_.ptr()))
	log_fatal("'If' parameter to module %s must be a python function", GetName().c_str());
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
  GetParameter("IcePickServiceKey",pickKey_);
  use_pick_ = false;
  if(pickKey_ != "")
    {
      if (use_if_)
	log_fatal("Please specify either IcePickServiceKey or If, but not both");
      i3_log("Looking for pick %s in the context.",pickKey_.c_str());
      pick_ = GetContext().Get<I3IcePickPtr>(pickKey_);
      if(!pick_)
	log_fatal("IcePick %s was not found in the context.  "
		  "Did you install it?",pickKey_.c_str());
      else
	use_pick_ = true;
    }

  // bounce this guy back to I3Module, which will in turn bounce to whoever derives
  // from us
  I3Module::Configure_();
}

bool I3ConditionalModule::ShouldDoProcess(I3FramePtr frame)
{
  i3_log("%s", __PRETTY_FUNCTION__);

  //
  //  You only run this function if it is a physics frame, otherwise
  //  things would get complicated, e.g. 
  //
  //    if 'thing' in frame and frame['thing'].value .
  //
  //  Otherwise return true so that the module runs...  and it
  //  probably won't do anything, as it probably doesn't implement
  //  Geometry(), etc.
  // 
  //  Though we're using this to check physics,
  //  we actually override ShouldDoProcess as some ConditionalModules
  //  like Keep implement Process(), not Physics()
  if (frame->GetStop() != I3Frame::Physics)
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

      i3_log("ShouldDoPhysics == %d", flag);
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
