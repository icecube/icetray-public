#ifndef CONSTRUCTOR_TEST_H
#define CONSTRUCTOR_TEST_H
/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  John Pretz
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
