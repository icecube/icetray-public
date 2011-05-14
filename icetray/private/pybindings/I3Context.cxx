//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id: ithon.cxx 25598 2006-11-25 02:52:57Z troy $
//
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <string>
#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Configuration.h>

using namespace boost::python;

bool 
context_putconfig(I3ContextPtr ctx, 
		  const std::string& where, I3ConfigurationPtr what)
{
  return ctx->Put(where, what);
}

I3ConfigurationPtr
context_getconfig(I3ContextPtr ctx)
{
	return ctx->Get<I3ConfigurationPtr>();
}

typedef std::map<std::string, std::pair<FrameFifoPtr, I3ModulePtr> > connections_t;

bool 
context_putoutboxes(I3ContextPtr ctx, 
		    const std::string& where, boost::shared_ptr<connections_t> what)
{
  return ctx->Put(where, what);
}

void register_I3Context()
{
  class_<I3Context, I3ContextPtr, boost::noncopyable >("I3Context")
    .def(init<>())
    .def("__setitem__", &context_putconfig)
    .def("__setitem__", &context_putoutboxes)
    .add_property("configuration", &context_getconfig)
    ;

  class_<connections_t, boost::shared_ptr<connections_t> >("Connections")
    ;
}
