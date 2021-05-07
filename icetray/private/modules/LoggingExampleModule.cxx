/**
 *  $Id$
 *
 *  Copyright (C) 2021 the IceCube Collaboration <http://www.icecube.wisc.edu>
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

#include <iostream>
#include <icetray/I3Module.h>
#include <icetray/I3Logging.h>

struct LoggingExampleModule : public I3Module
{
  LoggingExampleModule(const I3Context& context):
    I3Module(context){}
  virtual ~LoggingExampleModule() {}

  void Process(){
    I3FramePtr frame = PopFrame();
    // Listed in increasing level of severity and (hopefully)
    // decreasing level of verbosity.
    log_trace("This is my logging statement.  There are many like them, but this one is mine. "
	      "Logging is my best friend.  I must master logging as I must master my life. "
	      "Without me, logging is useless.  Without logging, I am useless.");
    log_debug("The most effective debugging tool is still careful thought, "
	      "coupled with judiciously placed print statements.");	      
    log_info("The human head weighs 8lbs.");
    log_warn("Danger, Will Robinson!");
    log_error("PC LOAD LETTER");
    try{
      log_fatal("halt and catch fire");
    }catch(const std::exception& e){
      std::cout<<"(cout) fatal: "<<e.what()<<std::endl;
    }
    PushFrame(frame);
  }

  SET_LOGGER("LoggingExampleModule");
};

I3_MODULE(LoggingExampleModule);
