#ifndef I3_SHOVEL_SCRIPTING_QSTRINGLIST_CONVERSION_H
#define I3_SHOVEL_SCRIPTING_QSTRINGLIST_CONVERSION_H

#include <boost/python.hpp>
class QStringList;

struct QStringListConverter{
    // QStringList to list of PyString
    static PyObject* convert( QStringList const& s );

    // list of PyString to QStringList: checking
    static void* convertible( PyObject* obj_ptr );

    // list of PyString to QStringList: conversion
    static void construct( PyObject* obj_ptr,
                           boost::python::converter::rvalue_from_python_stage1_data* data );

    QStringListConverter(); // run constructor to register the conversion
};

#endif
