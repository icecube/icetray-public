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

#include "PythonFunction.h"

using namespace std;
namespace bp = boost::python;

PythonFunction::PythonFunction(const I3Context& context, bp::object func)
  : I3ConditionalModule(context), obj(func)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  AddOutBox("OutBox");

  unsigned implicit_args = 1; // frame
#if PY_MAJOR_VERSION >= 3
  if (!PyObject_HasAttrString(obj.ptr(), "__code__")) {
    if (PyObject_HasAttrString(obj.ptr(), "__func__"))
      obj = obj.attr("__func__");
#else
  if (!PyObject_HasAttrString(obj.ptr(), "func_code")) {
    if (PyObject_HasAttrString(obj.ptr(), "im_func"))
      obj = obj.attr("im_func");
#endif
    else
      obj = obj.attr("__call__");
    implicit_args = 2; // self, frame
  }
  i3_log("got python object %p", obj.ptr());

  //
  // this protocol "func_code" and "co_varnames" may change
  // in a future version of python, and then it will be #ifdef time
  //
#if PY_MAJOR_VERSION >= 3
  bp::object code = obj.attr("__code__");
#else
  bp::object code = obj.attr("func_code");
#endif
  bp::object argnamesobj = code.attr("co_varnames");
  i3_log("Getting argnames");
  bp::tuple argnames = bp::extract<bp::tuple>(code.attr("co_varnames"));
  unsigned argcount = bp::extract<unsigned>(code.attr("co_argcount"));
  i3_log("Getting defaults");
#if PY_MAJOR_VERSION >= 3
  bp::object defobj = obj.attr("__defaults__");
#else
  bp::object defobj = obj.attr("func_defaults");
#endif
  bp::tuple deftuple;
  if (defobj.ptr() != Py_None)
    deftuple = bp::extract<bp::tuple>(defobj);
  unsigned ndefaults = bp::len(deftuple);
  i3_log("varnames are %s", repr(argnames).c_str());
  i3_log("defaults are %s", repr(deftuple).c_str());
  i3_log("argcount is %u, with %u defaults", argcount, ndefaults);

  unsigned offset = argcount - ndefaults;
  for (unsigned i = implicit_args; i< argcount; i++)
    {
      std::string aname = bp::extract<std::string>(argnames[i]);
      i3_log("param[%u] %s", i, aname.c_str());

      std::string desc = "keyword argument '" + aname + "'";
      bp::object d;

      if (i >= offset)
	{
	  i3_log("default is %s", repr(deftuple[i-offset]).c_str());
	  d = deftuple[i-offset];
          configuration_.Add(aname, desc, d);
	}
	else
	{
	  configuration_.Add(aname, desc);
	}
    }

  bp::list l;
  l.append(I3Frame::Physics);
  AddParameter("Streams",
	       "A list of frame types which this function should fire on.  By default "
	       "runs only on physics frames",
	       l);

  // try to get the numpy bool type
  // don't use a handle, this is okay if it fails
  PyObject* numpy_module = PyImport_ImportModule("numpy");

  // don't use a handle here either... this is a borrowed reference
  PyObject* errobj = PyErr_Occurred();
  PyErr_Clear();

  if (numpy_module != 0 and errobj == 0)
    {
      numpy_bool_type = bp::handle<>(PyObject_GetAttrString(numpy_module, 
							    "bool_"));
      
      numpy_true = bp::handle<>(PyObject_GetAttrString(numpy_module,
						       "True_"));
      
      numpy_false = bp::handle<>(PyObject_GetAttrString(numpy_module,
							"False_"));
    }
  Py_XDECREF(numpy_module);
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

  configkeys = configuration_.keys();
  for (unsigned i = 0; i< configkeys.size(); i++)
    {
      i3_log("param %s", configkeys[i].c_str());
      if (configkeys[i] != "Streams" && configkeys[i] != "If" && configkeys[i] != "IcePickServiceKey")
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
    else if ((void*)p->ob_type == (void*)numpy_bool_type.get())
      {
	if (p == numpy_true.get())
	  flag = true;
	else if (p == numpy_false.get())
	  flag = false;
	else
	  log_fatal("Python function '%s' returned object of type numpy.bool_, but it isn't either numpy.True_ or numpy.False_... what gives?",
		    GetName().c_str());
      }
    else
      {
	bp::handle<> h(p);
	rv = bp::object(h);
	flag = bp::extract<bool>(rv);
	i3_log("Extracted value %d", flag);
      }
    i3_log("frame @ %p", frame.get());

    if (flag)
      PushFrame(frame,"OutBox");

  } catch (const bp::error_already_set& as) {

    log_fatal("Python function inside PythonFuncton module '%s' returned value %s, which won't convert to True, False, or None", 
	      GetName().c_str(), repr(rv).c_str());
    throw as;
  }

}

