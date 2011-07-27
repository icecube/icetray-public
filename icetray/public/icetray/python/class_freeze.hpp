#ifndef CLASS_FREEZE_HPP_INCLUDED
#define CLASS_FREEZE_HPP_INCLUDED

#include <boost/python.hpp>

//
// Disable the dynamism of python classes. This will make it
// easier to catch simple scripting errors due to changes in the
// attribute names
//

using boost::python::object;
void setattr_with_dynamism_disabled( const object& obj, 
				     const object& name, 
				     const object& value);

#endif
