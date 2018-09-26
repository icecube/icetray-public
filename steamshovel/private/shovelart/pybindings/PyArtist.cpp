#include <iostream>

#include <QFont>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <icetray/I3Logging.h>
#include <icetray/I3FrameObject.h>
#include <icetray/serialization.h>

#include "shovelart/Artist.h"
#include "shovelart/Scene.h"
#include "shovelart/Scenario.h"
#include "scripting/gil.h"
#include "scripting/pyerr.h"
#include "scripting/pytypename.h"
#include "shovelart.h"

// has to be last, because it overwrites a template specialization
#include "scripting/pycompat.h"

namespace scripting{ namespace shovelart{

namespace bp = boost::python;

#ifdef __GNUC__
  __attribute__((unused))
#endif
SET_LOGGER( "PyArtist" );

/** Quasi-abstract base class for Artists written in Python
 *
 */
class PyArtist : public GLArtist, public bp::wrapper< Artist >
{

private:
	std::vector< bp::object > cleanups_;

	/// Get the python object equivalent to a 'self' method parameter
	bp::object getSelf(){
		bp::reference_existing_object::apply<PyArtist*>::type convert;
		return bp::object( bp::handle<>( convert(this) ) );
	}

	/// Return true if obj has an attr with the given name
	bool hasAttr( bp::object obj, const std::string& name ){
		return PyObject_HasAttrString(obj.ptr(), name.c_str() );
	}

protected:
	/// Run cleanup actions; to be called from Scenario
	virtual void cleanUp( ){
		log_debug_stream( "Removing python artist with " << cleanups_.size() << " cleanup actions" );
		ScopedGIL gil;
		try{
			for( std::vector<bp::object>::reverse_iterator i = cleanups_.rbegin();
			     i != cleanups_.rend(); ++i )
				(*i)();
		}
		catch( bp::error_already_set& e ){
			log_error_stream( "Python exception in PyArtist::cleanupOnDelete\n" << e );
		}
		cleanups_.clear();
	}

public:
	virtual ~PyArtist(){
		log_trace("Deleting a PyArtist");
	}

	void defineSettings( bp::object settings ){
		bp::extract<bp::tuple> get_tuple( settings );
		if( get_tuple.check() ){
			bp::tuple tp = get_tuple();
			const int n = len(tp) / 2;
			for( int i = 0; i < n; ++i ){
				bp::str name = bp::extract<bp::str>( tp[2 * i + 0] );
				bp::object value = tp[2 * i + 1];
				addSetting( name, value );
			}
		} else {
			// we assume that this is a dict (old style call)
			bp::dict dt = bp::extract<bp::dict>( settings );
			bp::list keys = dt.keys();			
			for( unsigned i = 0; i < bp::len(keys); ++i ){
				bp::object key = keys[i];
				bp::object value = dt[key];
				bp::str name = bp::extract<bp::str>( key );
				addSetting( name, value );
			}
		}
	}

	void addSetting( bp::str pyname, bp::object pyvalue ){
		std::string name = bp::extract< std::string >( pyname );
		Artist::Setting setting = newSettingFromPython( this, pyvalue );
		if( setting.empty() )
			log_error_stream ("Ignoring setting of unknown type, key=" << name );
		else
			Artist::addSetting( name, setting );
	}

	bp::object getSetting( const std::string& key ){
		return settingValueToPython( *this, key );
	}

	virtual std::string description() {
		ScopedGIL gil;
		bp::object cls = getSelf().attr("__class__");
		return bp::extract<std::string>(cls.attr("__name__"));
	}

	virtual typelist requiredTypes(){
		typelist ret;
		log_trace("requiredTypes");
		ScopedGIL gil;
		try{
			bp::object self = getSelf();
			if( hasAttr( self, "requiredTypes" )){
				bp::object tlist = self.attr("requiredTypes");
				for( int i = 0, n = bp::len(tlist); i < n; ++i ){
					bp::extract<std::string> get_str( tlist[i] );
					ret.push_back( get_str.check() ? get_str() : type_name( tlist[i] ) );
				}
			}
		}
		catch( bp::error_already_set& e ){
			log_error_stream( "Unexpected exception in PyArtist::requiredTypes\n" << e );
		}
		if( ret.size() != numRequiredKeys() ){
			log_trace( "Incorrect type list size, so will just return empty" );
			ret.clear();
		}
		return ret;
	}

	virtual unsigned numRequiredKeys(){
		log_trace("PyArtist::numRequiredKeys()");
		ScopedGIL gil;
		try{
			bp::object self = getSelf();
			if( hasAttr( self, "requiredTypes" ) )
				return bp::len( self.attr("requiredTypes") );
			else
				return bp::extract<unsigned>(self.attr("numRequiredKeys"));
		}
		catch( bp::error_already_set& e ){
			log_error_stream( "Python exception in PyArtist::numRequiredKeys\n" << e );
		}
		return 0;
	}

	/** C++ API: Call the python class's overridden create() method
	 *
	 * Because this has a different signature in C++ and in Python, and because it's never
	 * useful to call this from Python, we no longer expose this method as a bp::pure_virtual.
	 * Instead, pyAbstractCreate is exposed as the create() method.  It will signal an error
	 * if a python artist forgets to define its own create()
	 */
	virtual void create(I3FramePtr ptr, SceneGroup* group, const SceneState& state ){
		log_trace( "PyArtist::create" );
		ScopedGIL gil;
		try{
			PyArtistSceneGroup output(group, state);
			bp::object pycreate = this->get_override("create");
			if( pycreate ){
				pycreate(ptr, boost::ref(output));
				return;
			}
			else
				return pyAbstractCreate(ptr, output);
		}
		catch( bp::error_already_set& e ){
			log_error_stream( "Python exception in PyArtist::create\n" << e );
		}
	}

	virtual void pyAbstractCreate( I3FramePtr, PyArtistSceneGroup& output ){
		log_error_stream( "PyArtist " << description() << " does not define required method create()" );
	}

	virtual bool isValidKey( I3FramePtr ptr, int key_idx, const std::string& key ){
		log_trace("PyArtist::isValidKey");
		ScopedGIL gil;
		try{
			// Call the python isValidKey if it exists
			bp::override pyoverride = this->get_override("isValidKey");
			if( pyoverride ){
				return pyoverride(ptr,key_idx,key);
			}

			// otherwise work with the requiredTypes list
			return Artist::isValidKey( ptr, key_idx, key );
		}
		catch( bp::error_already_set& e ){
			log_error_stream( "Python exception in PyArtist::isValidKey:\n" << e );
		}
		return false;
	}

	void addCleanupAction( bp::object action ){
		cleanups_.push_back(action);
	}

};

SceneState::OverlayHints overlaySizeHints( ArtistPtr artist, const PyArtistSceneGroup& output ){
	log_debug( "Fetching PyArtist size hints" );
	const SceneState::OverlayHintsMap& hints = output.state_.overlayHintsMap_;
	SceneState::OverlayHintsMap::const_iterator found = hints.find( artist.get() );
	if( found != hints.end() ){
		const SceneState::OverlayHints& hints = found->second;
		log_debug_stream( "Found " << hints.size() << " OverlayHints for this artist");
		return hints;
	}
	else
		log_debug("No hints for this artist");
	return SceneState::OverlayHints();
}

double overlayScalingFactor( ArtistPtr, const PyArtistSceneGroup& output ){
	log_debug( "Fetching PyArtist scaling factor" );
	return output.state_.scaling_factor;
}

void export_pyartist_types(){

	bp::class_< PyArtist, boost::shared_ptr<PyArtist>, boost::noncopyable >( "PyArtist",
			"Python wrapper for Artist class.  Python-defined artists "
			"must subclass this type, and must define methods description() "
			"and create(). They must also either set numRequiredKeys "
			"and provide an isValidKey() method if more than zero "
			"key types are required, or else set requiredTypes to a list "
			"of I3FrameObject types")
		.def("isValidKey", &PyArtist::isValidKey,
		     "Return true if frame[key] is a valid key_idx'th input to this artist; "
		     "must be overridden in child classes unless a requiredTypes list "
		     "is given or numRequiredKeys is 0.",
		     ( bp::arg("self"), bp::arg("frame"), bp::arg("key_idx"), bp::arg("key") ) )
		.def("create", &PyArtist::pyAbstractCreate,
		     "[Abstract base class method, must be overridden.] "
		     "Generate SceneObjects for this artist. The values of "
		     "frame[self.keys()] are guaranteed to be valid "
		     "if this artist accepts keys.",
		     ( bp::arg( "self"), bp::arg("frame"), bp::arg("output") ) )
		.def("keys", &Artist::keys, bp::return_value_policy<bp::reference_existing_object>(),
		     "Return the currently set keys of this artist. When called "
		     "from within create(), the Artist may assume that the "
		     "returned keys are valid according to isValidKey()",
		     bp::arg("self") )
		.def("defineSettings", &PyArtist::defineSettings,
		     "Specify new settings as a dictionary mapping from "
		     "strings to values (loosing order), or as a tuple of "
		     "alternating keys and values. Should only be called from __init__.",
		     (bp::arg("self"), bp::arg("settings")) )
		.def("addSetting", &PyArtist::addSetting,
		     "Add a setting. Should only be called from __init__.",
		     (bp::arg("self"), bp::arg("name"), bp::arg("value")) )
		.def("setting", &PyArtist::getSetting,
		     "Return the value of the given setting. "
		     "If the setting is a RangeSetting, returns its current floating-point value.",
		     (bp::arg("self"), bp::arg("setting_key")) )
		.def("addCleanupAction", &PyArtist::addCleanupAction,
		     "Specify a callable object to be called when "
		     "this artist is deleted. Use in place of destructors "
		     "to release any resources acquired by the artist.",
		     (bp::arg("self"), bp::arg("action")) )
		.def("overlaySizeHints", &overlaySizeHints,
		     "Fetch size hints for any resizable overlays this "
		     "artist has generated. Used by matplotlib artists "
		     "to size their figures; most artists do not need to call this method.",
		     (bp::arg("self"), bp::arg("output")) )
		.def("overlayScalingFactor", &overlayScalingFactor,
		     "Get the current scaling factor for overlays, allowing "
		     "artists to make pixel density adjustments",
		     (bp::arg("self"), bp::arg("output")) )
		;

}

}} // namespace scripting::shovelart
