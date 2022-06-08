//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
//

#ifndef __STD_CONT_POD_PYBINDINGS
#define __STD_CONT_POD_PYBINDINGS

#include <icetray/OMKey.h>
#include <icetray/I3Frame.h>
#include <icetray/impl.h>
#include <vector>
#include <string>

using namespace boost::python;
using std::vector;
using std::string;

template <typename T1, typename T2> 
struct std_pair_to_tuple 
{ 
  static PyObject* convert(std::pair<T1, T2> const& p) 
  { 
    return boost::python::incref(boost::python::make_tuple(p.first, p.second).ptr()); 
  } 
}; 

template <typename T1, typename T2> 
struct std_pair_to_python_converter 
{ 
  std_pair_to_python_converter() 
  { 
    boost::python::to_python_converter< 
    std::pair<T1, T2>, 
      std_pair_to_tuple<T1, T2> >(); 
  } 
};

template <typename T>
std::string vector_repr(const std::vector<T>& v)
{
  boost::python::list l(v);
  return repr(l);
}

template <typename T>
static
void reg(const char* name)
{
  std::string s = "vector_";
  s += name; 
  class_<vector<T> >(s.c_str())
    .def(vector_indexing_suite<vector<T> >())
    .def("__repr__", &vector_repr<T>)
    ;
  from_python_sequence<vector<T>, variable_capacity_policy>();
}

template <typename Map>
class i3_map_extras : public def_visitor<i3_map_extras<Map> >
{
  static list keys(Map const&  x)
  {
    list t;
    for(typename Map::const_iterator it = x.begin(); it != x.end(); it++)
      t.append(it->first);
    return t;
  }

  template <typename Class>
  void visit(Class& cl) const
  {
    // std::cout << __PRETTY_FUNCTION__ << "\n";
    cl
      .def("keys", &keys)
      ;
  }

  friend class boost::python::def_visitor_access;
};

#endif
