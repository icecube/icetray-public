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

#include <vector>

#include <dataclasses/physics/I3RecoPulse.h>
#include <icetray/I3Frame.h>
#include <icetray/python/dataclass_suite.hpp>
#include <dataclasses/ostream_overloads.hpp>

using namespace boost::python;

static I3RecoPulseSeriesMapPtr
from_frame(I3Frame &frame, const std::string &name)
{
	if (!frame.Has(name)) {
		PyErr_SetString(PyExc_KeyError, name.c_str());
		throw_error_already_set();
	}
	I3RecoPulseSeriesMapConstPtr rpsm =
	    frame.Get<I3RecoPulseSeriesMapConstPtr>(name);
	if (!rpsm) {
		PyErr_SetString(PyExc_TypeError, name.c_str());
		throw_error_already_set();
	}
	return boost::const_pointer_cast<I3RecoPulseSeriesMap>(rpsm);
}

namespace {
static PyBufferProcs rps_bufferprocs;

static int
I3RecoPulseSeries_getbuffer(PyObject *obj, Py_buffer *view, int flags)
{
	if (view == NULL) {
		PyErr_SetString(PyExc_ValueError, "NULL view");
		return -1;
	}

	view->shape = NULL;
	view->internal = NULL;
	view->suboffsets = NULL;
	view->buf = NULL;

	handle<> self(boost::python::borrowed(obj));
	object selfobj(self);
	std::vector<I3RecoPulse> &ts =
	    extract<std::vector<I3RecoPulse> &>(selfobj)();
	if (ts.size() == 0) {
		PyErr_SetString(PyExc_BufferError, "Pulse series is empty.");
		view->obj = NULL;
		return -1;
	}
	if ((flags & PyBUF_WRITABLE) == PyBUF_WRITABLE) {
		PyErr_SetString(PyExc_BufferError, "Cannot provide writable "
		    "contiguous buffer.");
		view->obj = NULL;
		return -1;
	}
	if ((flags & PyBUF_F_CONTIGUOUS) == PyBUF_F_CONTIGUOUS) {
		PyErr_SetString(PyExc_BufferError, "Cannot provide FORTRAN "
		    "contiguous buffer.");
		view->obj = NULL;
		return -1;
	}

	view->obj = obj;
	view->len = 2*ts.size()*sizeof(double);
	view->itemsize = sizeof(double);
	if (flags & PyBUF_FORMAT)
		view->format = (char *)"d";
	else
		view->format = NULL;
	view->ndim = 2;

	view->shape = new Py_ssize_t[2];
	view->shape[0] = ts.size();
	view->shape[1] = 2;

	// To honor a contiguous buffer request, make a copy of the
	// data. This violates the spirit of the buffer protocol
	// slightly, but both simplifies the API and allows a
	// faster memory copy than iterating over the list in Python
	// would.

	view->buf = malloc(view->len);
	view->readonly = 1;

	view->strides = new Py_ssize_t[2];
	view->strides[0] = 2*view->itemsize;
	view->strides[1] = view->itemsize;

	int j = 0;
	for (auto i : ts) {
		((double *)view->buf)[2*j + 0] = i.GetTime();
		((double *)view->buf)[2*j + 1] = i.GetCharge();
		j++;
	}

	view->suboffsets = NULL;
	view->internal = view->buf;

        Py_INCREF(obj);

        return 0;
}

static void
I3RecoPulseSeries_relbuffer(PyObject *obj, Py_buffer *view)
{
	if (view->strides != NULL)
		delete [] view->strides;
	if (view->shape != NULL)
		delete [] view->shape;
	if (view->suboffsets != NULL)
		delete [] view->suboffsets;
	if (view->buf != NULL)
		free(view->buf);
}
}


void register_I3RecoPulse()
{
  object rps = class_<std::vector<I3RecoPulse> >("vector_I3RecoPulse")
    .def(dataclass_suite<std::vector<I3RecoPulse> >())
    ;

  // Add buffer protocol interface
  PyTypeObject *rpsclass = (PyTypeObject *)rps.ptr();
  rps_bufferprocs.bf_getbuffer = I3RecoPulseSeries_getbuffer;
  rps_bufferprocs.bf_releasebuffer = I3RecoPulseSeries_relbuffer;
  rpsclass->tp_as_buffer = &rps_bufferprocs;
#if PY_MAJOR_VERSION < 3
  rpsclass->tp_flags |= Py_TPFLAGS_HAVE_NEWBUFFER;
#endif


  class_<I3RecoPulseSeriesMap, bases<I3FrameObject>, I3RecoPulseSeriesMapPtr>("I3RecoPulseSeriesMap")
    .def(dataclass_suite<I3RecoPulseSeriesMap>())
    .def("from_frame", &from_frame, args("frame", "key"),
        "Get an I3RecoPulseSeriesMap from the frame, performing any necessary "
        "format conversions behind the scenes.")
    .staticmethod("from_frame")
    ;
  register_pointer_conversions<I3RecoPulseSeriesMap>();

  scope outer = 
  class_<I3RecoPulse, boost::shared_ptr<I3RecoPulse> >("I3RecoPulse")
    #define PROPS (Time)(Charge)(Width)(Flags)
    BOOST_PP_SEQ_FOR_EACH(WRAP_PROP, I3RecoPulse, PROPS)
    #undef PROPS
    .def(dataclass_suite<I3RecoPulse>())
    ;
  
  enum_<I3RecoPulse::PulseFlags>("PulseFlags")
    .value("LC", I3RecoPulse::LC)
    .value("ATWD", I3RecoPulse::ATWD)
    .value("FADC", I3RecoPulse::FADC)
    ;
}
