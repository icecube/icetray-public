#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/weak_ptr.hpp>

#include <icetray/I3Logging.h>

#include <QMetaObject>
#include <QThread>

#include "shovelart/Artist.h"
#include "shovelart/ArtistRegistry.h"
#include "shovelart/Scene.h"
#include "shovelart/SceneObject.h"
#include "shovelart/Scenario.h"
#include "shovelart/BindSignal.h"
#include "shovelart.h"

// has to be last, because it overwrites a template specialization
#include "scripting/pycompat.h"

namespace scripting { namespace shovelart {

namespace bp = boost::python;

#ifdef __GNUC__
  __attribute__((unused))
#endif
SET_LOGGER("ScenarioBridge");

/**
 * ArtistPtr, a boost::shared_ptr, is the canonical artist handle when
 * interacting with a Scenario in C++, but we don't want Python to be involved
 * with the lifetimes of Artists.  Moreover, ArtistPtr participates in the
 * PyArtist class, which is a bp::wrapper and suffers from wrapper's
 * type problems.  This weak handle type is therefore used for all direct
 * user interaction with Scenarios through python.
 */
typedef boost::weak_ptr<Artist> ArtistWeakHandle;

/** Attempt to convert a weak handle to a real ArtistPtr that can be used, or throw otherwise */
static ArtistPtr upgradeHandle( const ArtistWeakHandle& handle ){
	ArtistPtr p( handle.lock() );
	if( p ){
		return p;
	}
	else{
		throw std::runtime_error( "Attempt to use expired ArtistHandle" );
	}
}

/**
 * Python-to-C++ converter to turn ArtistWeakHandles, which are exposed to python,
 * into ArtistPtrs that can be used as keys to Scenario methods.
 */
struct ArtistWeakPtrtoSharedPtr{

	ArtistWeakPtrtoSharedPtr(){
		bp::converter::registry::push_back( &convertible, &construct, bp::type_id<ArtistPtr>() );
	}

	static void* convertible( PyObject* obj_ptr ){
		bp::object O( bp::handle<>(bp::borrowed(obj_ptr)) );
		bp::extract< ArtistWeakHandle > e(O);
		if( e.check() ) return obj_ptr;
		return 0;
	}

	static void construct( PyObject* obj_ptr, bp::converter::rvalue_from_python_stage1_data* data )
	{
		bp::object O( bp::handle<>(bp::borrowed(obj_ptr)) );
		bp::extract< ArtistWeakHandle > e(O);
		if( !e.check() ) bp::throw_error_already_set();
		void* storage = ((bp::converter::rvalue_from_python_storage<ArtistPtr>*)data)->storage.bytes;
		ArtistPtr source(e().lock());
		if( source ){
			new (storage) ArtistPtr( e().lock() );
			data->convertible = storage;
		}
		else{
			PyErr_SetString(PyExc_RuntimeError, "Expired ArtistHandle");
			bp::throw_error_already_set();
		}
	}
};


std::string getDescription( const ArtistWeakHandle& h ){
	return upgradeHandle(h)->description();
}

ArtistKeyList getKeys( const ArtistWeakHandle& h ){
	return upgradeHandle(h)->keys();
}

unsigned numRequiredKeys( const ArtistWeakHandle& h ){
	return upgradeHandle(h)->numRequiredKeys();
}

bool isActive( const ArtistWeakHandle& h ){
	return upgradeHandle(h)->isActive();
}

bp::list artistSettingNames( const ArtistWeakHandle& h ){
	bp::list ret;
	ArtistPtr ptr = upgradeHandle(h);
	const Artist::SettingsMap& settings = ptr->settings();
	for( Artist::SettingsMap::const_iterator i = settings.begin(); i!=settings.end(); ++i )
	{
		ret.append( i->name );
	}
	return ret;
}

bp::dict artistSettings( const ArtistWeakHandle& h ){
	bp::dict ret;
	ArtistPtr ptr = upgradeHandle(h);
	const Artist::SettingsMap& settings = ptr->settings();
	for( Artist::SettingsMap::const_iterator i = settings.begin(); i!=settings.end(); ++i )
	{
		ret[ i->name ] = settingValueToPython( *ptr, i->name, false /*don't evaluate ranges*/ );
	}
	return ret;
}

bp::object artistSettingValue( const ArtistWeakHandle& h, const std::string& key ){
	return settingValueToPython( *upgradeHandle(h), key );
}

SceneState::OverlayHints artistOverlayHints( boost::shared_ptr<Scenario> scenario, const ArtistWeakHandle& h ){
	ArtistPtr ptr = upgradeHandle(h);
	const SceneState& state = scenario->accessState();
	const SceneState::OverlayHintsMap::const_iterator i = state.overlayHintsMap_.find(ptr.get());
	if( i != state.overlayHintsMap_.end() ){
		return i->second;
	}
	else
		return SceneState::OverlayHints();
}

void changeArtistSettingValue( boost::shared_ptr<Scenario> scenario, const ArtistWeakHandle& a,
                               const std::string& key, bp::object value )
{
	// changeSettingFromPython ultimately calls threadSafeQInvoke before modifying the Scenario.
	changeSettingFromPython( scenario, upgradeHandle(a), key, value );
}

ArtistWeakHandle addToScenario( Scenario& s, const std::string& desc,
                                const ArtistKeyList& keys = ArtistKeyList() )
{

	const ArtistFactory& fact = ArtistRegistry::global().getFactory( desc );
	log_debug_stream( "Adding: " << fact.description << " with " << keys.size() << " keys" );

	if( fact.keyCount < keys.size() ){
		throw std::runtime_error("User provided too many keys");
	}

	ArtistKeyList final_keys( keys );
	while( final_keys.size() < fact.keyCount ){
		final_keys.push_back("");
	}

	ArtistPtr p = fact.make(final_keys);
	if( s.thread() == QThread::currentThread() ){
		s.add(p);
	}
	else{
		log_trace( "Beginning queued creation of an artist" );
		Py_BEGIN_ALLOW_THREADS
		QMetaObject::invokeMethod( &s, "add", Qt::BlockingQueuedConnection, Q_ARG(ArtistPtr,p) );
		Py_END_ALLOW_THREADS
		log_trace( "Finished queued creation of an artist" );
	}
	return ArtistWeakHandle(p);
}

void scenarioSetKey( Scenario* s, ArtistPtr a, int i, const std::string& v ){
	threadSafeQInvoke( boost::bind(&Scenario::setKey, s,a,i,v), s );
}

void scenarioRemove( Scenario* s, ArtistPtr a ){
	threadSafeQInvoke( boost::bind(&Scenario::remove, s, a), s );
}

void scenarioClear( Scenario* s ){
	threadSafeQInvoke( boost::bind(&Scenario::clear, s), s );
}

void scenarioSetIsActive( Scenario* s, ArtistPtr a, bool i ){
	threadSafeQInvoke( boost::bind(&Scenario::setIsActive, s, a, i), s );
}

void scenarioSetOverlaySizeHints( Scenario* s, ArtistPtr a, const SceneState::OverlayHints& hints ){
	threadSafeQInvoke( boost::bind(&Scenario::setOverlaySizeHints, s, a, hints ), s );
}

void scenarioSelectOMKeys( Scenario* s, const OMKeySet& k ){
	threadSafeQInvoke( boost::bind(&Scenario::selectOMKeys, s, k ), s );
}

std::vector< ArtistWeakHandle > getCurrentArtists( Scenario const& s )
{
	const std::vector<ArtistPtr>& artists = s.getArtists();
	std::vector< ArtistWeakHandle > ret;
	for( std::vector<ArtistPtr>::const_iterator i = artists.begin();
	     i != artists.end(); ++i )
	{
		ret.push_back( ArtistWeakHandle(*i) );
	}
	return ret;
}

/**
 * Get the names of available artist factories from the global list
 */
std::vector<std::string> listAvailableArtists(){
	std::vector< std::string > ret;
	for( ArtistRegistry::const_iterator it = ArtistRegistry::global().begin();
	     it != ArtistRegistry::global().end(); ++it ){
		ret.push_back( it->description );
	}
	return ret;
}

/**
 * Register a new python artist type with the global list.
 */
void registerPythonArtist( bp::object art_type ){
	const char* module_path = bp::extract<const char*>( art_type.attr("__module__") );
	const char* classname = bp::extract<const char*>( art_type.attr("__name__" ) );
	ArtistRegistry::global().registerExternalArtist( module_path, classname );
}

void export_scenario_types(){

	bp::class_< ArtistWeakHandle >( "ArtistHandle", bp::no_init )
		.add_property( "description", getDescription )
		.add_property( "keys", getKeys )
		.add_property( "numRequiredKeys", numRequiredKeys )
		.add_property( "isActive", isActive )
		.add_property( "settings", artistSettings )
		.def( "settingValue", artistSettingValue )
	;

	bp::class_< std::vector< ArtistWeakHandle > >( "ArtistHandleList", bp::no_init )
		.def( bp::vector_indexing_suite< std::vector< ArtistWeakHandle > >() );

	ArtistWeakPtrtoSharedPtr();

	bp::class_<Scenario, boost::noncopyable>("Scenario", bp::no_init)
		.def( "listAvailableArtists", listAvailableArtists )
		.staticmethod( "listAvailableArtists" )
		.def( "registerArtist", registerPythonArtist )
		.staticmethod( "registerArtist" )
		.def( "getArtists", getCurrentArtists,
		      "Get a list of handles to the scenario's current artists" )
		.def( "add", addToScenario,
		      ( bp::arg("name"), bp::arg("keys")=ArtistKeyList() ),
		      "Add an artist to the scenario" )
		.def( "setKey", scenarioSetKey )
		.def( "remove", scenarioRemove )
		.def( "clear", scenarioClear )
		.def( "setIsActive", scenarioSetIsActive )
		.def( "selectOMKeys", scenarioSelectOMKeys )
		.def( "setOverlaySizeHints", scenarioSetOverlaySizeHints )
		.def( "overlaySizeHints", artistOverlayHints )
		.def( "changeSetting", changeArtistSettingValue )
	;

}

}} // namespace scripting::shovelart
