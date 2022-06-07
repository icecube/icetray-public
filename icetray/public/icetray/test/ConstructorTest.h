#ifndef CONSTRUCTOR_TEST_H
#define CONSTRUCTOR_TEST_H
/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  John Pretz
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
#include "icetray/I3Configuration.h"
#include "icetray/I3Context.h"
#include "icetray/I3Frame.h"

template <class T>
void clean_constructor_test(const std::string& outbox = "OutBox")
{
  typedef std::map<std::string, std::pair<FrameFifoPtr, I3ModulePtr> > outboxmap_t;
  I3Context context;
  {
    I3ConfigurationPtr config(new I3Configuration());
    context.Put(config);
    
    boost::shared_ptr<outboxmap_t> ob(new outboxmap_t);
    (*ob)[outbox] = make_pair(FrameFifoPtr(), I3ModulePtr());
    context.Put("OutBoxes",ob);

    T module(context);
  }  
}

#endif
