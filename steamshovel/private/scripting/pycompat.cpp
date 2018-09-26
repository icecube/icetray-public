#include <boost/version.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include "shovelart/SceneObject.h"
#include "gil.h"
#include <icetray/I3Logging.h>

#include "pycompat.h"

// This file addresses an issue that only arises on Python 2.7.3 and above
#if PY_VERSION_HEX >= 0x02070300

/*
 * Here be dragons.
 *
 * For shared_ptr's that hold a class which derives from boost::python::wrapper, python is invoked when
 * the shared_ptr's reference count goes to zero and the object is destructed.  This presents a
 * problem for code that wants to safely work with, e.g., ArtistPtrs, but doesn't want to be
 * responsible holding the GIL in the possible event that their destructors run.
 *
 * The following workaround puts a GIL around the destruction function that runs within the
 * boost::shared_ptr implementation if the pointer is carrying a shared_ptr_deleter from boost::python.
 * This is not pretty, but it seems to work correctly in boost 1.38 and 1.53.
 */

// In boost 1.49 and 1.53, this void* specialization is invoked for all cases, but in boost 1.38,
// type-specific specializations below are invoked.  I don't know the exact point at which this
// behavior changed, but I think 1.38 is the only outdated version likely to appear in the icecube world.
// The type-specific specializations are brittle,
// and probably shouldn't be used, so emit a warning for ancient boost users in that situation.
// Most users of boost 1.38 won't hit the warning, of course, because their python will be < 2.7.3

#if BOOST_VERSION >= 103800

#include "shovelart/Artist.h"
#include "shovelart/SceneVariant.h"

namespace boost { namespace detail {

// fun fact: ptr is null, the actual work of
// releasing resources is done by del!
#define SPECIALIZE( r, _, T ) \
template<> \
void sp_counted_impl_pd<T*, shared_ptr_deleter>::dispose(){ \
	scripting::ScopedGIL gil; \
	log_trace( "Locking the GIL and killing a %s (" BOOST_PP_STRINGIZE(T) ") at %p", \
	    python::extract<const char*>( \
        	python::object(del.owner).attr("__class__").attr("__name__") \
    	)(), del.owner.get() \
	); \
	del( ptr ); \
} \

BOOST_PP_SEQ_FOR_EACH( SPECIALIZE, _, SHOVELART_GIL_PROTECTED_TYPES )

}} // namespace boost::detail

#endif // BOOST_VERSION

#endif // PY_VERSION_HEX >= 0x0207030000

// For older versions of python, this file intentionally left blank
