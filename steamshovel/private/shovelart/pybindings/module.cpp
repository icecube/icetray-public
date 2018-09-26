#include "shovelart.h"

#include <boost/python.hpp>
#include <icetray/load_project.h>

#include <QMetaType>

namespace scripting{ namespace shovelart{

	void export_shovelart_types(); // Types.cpp
	void export_pyartist_types(); // PyArtist.cpp
	void export_scene_object_types(); // SceneObject.cpp
	void export_scene_variant_types(); // SceneVariant.cpp
	void export_scenario_types(); // Scenario.cpp

BOOST_PYTHON_MODULE(shovelart){

	// The Scenario bindings will use ArtistPtrs in queued metamethod invocations
	qRegisterMetaType< ArtistPtr >();

	// keep shovelart docstrings, disable c++ signatures; this looks best in Sphinx output
	boost::python::docstring_options doc_opts;
	doc_opts.disable_cpp_signatures();

	load_project( "shovelart", false );

	using namespace scripting::shovelart;
	export_shovelart_types();
	export_scene_variant_types();
	export_scene_object_types();
	export_pyartist_types();
	export_scenario_types();
};

}} // namespace scripting::shovelart
