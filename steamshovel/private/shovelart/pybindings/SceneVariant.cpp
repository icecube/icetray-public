#include <sstream>

#include <QColor>
#include <QFont>
#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "shovelart/Artist.h"
#include "shovelart/graphicsmath.h"
#include "shovelart/SceneVariant.h"
#include "shovelart/ColorVariant.h"
#include "shovelart/Scene.h"
#include "shovelart/graphicsmath.h"
#include "scripting/gil.h"
#include "shovelart.h"

// has to be last, because it overwrites a template specialization
#include "scripting/pycompat.h"

namespace scripting{ namespace shovelart{

namespace bp = boost::python;

/// wrapper type for SceneVariant subclasses defined in Python
template < typename T >
class VariantWrap : public SceneVariant<T>, public bp::wrapper< SceneVariant<T> >
{
public:
	virtual T value( double vistime ){
		ScopedGIL gil;
		return this->get_override("value")(vistime);
	};
};

/**
 * Pythonic TimeWindowColor constructor
 */
TimeWindowColorPtr make_TimeWindowColor( bp::object arg1,
                                         bp::object arg2,
                                         ColorMapPtr cm )
{
	const TimeWindow* times = 0;
	bp::extract<PyArtistSceneGroup> get_scene_group(arg1);
	if( get_scene_group.check() ){
		times = get_scene_group().state_.times;
	} else {
		times = bp::extract<TimeWindow*>(arg1);
	}

	bp::extract<double> get_double(arg2);
	if( get_double.check() ){
		const double t = get_double();
		return TimeWindowColorPtr( new TimeWindowColor( times, t, cm ) );
	}else{
		std::vector<double> t(bp::len(arg2));
		for( unsigned i = 0, n = t.size(); i < n; ++i )
			t[i] = bp::extract<double>(arg2[i]);
		return TimeWindowColorPtr( new TimeWindowColor( times, t, cm ) );
	}
}

/**
 * Pythonic ParticlePoint contructor
 */
ParticlePointPtr make_ParticlePoint( const PyArtistSceneGroup& out,
                                     const I3ParticleConstPtr& particle,
                                     TimeWindow::TimePoint time )
{
	return ParticlePointPtr( new ParticlePoint( particle, WindowTime( out.state_.times, time ) ) );
}


/**
 * Export variant types in Python for the templated type
 * For T=TypeName:
 *    - Export VariantTypeName and ConstantTypeName
 *    - Export PyVariantTypeName if wrapper == true
 *    - Export StepFunctionTypeName if step == true
 *    - Export LinterpFunctionTypeName if linterp == true
 */
template <typename T>
void
export_SceneVariant( const std::string& Tname, bool wrapper = false, bool step = false, bool interp = false ){

	typedef SceneVariant<T> VT;
	std::string name = "Variant" + Tname;

	// If a wrapper is to be defined, it needs to be defined first;
	// exposing the wrapper class after the base class causes further
	// subclasses of the base class to be incorrectly given the wrapper
	// as their base class.  This may be a bp bug or I may just not
	// understand.  The workaround comes from http://mail.python.org/pipermail/cplusplus-sig/2010-July/015646.html
	if( wrapper ){
		typedef VariantWrap<T> VWT;
		std::string wrapperName = "Py" + name;
		bp::class_< VWT, boost::shared_ptr< VWT >, boost::noncopyable >( wrapperName.c_str(), bp::init<>() )
			.def( "value", bp::pure_virtual(&VT::value) );
	}

	// define the base class
	bp::class_< VT, boost::shared_ptr<VT>, boost::noncopyable >( name.c_str(), bp::no_init )
		.def( "value", bp::pure_virtual( &VT::value ) );

	// constant form
	typedef SceneConstant<T> SCT;
	std::string constantName = "Constant" + Tname;
	bp::class_< SCT, boost::shared_ptr<SCT>, bp::bases<VT> >( constantName.c_str(), bp::init<T>() );

	// step function form
	if( step ){
		typedef SceneStepFunction<T> SST;
		std::string stepname =  "StepFunction" + Tname;
		bp::class_< SST, boost::shared_ptr< SST >, bp::bases<VT> >(
			  stepname.c_str(), bp::init<T,bp::optional<double> >() )
			.def( "add", &SST::add )
			.def( "value", &SST::value )
		;
	}

	// linterp form
	if( interp ){
		typedef SceneLinterpFunction<T> SLT;
		std::string linterpname =  "LinterpFunction" + Tname;
		bp::class_< SLT, boost::shared_ptr< SLT >, bp::bases<VT> >( linterpname.c_str(), bp::init<T,bp::optional<double> >() )
			.def( "add", &SLT::add );
	}
}



void export_scene_variant_types(){

	// Any type exported with export_SceneVariant and wrapper==true should also be included
	// in the SVTYPES list in in scripting/pycompat.cpp

	export_SceneVariant< vec3d >( "Vec3d", true, true, true );
	bp::class_< std::vector<VariantVec3dPtr> >( "VariantVec3dList" )
		.def( bp::vector_indexing_suite< std::vector< VariantVec3dPtr > >() );

	export_SceneVariant< float >( "Float", true, true, true );

	export_SceneVariant< double >( "Time", true, true, false );

	export_SceneVariant< QColor >( "QColor", true, true, true );

	bp::class_< TimeWindow, TimeWindow* >( "TimeWindow", bp::init<double, double>() ); 

	bp::class_< TimeWindowColor,
	            TimeWindowColorPtr,
	            bp::bases< VariantQColor >,
	            boost::noncopyable >( "TimeWindowColor", bp::no_init )
		.def( "__init__", bp::make_constructor(make_TimeWindowColor) )
		// .def( bp::init<TimeWindow*, double, ColorMapPtr>() )
		.def( "value", &TimeWindowColor::value )
	;

	bp::enum_< TimeWindow::TimePoint >("TimePoint")
		.value( "START", TimeWindow::START )
		.value( "COLOR_START", TimeWindow::COLOR_START )
		.value( "CURRENT", TimeWindow::CURRENT )
		.value( "COLOR_END", TimeWindow::COLOR_END )
		.value( "END", TimeWindow::END )
	;

	bp::class_< ParticlePoint, ParticlePointPtr, bp::bases<VariantVec3d> >( "ParticlePoint", bp::no_init )
		.def( "__init__", bp::make_constructor(&make_ParticlePoint) )
		.def( "value", &ParticlePoint::value )
	;
}

}} // namespace scripting::shovelart
