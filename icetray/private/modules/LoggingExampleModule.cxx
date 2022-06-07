/**
 *  $Id$
 *
 *  Copyright (C) 2021 the IceCube Collaboration <http://www.icecube.wisc.edu>
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
