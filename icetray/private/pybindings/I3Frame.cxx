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

#include <icetray/I3Frame.h>
#include <icetray/serialization.h>
#include <icetray/python/boost_serializable_pickle_suite.hpp>

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
    .def(init<I3Frame>())
    .def("Has", (bool (I3Frame::*)(const std::string &, const I3Frame::Stream &) const)&I3Frame::Has)
    .def("Has", (bool (I3Frame::*)(const std::string &) const)&I3Frame::Has)
    .def("Delete", &I3Frame::Delete)
    .def("__delitem__", &I3Frame::Delete)
    .def("Rename", &I3Frame::Rename)
    .def(self_ns::str(self))
    .def("Put", frame_put)
    .def("Put", frame_put_on_stream)
    .def("Get", &frame_get<I3FrameObject>)
    // more pythonic interface
    .def("__contains__", (bool (I3Frame::*)(const std::string &) const)&I3Frame::Has)
    .def("__getitem__", &frame_get<I3FrameObject>)
    .def("__setitem__", put_fn_p)
    .def("__delitem__", &I3Frame::Delete)
    .def("keys", &frame_keys)
    .def("values", &frame_values)
    .def("items", &frame_items)
    .def("size", (I3Frame::size_type (I3Frame::*)() const)&I3Frame::size)
    .def("__len__", (I3Frame::size_type (I3Frame::*)() const)&I3Frame::size)
    .add_property("Stop", (I3Frame::Stream (I3Frame::*)() const)&I3Frame::GetStop, &I3Frame::SetStop)
    .def("get_stop", (I3Frame::Stream (I3Frame::*)(const std::string&) const)&I3Frame::GetStop)
    .def("change_stream", &I3Frame::ChangeStream)
    .def("merge", &I3Frame::merge)
    .def("take", (void (I3Frame::*)(const I3Frame&,const std::string&)) &I3Frame::take)
    .def("take", (void (I3Frame::*)(const I3Frame&,const std::string&,const std::string&)) &I3Frame::take)
    .def("purge", (void (I3Frame::*)())&I3Frame::purge)
    .def("purge", (void (I3Frame::*)(const I3Frame::Stream&))&I3Frame::purge)
    .def("assign", &I3Frame::assign)
    .def("clear", &I3Frame::clear)
    .def("type_name", (std::string (I3Frame::*)(const std::string&) const) &I3Frame::type_name)
    .def("size", (I3Frame::size_type (I3Frame::*)(const std::string&) const) &I3Frame::size)
    .def("as_xml", &I3Frame::as_xml)
    .def_readonly("None", I3Frame::None)
    .def_readonly("Geometry", I3Frame::Geometry)
    .def_readonly("Calibration", I3Frame::Calibration)
    .def_readonly("DetectorStatus", I3Frame::DetectorStatus)
    .def_readonly("DAQ", I3Frame::DAQ)
    .def_readonly("Physics", I3Frame::Physics)
    .def_readonly("TrayInfo", I3Frame::TrayInfo)
    .def_pickle(boost_serializable_pickle_suite<I3Frame>())
    ;

  class_<I3Frame::Stream>("Stream")
    .def(init<char>())
    .def(self == self) // comparison operator
    .def(self != self) // comparison operator
    .def(self < self) // comparison operator
    .def("__str__", &I3Frame::Stream::str)
    .def("__repr__", format_stream)
    .add_property("id", &I3Frame::Stream::id)
    ;

  class_<std::vector<I3FramePtr> >("vector_I3Frame")
    .def(vector_indexing_suite<std::vector<I3FramePtr>, true>())
    ;
  from_python_sequence<std::vector<I3FramePtr>, variable_capacity_policy>();
}
