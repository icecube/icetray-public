#ifndef ICETRAY_IMPL_H_INCLUDED
#define ICETRAY_IMPL_H_INCLUDED

#include <boost/python.hpp>
#include <icetray/I3Logging.h>

#if defined(I3_FRAMEWORK_LOGGING)
#define i3_log(format, ...) log_trace("%s: " format, this->GetName().c_str(), ##__VA_ARGS__)
#else
#define i3_log(format, ...) 
#endif

inline std::string repr(const boost::python::object& obj)
{
  return boost::python::extract<std::string>(obj.attr("__repr__")());
}
#endif
