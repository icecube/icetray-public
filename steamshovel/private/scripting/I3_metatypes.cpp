#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor.hpp>
#include <boost/python.hpp>

#include <QMetaProperty>

#include <icetray/I3Logging.h>
#include <icetray/I3Frame.h>
#include <icetray/I3FrameObject.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/physics/I3RecoPulse.h>

#include "I3_metatypes.h"

#define TYPES (I3Geometry)(I3Frame)(I3FrameObject)(I3RecoPulseSeriesMap)

// Q_DECLARE_METATYPE must appear at global scope
#define DECL(r, P, T) Q_DECLARE_METATYPE( BOOST_PP_CAT(T, P) );

BOOST_PP_SEQ_FOR_EACH( DECL, Ptr, TYPES );
BOOST_PP_SEQ_FOR_EACH( DECL, ConstPtr, TYPES );

#undef DECL

namespace scripting{

#ifdef __GNUC__
  __attribute__((unused))
#endif
SET_LOGGER("qmeta");

void declare_i3_qt_metatypes(){
#define REGISTER(r, P, T) \
	qRegisterMetaType< BOOST_PP_CAT(T,P) >(); \
	log_trace( "MetaType registered " BOOST_PP_STRINGIZE( BOOST_PP_CAT(T,P) ) );
BOOST_PP_SEQ_FOR_EACH( REGISTER, Ptr, TYPES );
BOOST_PP_SEQ_FOR_EACH( REGISTER, ConstPtr, TYPES );
#undef REGISTER
}

QGenericArgument make_i3_qarg( const boost::python::object& pobj, const QByteArray& typestr,
                               std::vector<boost::any>& refs )
{
	namespace bp = boost::python;
// see qmeta_args.cpp, make_qarg(), for the intended behavior of this function

#define SUPPORTED_ARG( ARG ) \
	if( typestr == BOOST_PP_STRINGIZE(ARG) ) \
	{ \
		ARG tmp = boost::python::extract<ARG>(pobj); \
		boost::shared_ptr< ARG > ptr( new ARG(tmp) ); \
		refs.push_back( ptr ); \
		QGenericArgument ret = Q_ARG( ARG, (*ptr) ); \
		return ret; \
	}

#define I3_ARG( r, P, T ) SUPPORTED_ARG( BOOST_PP_CAT( T, P ) )

	BOOST_PP_SEQ_FOR_EACH( I3_ARG, ConstPtr, TYPES );

#undef I3_ARG

	return QGenericArgument();
}

#undef TYPES

} // namespace scripting
