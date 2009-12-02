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

#include <icetray/I3Frame.h>
#include <icetray/serialization.h>

using namespace boost::python;

template <typename T>
inline static 
boost::shared_ptr<T> frame_get(const I3Frame* f, const std::string& where)
{
  if (!f->Has(where))
    {
      PyErr_SetString(PyExc_KeyError, where.c_str());
      throw_error_already_set();
      return boost::shared_ptr<T>();
    }

  try {
    boost::shared_ptr<const T> thing = f->Get<boost::shared_ptr<const T> >(where,true);
    return boost::const_pointer_cast<T>(thing);
  } catch (const boost::archive::archive_exception& e) { 
    std::ostringstream message;
    message << "Frame caught exception \"" << e.what() << "\" for key \"" << where << "\" of type " << f->type_name(where);
    PyErr_SetString(PyExc_RuntimeError,message.str().c_str());
    throw boost::python::error_already_set(); 
    return boost::shared_ptr<T>();
  }
}

//struct I3FrameProxy : I3Frame
//{
//  typedef map<string, I3FrameObjectConstPtr>::value_type value_type;
//  typedef map<string, I3FrameObjectConstPtr>::key_type key_type;
//  typedef map<string, I3FrameObjectConstPtr>::difference_type difference_type;
//  typedef map<string, I3FrameObjectConstPtr>::iterator iterator;
//  typedef map<string, I3FrameObjectConstPtr>::key_compare key_compare;
//  key_compare key_comp() { return std::less<std::string>(); }
//  I3FrameObjectPtr operator[](const std::string& key)
//  {
//    return frame_get<I3FrameObject>(this, key);
//  }
//};

// static vector<string>
// keys(I3Frame* f)
// {
//   vector<string> v;
//   I3Frame::typename_iterator iter = f->typename_begin();
//   while(iter != f->typename_end())
//     {
//       v.push_back(iter->first);
//       iter++;
//     }
//   return v;
// }


// template <typename T>
// std::pair<T, T> range(const I3Frame& f)
// {
//     return frame_range(f.begin(), f.end());
// }

// struct frame_iterators
// {
//   typedef I3Frame::const_iterator iterator;
//   static iterator begin(I3Frame& x) { return x.begin(); }
//   static iterator end(I3Frame& x) { return x.end(); }
// };
// 

static list frame_keys(I3Frame const& x)
{
        list t;
        for(I3Frame::typename_iterator it = x.typename_begin(); it != x.typename_end(); it++)
          t.append(it->first);
        return t;
}

static list frame_values(I3Frame const& x)
{
        list t;
        for(I3Frame::typename_iterator it=x.typename_begin(); it!=x.typename_end(); ++it)
          t.append(frame_get<I3FrameObject>(&x, it->first));
        return t;
}

static list frame_items(I3Frame const& x)
{
  list t;
  for(I3Frame::typename_iterator it=x.typename_begin(); it!=x.typename_end(); ++it)
    {
      t.append(boost::python::make_tuple(it->first, frame_get<I3FrameObject>(&x, it->first)));
    }
  return t;
}

static void frame_put(I3Frame& f, const std::string& s, I3FrameObjectConstPtr fop)
{
  f.Put(s, fop);
}

static void frame_put_on_stream(I3Frame& f, 
				const std::string& s, 
				I3FrameObjectConstPtr fop,
				const I3Frame::Stream& stream)
{
  f.Put(s, fop, stream);
}

// used for I3Frame::Stream::__repr__
std::string format_stream(const I3Frame::Stream& s)
{
  return std::string("icetray.I3Frame.") + s.str();
}

void register_I3Frame()
{
  void (I3Frame::* put_fn_p)(const std::string&, I3FrameObjectConstPtr) = &I3Frame::Put;

  using boost::python::iterator;
  scope i3frame_scope = 
    class_<I3Frame, boost::shared_ptr<I3Frame> >("I3Frame")
    .def(init<I3Frame::Stream>())
    .def(init<char>())
    .def("Has", &I3Frame::Has)
    .def("Delete", &I3Frame::Delete)
    .def("__delitem__", &I3Frame::Delete)
    .def("Rename", &I3Frame::Rename)
    .def(self_ns::str(self))
    .def("Put", frame_put)
    .def("Put", frame_put_on_stream)
    .def("Get", &frame_get<I3FrameObject>)
    // more pythonic interface
    .def("__contains__", &I3Frame::Has)
    .def("__getitem__", &frame_get<I3FrameObject>)
    .def("__setitem__", put_fn_p)
    .def("__delitem__", &I3Frame::Delete)
    .def("keys", &frame_keys)
    .def("values", &frame_values)
    .def("items", &frame_items)
    .def("size", (I3Frame::size_type (I3Frame::*)() const)&I3Frame::size)
    .def("__len__", (I3Frame::size_type (I3Frame::*)() const)&I3Frame::size)
    .def("GetStop", &I3Frame::GetStop)
    .def("SetStop", &I3Frame::SetStop)
    .add_property("Stop", &I3Frame::GetStop, &I3Frame::SetStop)
    .def("merge", &I3Frame::merge)
    .def("purge", (void (I3Frame::*)())&I3Frame::purge)
    .def("purge", (void (I3Frame::*)(const I3Frame::Stream&))&I3Frame::purge)
    .def("assign", &I3Frame::assign)
    .def("clear", &I3Frame::clear)
    .def("type_name", (std::string (I3Frame::*)(const std::string&) const) &I3Frame::type_name)
    .def("size", (I3Frame::size_type (I3Frame::*)(const std::string&) const) &I3Frame::size)
    .def_readonly("None", I3Frame::None)
    .def_readonly("Geometry", I3Frame::Geometry)
    .def_readonly("Calibration", I3Frame::Calibration)
    .def_readonly("DetectorStatus", I3Frame::DetectorStatus)
    .def_readonly("Physics", I3Frame::Physics)
    .def_readonly("TrayInfo", I3Frame::TrayInfo)
    ;

  class_<I3Frame::Stream>("Stream")
    .def(init<char>())
    .def(self == self) // comparison operator
    .def(self < self) // comparison operator
    .def("__str__", &I3Frame::Stream::str)
    .def("__repr__", format_stream)
    .add_property("id", &I3Frame::Stream::id)
    ;
}
