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

