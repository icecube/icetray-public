/**
 *  $Id$
 *  
 *  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
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

#include <iostream>
#include <set>

#include <icetray/I3Context.h>

using namespace std;

class I3Context;

#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/impl.h>
#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>

namespace bp = boost::python;

class PythonFunction : public I3Module
{
 public:
  PythonFunction(const I3Context& context);
  virtual ~PythonFunction();
  void Configure();
  void Process();

 private:
  bp::object obj;
  std::set<I3Frame::Stream> streams;
  bp::dict paramsd;
  std::vector<std::string> configkeys;

  PythonFunction(const PythonFunction&);
  PythonFunction& operator=(const PythonFunction&);

  SET_LOGGER("PythonFunction");

};

I3_MODULE(PythonFunction);


PythonFunction::PythonFunction(const I3Context& context)
  : I3Module(context)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  AddOutBox("OutBox");
  bp::list l;
  l.append(I3Frame::Physics);
  AddParameter("Streams", 
	       "A list of frame types which this function should fire on.  By default "
	       "runs only on physics frames",
	       l);

  configkeys = configuration_.keys();

  for (unsigned i = 0; i< configkeys.size(); i++)
    {
      i3_log("param %s", configkeys[i].c_str());
      if (configkeys[i] != "Streams")
	configuration_.Add(configkeys[i], "keyword argument", "(nil)");
    }
}

PythonFunction::~PythonFunction()
{
  i3_log("%s", __PRETTY_FUNCTION__);
}

void PythonFunction::Configure()
{
  i3_log("%s", __PRETTY_FUNCTION__);
  obj = context_.Get<bp::object>("object");
  log_debug("got python object %p", obj.ptr());
  std::vector<I3Frame::Stream> svec;
  GetParameter("Streams", svec);
  streams = std::set<I3Frame::Stream>(svec.begin(), svec.end());

  for (unsigned i = 0; i< configkeys.size(); i++)
    {
      i3_log("param %s", configkeys[i].c_str());
      if (configkeys[i] != "Streams")
	paramsd[configkeys[i]] = configuration_.Get(configkeys[i]);
    }
}

void PythonFunction::Process()
{
  i3_log("%s", __PRETTY_FUNCTION__);
  I3FramePtr frame = PopFrame();
  if (!frame)

    {

      log_fatal("No frame in inbox:  python functions cannot be driving modules.");
      return;
    }
  if (streams.find(frame->GetStop()) == streams.end())
    {
      i3_log("stream not found");
      PushFrame(frame);
      return;
    }
  log_debug("process!");
  bp::object rawfn = raw_function(obj);
  bp::tuple tupleargs = bp::make_tuple(frame);
  bp::object rv;

  PyObject* p = PyObject_Call(obj.ptr(), tupleargs.ptr(), paramsd.ptr());
  bool flag;
  if (p == NULL)
    {
      log_error("Error running python function as module:");
      throw bp::error_already_set();
    }
  try {
    if (p == Py_None)
      {
	flag = true;
	i3_log("Keeping frame because we got None");
      }
    else
      {
	bp::handle<> h(p);
	rv = bp::object(h);
	flag = bp::extract<bool>(rv);
	i3_log("Extracted boolean value %d", flag);
      }
    i3_log("frame @ %p", frame.get());

    if (flag)
      PushFrame(frame,"OutBox");

  } catch (const bp::error_already_set& as) {
    
    PyObject* o = PyObject_Repr(rv.ptr());
    log_fatal("Python function inside PythonFuncton module '%s' returned value %s, which won't convert to True, False, or None", 
	      GetName().c_str(), PyString_AsString(o));
    Py_DECREF(o);
    throw as;
  }

}

