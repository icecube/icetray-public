/**
 *  Copyright (C) 2007-2010   Troy D. Straszheim  <troy@icecube.umd.edu>
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

  configkeys = configuration_.keys();

  obj = context_.Get<bp::object>("object");
  i3_log("got python object %p", obj.ptr());

  //
  // this protocol "func_code" and "co_varnames" may change
  // in a future version of python, and then it will be #ifdef time
  //
  bp::object code = obj.attr("func_code");
  bp::object argnamesobj = code.attr("co_varnames");
  i3_log("Getting argnames");
  bp::tuple argnames = bp::extract<bp::tuple>(code.attr("co_varnames"));
  unsigned argcount = bp::extract<unsigned>(code.attr("co_argcount"));
  i3_log("Getting defaults");
  bp::object defobj = obj.attr("__defaults__");
  bp::tuple deftuple;
  if (defobj.ptr() != Py_None)
    deftuple = bp::extract<bp::tuple>(defobj);
  unsigned ndefaults = bp::len(deftuple);
  i3_log("varnames are %s", repr(argnames).c_str());
  i3_log("defaults are %s", repr(deftuple).c_str());
  i3_log("argcount is %u, with %u defaults", argcount, ndefaults);

  unsigned offset = argcount - ndefaults;
  for (unsigned i = 1; i< argcount; i++)
    {
      std::string aname = bp::extract<std::string>(argnames[i]);
      i3_log("param[%u] %s", i, aname.c_str());

      std::string desc = "keyword argument '" + aname + "'";
      bp::object d;

      if ((i-1) + offset < argcount)
	{
	  i3_log("default is %s", repr(deftuple[i-1]).c_str());
	  d = deftuple[i-1];
	}
      configuration_.Add(aname, desc, d);
    }

  bp::list l;
  l.append(I3Frame::Physics);
  AddParameter("Streams", 
	       "A list of frame types which this function should fire on.  By default "
	       "runs only on physics frames",
	       l);
}

PythonFunction::~PythonFunction()
{
  i3_log("%s", __PRETTY_FUNCTION__);
}

void PythonFunction::Configure()
{
  i3_log("%s", __PRETTY_FUNCTION__);
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
  i3_log("process!");
  bp::object rawfn = raw_function(obj);
  bp::tuple tupleargs = bp::make_tuple(frame);
  bp::object rv;

  i3_log("tupleargs = %s", repr(tupleargs).c_str());
  i3_log("paramsd = %s", repr(paramsd).c_str());
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

