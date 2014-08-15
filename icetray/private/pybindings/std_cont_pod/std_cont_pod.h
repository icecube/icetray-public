//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
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
