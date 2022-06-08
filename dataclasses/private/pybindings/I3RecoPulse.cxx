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
static PyBufferProcs rpsm_bufferprocs;

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
	view->len = 3*ts.size()*sizeof(double);
	view->itemsize = sizeof(double);
	if (flags & PyBUF_FORMAT)
		view->format = (char *)"d";
	else
		view->format = NULL;
	view->ndim = 2;

	view->shape = new Py_ssize_t[2];
	view->shape[0] = ts.size();
	view->shape[1] = 3;

	// To honor a contiguous buffer request, make a copy of the
	// data. This violates the spirit of the buffer protocol
	// slightly, but both simplifies the API and allows a
	// faster memory copy than iterating over the list in Python
	// would.

	view->buf = malloc(view->len);
	view->readonly = 1;

	view->strides = new Py_ssize_t[2];
	view->strides[0] = 3*view->itemsize;
	view->strides[1] = view->itemsize;

	int j = 0;
	for (auto i : ts) {
		((double *)view->buf)[3*j + 0] = i.GetTime();
		((double *)view->buf)[3*j + 1] = i.GetCharge();
		((double *)view->buf)[3*j + 2] = i.GetWidth();
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

static int
I3RecoPulseSeriesMap_getbuffer(PyObject *obj, Py_buffer *view, int flags)
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
	I3RecoPulseSeriesMap &ts = extract<I3RecoPulseSeriesMap &>(selfobj)();
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

	view->len = 0;
	for (auto i : ts)
		view->len += 6*i.second.size()*sizeof(double);

	view->obj = obj;
	view->itemsize = sizeof(double);
	if (flags & PyBUF_FORMAT)
		view->format = (char *)"d";
	else
		view->format = NULL;
	view->ndim = 2;

	view->shape = new Py_ssize_t[2];
	view->shape[1] = 6;
	view->shape[0] = view->len/view->shape[1]/sizeof(double);

	// To honor a contiguous buffer request, make a copy of the
	// data. This violates the spirit of the buffer protocol
	// slightly, but both simplifies the API and allows a
	// faster memory copy than iterating over the list in Python
	// would.

	view->buf = malloc(view->len);
	view->readonly = 1;

	view->strides = new Py_ssize_t[2];
	view->strides[0] = 6*view->itemsize;
	view->strides[1] = view->itemsize;

	int j = 0;
	for (auto i : ts) {
		for (auto k : i.second) {
			((double *)view->buf)[6*j + 0] = i.first.GetString();
			((double *)view->buf)[6*j + 1] = i.first.GetOM();
			((double *)view->buf)[6*j + 2] = i.first.GetPMT();
			((double *)view->buf)[6*j + 3] = k.GetTime();
			((double *)view->buf)[6*j + 4] = k.GetCharge();
			((double *)view->buf)[6*j + 5] = k.GetWidth();
			j++;
		}
	}

	view->suboffsets = NULL;
	view->internal = view->buf;

        Py_INCREF(obj);

        return 0;
}

static list
I3RecoPulseSeriesMap_pmtoffsets(const I3RecoPulseSeriesMap &rpsm)
{
	list out;

	size_t j = 0;
	for (auto i : rpsm) {
		out.append(j);
		j += i.second.size();
	}

	return out;
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

  object rpsm = class_<I3RecoPulseSeriesMap, bases<I3FrameObject>, I3RecoPulseSeriesMapPtr>("I3RecoPulseSeriesMap")
    .def(dataclass_suite<I3RecoPulseSeriesMap>())
    .def("from_frame", &from_frame, args("frame", "key"),
        "Get an I3RecoPulseSeriesMap from the frame, performing any necessary "
        "format conversions behind the scenes.")
    .staticmethod("from_frame")
    .def("pmt_array_offsets", &I3RecoPulseSeriesMap_pmtoffsets,
        "Provide a list of offsets into a numpy.asarray()-ed "
        "I3RecoPulseSeriesMap corresponding to the beginning of the pulses for "
        "each OMKey. The format of the numpy.asarray() version of an "
        "I3RecoPulseSeriesMap is one row per pulse, PMTs grouped together, "
        "with columns (String, OM, PMT, Time, Charge, Width).")
    ;
  register_pointer_conversions<I3RecoPulseSeriesMap>();

  // Add buffer protocol interface
  PyTypeObject *rpsmclass = (PyTypeObject *)rpsm.ptr();
  rpsm_bufferprocs.bf_getbuffer = I3RecoPulseSeriesMap_getbuffer;
  rpsm_bufferprocs.bf_releasebuffer = I3RecoPulseSeries_relbuffer;
  rpsmclass->tp_as_buffer = &rpsm_bufferprocs;

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
