#include "Artist.h"
#include "scripting/gil.h"

#include <boost/python.hpp>

bool Artist::isValidKey( I3FramePtr ptr, int key_idx, const std::string& key ){
    return requiredTypes().at(key_idx) == ptr->type_name(key);
}

/**
 * Creates a python artist and sets its description.
 * bp::error_already_set may arise for various reasons.
 */
ArtistPtr Artist::pymake( const boost::python::object& artist_type,
                          const ArtistKeyList& keys ){
    scripting::ScopedGIL gil; // DO NOT REMOVE! Crashes follow!
    namespace bp = boost::python;
    ArtistPtr ret;
    bp::object artist_obj = artist_type();
    ret = bp::extract<ArtistPtr>(artist_obj);
    ret->keys_ = keys;
    const char* name = bp::extract<const char*>(artist_type.attr("__name__"));
    log_trace_stream("PyArtist " << name << " constructed");
    return ret;
}
