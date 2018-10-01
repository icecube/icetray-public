#include <I3/name_of.h>
#include <icetray/I3Parameter.h>

#include <icetray/serialization.h>
#include <icetray/Utility.h>

#include <serialization/optional.hpp>
#include <boost/python.hpp>
#include <boost/python/extract.hpp>

#include <icetray/serialization.h>
#include <icetray/impl.h>
#include <boost/numeric/conversion/cast.hpp>

#include <boost/mpl/and.hpp>
#include <boost/mpl/logical.hpp>

#include <boost/python.hpp>

using namespace boost;
using namespace std;
namespace bp = boost::python;

//
//   This file contains everything that doesn't need to be instantiated on a per-type basis
//
I3Parameter::~I3Parameter() 
{ }

I3Parameter::I3Parameter() 
  : got_by_module_(false) 
{ }

I3Parameter::I3Parameter(const I3Parameter& rhs)
{
  *this = rhs;
}

I3Parameter& 
I3Parameter::operator=(const I3Parameter& rhs)
{
  name_ = rhs.name_;
  description_ = rhs.description_;
  got_by_module_ = rhs.got_by_module_;

  if (rhs.default_)
    default_ = rhs.default_;
  if (rhs.configured_)
    configured_ = rhs.configured_;
  return *this;
}

string 
I3Parameter::default_value_str() const 
{ 
  if (default_)
    return repr(*default_);
  else
    return "(no default value)";
}

string 
I3Parameter::configured_value_str() const 
{ 
  if (configured_)
    return repr(*configured_);
  else
    return "(no configured value)";
}

void 
I3Parameter::set_configured(const boost::python::object& t)
{
  configured_ = t;
}

void 
I3Parameter::set_default(const boost::python::object& t)
{
  default_ = t;
}

boost::python::object
I3Parameter::value() const
{
  got_by_module_ = true;

  if (configured_)
    return *configured_;
  else if (default_)
    return *default_;
  else
    {
      log_fatal("this parameter (%s) has neither default nor configured value!",
		name_.c_str());
      return boost::python::object(); //warning stopper
    }
}

template <typename Archive>
static void
save_python(Archive &ar, boost::optional<boost::python::object>& obj)
{
  bp::object main = bp::import("__main__");
  bp::object global = main.attr("__dict__");

  if (typename Archive::is_saving())
    {
      std::string s;
      if(obj)
	s = bp::extract<std::string>(obj->attr("__repr__")());
      else
	s = "None";
      log_trace("saved param as '%s'", s.c_str());
      ar & make_nvp("value", s);
    }
  else
    {
      std::string s;
      ar & make_nvp("value", s);
      log_trace("about to eval '%s'", s.c_str());
      try {
	bp::object evalled = bp::eval(bp::str(s), global, global);
	obj = boost::optional<boost::python::object>(evalled);
      } catch (const bp::error_already_set& e){
	obj = boost::optional<boost::python::object>(bp::object(s));
      }
    }

}

template <typename Archive>
void
I3Parameter::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("name", name_);
  ar & make_nvp("description", description_);
  save_python(ar, default_);
  save_python(ar, configured_);
}

I3_BASIC_SERIALIZABLE(I3Parameter);

ostream& operator<<(ostream& os, const I3Parameter& pb)
{
  os << "  " << pb.name() << "\n"
     << "    Description :  " << pb.description() << "\n"
     << "    Default     :  " << pb.default_value_str() << "\n";
  if (pb.has_configured())
    os << "    Configured  :  " << pb.configured_value_str() << "\n";
  return os;
}
