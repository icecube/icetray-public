#ifndef I3_SHOVEL_SCRIPTING_DATA_H
#define I3_SHOVEL_SCRIPTING_DATA_H

#include <boost/python.hpp>
#include <icetray/I3Logging.h>
#include "gil.h"
#include "pyerr.h"

namespace scripting{

namespace bp = boost::python;

template <typename T>
void addDataToNamespace( T data, const std::string& name, std::string space = "__main__" ){
	ScopedGIL gil;
	try{
		bp::object mod = bp::import(space.c_str());
		mod.attr(name.c_str()) = data;
	}
	catch( bp::error_already_set& e ){
		log_error_stream("Could not set data into " << space << "." << name
                         << "\n" << e);
	}
}

} // namespace scripting

#endif /* I3_SHOVEL_SCRIPTING_DATA_H */
