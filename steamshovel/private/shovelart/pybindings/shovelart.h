#ifndef I3_SHOVEL_SCRIPTING_RENDERING_H
#define I3_SHOVEL_SCRIPTING_RENDERING_H

#include <boost/python/object_fwd.hpp>

#include "shovelart/Artist.h"
#include "shovelart/Scenario.h"
#include "shovelart/SceneObject.h"

class SceneGroup;
class SceneState;
class QObject;

// This header contains internal declarations for shovelart module's implementation
// and should not be moved to a public directory

namespace scripting{  namespace shovelart {

	// PyArtist's stateful SceneGroup object
	struct PyArtistSceneGroup{
		SceneGroup* group_;
		const SceneState& state_;
		PyArtistSceneGroup( SceneGroup* group, const SceneState& state ) :
			group_(group), state_(state)
		{}
	};

	// the following functions are implemented and documented in Settings.cpp

	// Invoke a nullary function in the thread that owns object Q.
	// This allows thread-safe calls to a Scenario object.
	void threadSafeQInvoke( const boost::function<void(void)>& c, QObject* q );

	// functions for reading and creating Artist::Settings from python code
	boost::python::object settingValueToPython( const Artist& a,
	                                            const std::string& key,
	                                            bool evaluate = true );

	Artist::Setting newSettingFromPython( Artist* a,
	                                      boost::python::object obj );

	void changeSettingFromPython( boost::shared_ptr<Scenario> scenario,
	                              ArtistPtr a,
	                              const std::string& key,
	                              boost::python::object value );

}} // namespace scripting::shovelart

// Comparison operator necessary to put weak pointers into a vector_indexing_suite
// must be at global scope
template< typename T, typename U>
inline bool operator==(const boost::weak_ptr<T>& a, const boost::weak_ptr<U>& b ){
       return !(a < b) && !(b < a);
}

#endif /* I3_SHOVEL_SCRIPTING_RENDERING_H */
