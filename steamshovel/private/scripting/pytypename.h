// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef I3_SHOVEL_SCRIPTING_PYTYPENAME_H
#define I3_SHOVEL_SCRIPTING_PYTYPENAME_H

#include <boost/python.hpp>
#include <string>

namespace scripting{

    /**
     * Given an class object referring to an I3FrameObject,
     * return the typename of the wrapped C++ class.
     */
    std::string type_name( boost::python::object pyclass );

}

#endif
