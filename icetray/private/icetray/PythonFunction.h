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

#ifndef PYTHON_FUNCTION_H
#define PYTHON_FUNCTION_H

#include <iostream>
#include <set>

#include <icetray/I3Context.h>

class I3Context;

#include <icetray/I3Frame.h>
#include <icetray/I3ConditionalModule.h>
#include <icetray/impl.h>
#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>

class PythonFunction : public I3ConditionalModule
{
 public:
  PythonFunction(const I3Context& context, boost::python::object func);
  virtual ~PythonFunction();
  void Configure();
  void Process();

 private:
  boost::python::object obj;
  std::set<I3Frame::Stream> streams;
  boost::python::dict paramsd;
  std::vector<std::string> configkeys;
  boost::python::handle<> numpy_bool_type;
  boost::python::handle<> numpy_true, numpy_false;

  PythonFunction(const PythonFunction&);
  PythonFunction& operator=(const PythonFunction&);

  SET_LOGGER("PythonFunction");
};

#endif // PYTHON_FUNCTION_H

