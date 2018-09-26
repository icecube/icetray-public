#ifndef I3_SHOVEL_SCRIPTING_VEC3D_CONVERSION_H
#define I3_SHOVEL_SCRIPTING_VEC3D_CONVERSION_H

#include <boost/python.hpp>

struct AnythingToVec3d{
    static void* convertible( PyObject* o );
    static void construct( PyObject* o,
                           boost::python::converter::rvalue_from_python_stage1_data* data );
    AnythingToVec3d(); // run constructor to register the conversion
};

#endif
