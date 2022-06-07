/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
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

#include <I3Test.h>
#include <icetray/IcetrayFwd.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Context.h>
#include <icetray/test/ConstructorTest.h>
#include <icetray/modules/Keep.h>
#include <icetray/modules/AddNulls.h>

#include <string>
using namespace std;

TEST_GROUP(CleanConstructorTests);

struct HasCrashingConstructor 
{
  HasCrashingConstructor(const I3Context& context) 
  { 
    log_fatal("this constructor crashes");
  }
};

TEST(CrashingConstructor)
{
  try {
    clean_constructor_test<HasCrashingConstructor>();
    FAIL("that should have thrown");
  } catch (const std::exception& e) { 
    /* ok */ 
  }
}

struct HasCleanConstructor 
{
  HasCleanConstructor(const I3Context& context) 
  { 
    log_trace("this constructor is fine");
  }
};

TEST(CleanConstructor)
{
  clean_constructor_test<HasCleanConstructor>();
}



class CleanConstModule : public I3Module
{
public:
  CleanConstModule(const I3Context& c) : I3Module(c) {}
};


TEST(CleanConstModule)
{
  clean_constructor_test<CleanConstModule>();
}
