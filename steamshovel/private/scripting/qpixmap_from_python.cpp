#include "qpixmap_from_python.h"
#include <boost/python.hpp>
#include <icetray/I3Logging.h>
#include <stdexcept>
#include <QImage>

QPixmap scripting::qpixmap_from_python( PyObject* pyobj, int w, int h ){
	if( !PyObject_CheckBuffer(pyobj) ){
		PyErr_SetString( PyExc_RuntimeError,
          "argument does not support buffer interface" );
		boost::python::throw_error_already_set();
	}

	Py_buffer view;

	if( PyObject_GetBuffer( pyobj, &view, PyBUF_SIMPLE ) != 0 ){
		PyErr_SetString( PyExc_RuntimeError,
		  "could not get buffer" );
		boost::python::throw_error_already_set();
	}

	if( view.len != w * h * 4 ){
		PyBuffer_Release(&view);
		PyErr_SetString( PyExc_RuntimeError,
		   "buffer has wrong size" );
		boost::python::throw_error_already_set();
	}

	// I'm not sure why QImage's format ARGB32 is actually reversed.
	// Transform from rgba (Agg) to bgra (QImage).
	unsigned char* data = (unsigned char*)(view.buf);
	for( int ipix = 0, n = h * w; ipix < n; ++ipix ){
		unsigned char r = data[4 * ipix + 0];
		unsigned char b = data[4 * ipix + 2];
		data[4 * ipix + 0] = b;
		data[4 * ipix + 2] = r;
	}

	QImage ret( data, w, h, QImage::Format_ARGB32 );

	PyBuffer_Release(&view);
	return QPixmap::fromImage(ret);
}
