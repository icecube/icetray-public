#ifndef I3_SHOVEL_SCRIPTING_QSTRING_CONVERSION_H
#define I3_SHOVEL_SCRIPTING_QSTRING_CONVERSION_H

#include <boost/python.hpp>
class QString;

struct QStringConverter{
    // QString to PyString
    static PyObject* convert( QString const& s );

    // PyString to QString: checking
    static void* convertible( PyObject* obj_ptr );

    // PyString to QString: conversion
    static void construct( PyObject* obj_ptr,
                           boost::python::converter::rvalue_from_python_stage1_data* data );

    QStringConverter(); // run constructor to register the conversion
};

#endif
