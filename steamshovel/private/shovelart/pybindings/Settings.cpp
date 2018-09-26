#include "shovelart.h"
#include "shovelart/BindSignal.h"
#include "shovelart/Artist.h"
#include "shovelart/ColorVariant.h"
#include "shovelart/Scenario.h"
#include "shovelart/OMKeySet.h"
#include "shovelart/RangeSetting.h"
#include "shovelart/ChoiceSetting.h"
#include "shovelart/KeySetting.h"
#include "shovelart/FileSetting.h"

#include <QColor>
#include <QFont>
#include <QString>
#include <string>
#include <QApplication>
#include <QThread>

#include <boost/bind.hpp>
#include <boost/preprocessor.hpp>
#include <boost/python.hpp>

// Bring on the preprocessor metaprogramming. 
// See documentation for boost/preprocessor.hpp for guidance.

// list of POD types, except bool, which is treated separately
#define PODTYPES (int)(QColor)(ColorMapPtr)(QFont)(OMKeySet)(QString)
// list of complex types that need special handling
#define COMPLEXTYPES (RangeSetting)(ChoiceSetting)(KeySetting)(FileSetting)

namespace scripting { namespace shovelart {

namespace bp = boost::python;

/**
 * Helper function: Run the given function within the thread that owns the invokee
 */
void threadSafeQInvoke( const boost::function<void(void)>& c, QObject* invokee ){
	if( invokee->thread() ==  QThread::currentThread() ){
		log_trace( "threadSafeQInvoke direct connection" );
		c();
	}
	else{
		log_trace( "threadSafeQInvoke queued connection" );
		Py_BEGIN_ALLOW_THREADS
		/* Create a proxy QObject, push it to the main thread,
		   and invoke the operation thereupon. We would use
		   QObject itself if it wasn't abstract; BindSignal is
		   a conveniently lightweight child class.
		*/
		BindSignal* bs = new BindSignal( c );
		bs->moveToThread( QApplication::instance()->thread() );
		bool success = QMetaObject::invokeMethod( bs, "call", Qt::BlockingQueuedConnection );
		if( !success ){
			log_error( "Error performing a queued Qt invocation" );
		}
		delete bs;
		Py_END_ALLOW_THREADS
	}
}

/**
 * Create a python object representing the current value of artist.setting(key).
 * If the setting holds a complex type like RangeSetting and evaluate == true,
 * the return value will be the current value of the Setting; if false, the
 * return value will be the Setting object itself.
 */
bp::object settingValueToPython( const Artist& artist, const std::string& key, bool evaluate ){

	const Artist::Setting& s = artist.settings()[ key ];

	#define SUPPORTED_PODTYPE( r, _, T )       \
	    if( s.is<T>() ){                       \
	        return bp::object( s.value<T>() ); \
	    }

	BOOST_PP_SEQ_FOR_EACH( SUPPORTED_PODTYPE, _, (bool) PODTYPES );
	#undef SUPPORTED_PODTYPE

	if( evaluate ){
		// return the value that the complex setting encapsulates
		#define SUPPORTED_COMPLEXTYPE( r, _, T )              \
		    if( s.is<T>() ){                                  \
		        return bp::object( s.valueFromComplex<T>() ); \
		    }

		BOOST_PP_SEQ_FOR_EACH( SUPPORTED_COMPLEXTYPE, _, COMPLEXTYPES );
		#undef SUPPORTED_COMPLEXTYPE
	}
	else{
		// return the object itself
		#define SUPPORTED_COMPLEXTYPE( r, _, T )   \
		    if( s.is<T>() ){                       \
		        return bp::object( s.value<T>() ); \
		    }

		BOOST_PP_SEQ_FOR_EACH( SUPPORTED_COMPLEXTYPE, _, COMPLEXTYPES );
		#undef SUPPORTED_COMPLEXTYPE
	}

	log_error_stream( "Returning None for setting of unknown type, key=" << key );
	return bp::object();
}

/**
 * Create a new setting object for the given artist from the given python object.
 * The type depends on the type of the given object.  NULL is returned if no appropriate type is found.
 */
Artist::Setting newSettingFromPython( Artist* a, bp::object value ){

	// extract<> will silently convert bools to ints and vice-versa,
	// so when checking for bool type use the Python C API to confirm the type
	if( PyBool_Check(value.ptr()) ){
		bp::extract<bool> boolval(value);
		return Artist::Setting( a, boolval() );
	}

	// now proceed with all other types
	#define SUPPORTED_TYPE( r, _, T ){            \
	    bp::extract<T> get_T(value);              \
	    if( get_T.check())                        \
	        return Artist::Setting( a, get_T() ); \
	}

	// all except bool
	BOOST_PP_SEQ_FOR_EACH( SUPPORTED_TYPE, _, PODTYPES COMPLEXTYPES );
	#undef SUPPORTED_TYPE

 	// if we arrive here, return an empty setting
	return Artist::Setting( a );
}

/**
 * Change a.setting(key) to a new python value.  Value type must match the the existing setting type.
 */
void changeSettingFromPython( boost::shared_ptr<Scenario> scenario,
                              ArtistPtr a,
                              const std::string& key,
                              bp::object value )
{

	const Artist::Setting& setting = a->settings()[ key ];

	// Treat bool specially, as in newSettingFromPython
	if( setting.is<bool>() && PyBool_Check(value.ptr()) ){
		const bool t = bp::extract<bool>(value);
		threadSafeQInvoke( boost::bind(&Scenario::changeSetting<bool>,
		                   scenario, a, key, t), scenario.get() );
		return;
	}

	#define SUPPORTED_TYPE( r, _, T )                   \
	if( setting.is<T>() ){                              \
	    bp::extract<T> get_T(value);                    \
	    if( get_T.check() ){                            \
	        T t = get_T();                              \
	        threadSafeQInvoke( boost::bind(&Scenario::changeSetting<T>, \
	                           scenario, a, key, t), scenario.get() );  \
	        return;                                     \
	    }                                               \
	}

	BOOST_PP_SEQ_FOR_EACH( SUPPORTED_TYPE, _, PODTYPES );
	#undef SUPPORTED_TYPE

	#define SUPPORTED_COMPLEXTYPE( T )                  \
	if( setting.is<T>() ){                              \
	    bp::extract<T::Value> get_V(value);             \
	    if( get_V.check() ){                            \
	        T::Value v = get_V();                       \
	        threadSafeQInvoke( boost::bind(&Scenario::changeComplexSetting<T>, \
	                           scenario, a, key, v), scenario.get() );         \
	        return;                                     \
	    }                                               \
	}

	SUPPORTED_COMPLEXTYPE( RangeSetting );
	SUPPORTED_COMPLEXTYPE( ChoiceSetting );
	SUPPORTED_COMPLEXTYPE( KeySetting );
	SUPPORTED_COMPLEXTYPE( FileSetting );
	#undef SUPPORTED_COMPLEXTYPE

	throw std::runtime_error( "Cannot set this setting using this type" );
}

}} //namespace scripting
