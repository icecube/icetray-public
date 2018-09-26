#include "qstringlist_converter.h"
#include <QStringList>

namespace py = boost::python;

PyObject* QStringListConverter::convert( QStringList const& sl ){
    py::list lst;
    for( int i = 0; i < sl.size(); ++i )
        lst.append( sl[i].toStdString() );
    return py::incref( lst.ptr() );
}

void* QStringListConverter::convertible( PyObject* obj_ptr ){
    if( !PySequence_Check( obj_ptr) )
        return 0;

    py::object obj( py::handle<>( py::borrowed(obj_ptr) ) );

    for( int i = 0, n = py::len(obj); i < n; ++i ){
        py::extract<const char*> get_cstr( obj[i] );
        if( !get_cstr.check() )
            return 0;
    }

    return obj_ptr;
}

void QStringListConverter::construct( PyObject* obj_ptr,
                                      boost::python::converter::rvalue_from_python_stage1_data* data )
{
    py::object obj( py::handle<>( py::borrowed(obj_ptr) ) );
    void* storage = ((py::converter::rvalue_from_python_storage<QStringList>*)data)->storage.bytes;
    new (storage) QStringList();
    data->convertible = storage;
    QStringList& sl = *static_cast<QStringList*>(storage);
    for( int i = 0, n = py::len(obj); i < n; ++i ){
        const char* s = py::extract<const char*>( obj[i] );
        sl.push_back( s );
    }
}

QStringListConverter::QStringListConverter(){
    boost::python::to_python_converter<QStringList, QStringListConverter>();
    boost::python::converter::registry::push_back( &convertible, &construct,
                                                   boost::python::type_id<QStringList>() );
}
