/**
 *  Copyright (C) 2007-2010   Troy D. Straszheim  <troy@icecube.umd.edu>
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

#include "PythonFunction.h"

using namespace std;
namespace bp = boost::python;

PythonFunction::PythonFunction(const I3Context& context, bp::object func)
  : I3ConditionalModule(context), obj(func)
{
  i3_log("%s", __PRETTY_FUNCTION__);
  AddOutBox("OutBox");

  unsigned implicit_args = 1; // frame

  if (!PyObject_HasAttrString(obj.ptr(), "__code__")) {
    if (PyObject_HasAttrString(obj.ptr(), "__func__"))
      obj = obj.attr("__func__");
    else
      obj = obj.attr("__call__");
    implicit_args = 2; // self, frame
  }
  i3_log("got python object %p", obj.ptr());

  //
  // this protocol "func_code" and "co_varnames" may change
  // in a future version of python, and then it will be #ifdef time
  //
  bp::object code = obj.attr("__code__");
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

