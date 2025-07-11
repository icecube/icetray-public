// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#include "vec3d_converter.h"
#include "shovelart/graphicsmath.h"

AnythingToVec3d::AnythingToVec3d(){
    boost::python::converter::registry::push_back(
        &convertible, &construct, boost::python::type_id<vec3d>()
    );
}

void* AnythingToVec3d::convertible( PyObject* o ){
    namespace bp = boost::python;
    bp::object obj( bp::handle<>(bp::borrowed(o)) );

    // check whether o is a suitable sequence
    if( PySequence_Size(o) >= 3 ){
        for( int i = 0; i < 3; ++i ){
            bp::extract<double> const n(obj[i]);
            if( !n.check() )
                return nullptr;
        }
        return o;
    }

    // check whether o has suitable attributes
    static constexpr std::array<const char *, 3> attrs {"x", "y", "z"};
    for(auto & attr : attrs){
#if PY_VERSION_HEX >= 0x030d0000  // python 3.13
        if( PyObject_HasAttrStringWithError(o, attr) != 1)
            return nullptr;
#else
        if( !PyObject_HasAttrString(o, attr) )
            return nullptr;
#endif
        bp::extract<double> const n(obj.attr(attr));
        if( !n.check() )
            return nullptr;
    }
    return o;
}

void AnythingToVec3d::construct( PyObject* o,
                                 boost::python::converter::rvalue_from_python_stage1_data* data ){
    namespace bp = boost::python;
    bp::object obj( bp::handle<>(bp::borrowed(o)) );
    void* storage = ((bp::converter::rvalue_from_python_storage<vec3d>*)data)->storage.bytes;

    if( PySequence_Size(o) >= 3 ){
        // beware: strange bug in boost.python if you do
        // bp::extract<double> x(obj[0]);
        // bp::extract<double> y(obj[1]);
        // bp::extract<double> z(obj[2]);
        // new ( storage ) vec3d( x(), y(), z() ); // all numbers are the same!
        const double x = bp::extract<double>(obj[0]);
        const double y = bp::extract<double>(obj[1]);
        const double z = bp::extract<double>(obj[2]);
        new ( storage ) vec3d( x, y, z );
        data->convertible = storage;
        return;
    }

    // if we arrive here, then o must suitable xyz attributes
    const double x = bp::extract<double>(obj.attr("x"));
    const double y = bp::extract<double>(obj.attr("y"));
    const double z = bp::extract<double>(obj.attr("z"));
    new ( storage ) vec3d( x, y, z );
    log_info_stream("xyz " << x << " " << y << " " << z);
    data->convertible = storage;
}
