/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
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
