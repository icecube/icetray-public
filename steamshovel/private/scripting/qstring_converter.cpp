#include "qstring_converter.h"
#include <QString>

namespace py = boost::python;

PyObject* QStringConverter::convert( QString const& s ){
    return py::incref( py::str( s.toStdString() ).ptr() );
}

void* QStringConverter::convertible( PyObject* obj_ptr ){
    py::object obj( py::handle<>( py::borrowed(obj_ptr) ) );
    py::extract<const char*> get_cstr( obj );
    return get_cstr.check() ? obj_ptr : 0;
}

void QStringConverter::construct( PyObject* obj_ptr,
                                  boost::python::converter::rvalue_from_python_stage1_data* data )
{
    py::object obj( py::handle<>( py::borrowed(obj_ptr) ) );
    py::extract<const char*> get_cstr( obj );
    void* storage = ((boost::python::converter::rvalue_from_python_storage<QString>*)data)->storage.bytes;
    const char* s = get_cstr();
    new (storage) QString(s);
    data->convertible = storage;
}

QStringConverter::QStringConverter(){
    boost::python::to_python_converter<QString, QStringConverter>();
    boost::python::converter::registry::push_back( &convertible, &construct, boost::python::type_id<QString>() );
}
