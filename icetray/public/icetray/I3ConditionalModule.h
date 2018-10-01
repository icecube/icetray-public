/**
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  Phil Roth           <proth@icecube.umd.edu>
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
