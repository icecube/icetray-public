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

void register_std_cont_pod()
{
  reg<string>("string");
  reg<double>("double");
  reg<int>("int");
  reg<short>("short");
  reg<char>("char");
  reg<unsigned>("unsigned");
  reg<unsigned long>("ulong");
  reg<OMKey>("OMKey");
  reg<I3Frame::Stream>("I3Frame_Stream");

  class_<std::map<int, int> >("map_int_int")
    .def(map_indexing_suite<std::map<int, int> >())
    .def(i3_map_extras<std::map<int, int> >())
    ;

  class_<std::map<OMKey, int> >("map_OMKey_int")
    .def(map_indexing_suite<std::map<OMKey, int> >())
    ;

  std_pair_to_python_converter<unsigned long long int, unsigned long long int>();
  std_pair_to_python_converter<string, I3FrameObjectConstPtr>();
  std_pair_to_python_converter<float, float>();
}

