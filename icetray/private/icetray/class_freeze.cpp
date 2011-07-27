#include <icetray/python/class_freeze.hpp>
#include <sstream>

//
// Disable the dynamism of python classes. This will make it
// easier to catch simple scripting errors due to changes in the
// attribute names
//
void setattr_with_dynamism_disabled( const boost::python::object& obj, 
				     const boost::python::object& name, 
				     const boost::python::object& value){
  if( PyObject_HasAttr(obj.ptr(), name.ptr()) ){
    PyObject_GenericSetAttr( obj.ptr(), name.ptr(), value.ptr());
  }else{
    std::stringstream ss;
    std::string clname = boost::python::extract<std::string>( obj.attr("__class__").attr("__name__") );
    std::string attname = boost::python::extract<std::string>( name );
    ss<<"*** The dynamism of this class has been disabled"<<std::endl;
    ss<<"*** Attribute ("<<attname<<") does not exist in class "<<clname<<std::endl; 
    throw std::runtime_error( ss.str().c_str() );
  }
}
