#include "pytypename.h"
#include "pyerr.h"

#include <icetray/I3FrameObject.h>
#include <icetray/I3Logging.h>
#include <icetray/name_of.h>
#include <boost/python.hpp>

namespace scripting{

    std::string type_name( boost::python::object pyclass ){
        using namespace boost::python;

        object obj;
        try{
            // To get the type_info out of a python type object (such as dataclasses.I3Geometry),
            // we need to create an instance. Object creation will fail for I3FrameObjects without
            // a default constructor.
            obj = pyclass();
        }
        catch( error_already_set& e ){
            log_error_stream( "Python exception making default frame object:\n" << e );
        }

        const I3FrameObject* fobj = extract<const I3FrameObject*>( obj );
        if( !fobj ){
            throw std::runtime_error( "Could not extract I3FrameObject*" );
        }

        return icetray::name_of( typeid( *fobj ) );
    }

}
