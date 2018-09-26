#include <iostream>
#include <climits>

#include <QImage>
#include <QFont>

#include <boost/python.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <icetray/OMKey.h>

#include "shovelart/Artist.h"
#include "shovelart/graphicsmath.h"
#include "shovelart/SceneObject.h"
#include "shovelart/Scene.h"
#include "scripting/gil.h"
#include "scripting/qpixmap_from_python.h"
#include "shovelart.h"

// has to be last, because it overwrites a template specialization
#include "scripting/pycompat.h"

namespace scripting{ namespace shovelart {

namespace bp = boost::python;

struct noarg{};

/** Add the template argument to the group, and return the argument. See PySO<>() */
template < typename T >
static inline
T* ret( PyArtistSceneGroup& o, T* t ){
	o.group_->add(t);
	return t;
}

/* Template struct and specializations for creating SceneObjects.
 * This is more friendly to older GCCs than a series of overloaded template functions. */
template < typename T,
           typename A1 = noarg, typename A2 = noarg,
           typename A3 = noarg, typename A4 = noarg,
           typename A5 = noarg, typename A6 /*unimplemented*/ = noarg >
struct PySO;

// specializations by arity

template < typename T >
struct PySO<T>{

	static T* make( PyArtistSceneGroup& output ){
		return ret<T>( output, new T() );
	}

};

template < typename T, typename A1 >
struct PySO<T, A1>{

	static T* make( PyArtistSceneGroup& output, A1 a1 ){
		return ret<T>( output, new T(a1) );
	}

};

template < typename T, typename A1, typename A2 >
struct PySO<T, A1, A2>{

	static T* make( PyArtistSceneGroup& output, A1 a1, A2 a2 ){
		return ret<T>( output, new T(a1,a2) );
	}

};

template < typename T, typename A1, typename A2, typename A3 >
struct PySO<T, A1, A2, A3>{

	static T* make( PyArtistSceneGroup& output, A1 a1, A2 a2, A3 a3 ){
		return ret<T>( output, new T(a1,a2,a3) );
	}

};

template < typename T, typename A1, typename A2, typename A3, typename A4 >
struct PySO<T, A1, A2, A3, A4>{

	static T* make( PyArtistSceneGroup& output, A1 a1, A2 a2, A3 a3, A4 a4 ){
		return ret<T>( output, new T(a1,a2,a3,a4) );
	}

};

template < typename T, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct PySO<T, A1, A2, A3, A4, A5>{

	static T* make( PyArtistSceneGroup& output, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ){
		return ret<T>( output, new T(a1,a2,a3,a4, a5) );
	}

};

// specialize types that need a reference to a QGLWidget, which is never exported explicitly to python
template <>
struct PySO<TextOverlay, const std::string&, const QFont&>{
	static TextOverlay* make( PyArtistSceneGroup& output, const std::string& text, const QFont& font ){
		return ret( output, new TextOverlay(output.state_.gl, text, font ) );
	}
};

template <>
struct PySO<TextObject, const std::string&, vec3d>{
	static TextObject* make( PyArtistSceneGroup& output, const std::string& text, vec3d pos ){
		return ret( output, new TextObject(output.state_.gl, text, pos ) );
	}
};

/**
 * Implementation of sceneobject.setColor() for python objects:
 * the color setting can be either a an rgb[a] tuple, a shovelart.QColor,
 * or a shovelart.VariantQColor.
 */
void setColorFromPython( ColoredObject* so, bp::object color_setting ){

	bp::extract< bp::tuple > tuple_input(color_setting);
	if( tuple_input.check() ){
		bp::tuple c = tuple_input();
		int len = bp::len(c);
		qreal r = bp::extract< qreal >( c[0] );
		qreal g = bp::extract< qreal >( c[1] );
		qreal b = bp::extract< qreal >( c[2] );
		qreal a = len > 3 ? bp::extract< qreal >( c[3] ) : 1.0;
		return so->setColor( new SceneConstant<QColor>( QColor::fromRgbF( r,g,b,a ) ) );
	}

	bp::extract< QColor > const_input(color_setting);
	if( const_input.check() ){
		return so->setColor( new SceneConstant<QColor>(const_input) );
	}

	bp::extract< VariantQColorPtr > qcolor_input(color_setting);
	if( qcolor_input.check() ){
		return so->setColor( qcolor_input() );
	}

	log_error("Could not setColor: problem with python input" );
}

/// create a VariantFloat from an existing variant, or from a constant number
static VariantFloatPtr getFloat(bp::object setting){
	bp::extract<double> constant(setting);
	if( constant.check() ){
		double w = constant();
		return VariantFloatPtr( new SceneConstant<float>( w ) );
	}

	bp::extract< VariantFloatPtr > variant(setting);
	if( variant.check() ){
		return variant();
	}

	return VariantFloatPtr();
}

/// create a VariantVec3d from an existing variant, or from a contant vec3d
static VariantVec3dPtr getVec3d(bp::object setting){
	bp::extract<vec3d> constant(setting);
	if( constant.check() ){
		vec3d v = constant();
		return VariantVec3dPtr( new SceneConstant<vec3d>( v ) );
	}

	bp::extract< VariantVec3dPtr > variant(setting);
	if( variant.check() ){
		return variant();
	}

	return VariantVec3dPtr();
}

/// Set selection content from python-- currently accepts OMKeys only, but can be extended
void setSelectionContentFromPython( SolidObject* so, bp::object scontent ){
	bp::extract<OMKey> omkey(scontent);
	if( omkey.check() ){
		so->setSelectionContent( boost::any(omkey()) );
		return;
	}
	// note: if additional types are accepted in the future, change the pybinding documentation below

	log_error( "Could not setSelectionContentFromPython: unknown data type" );
}

void setLinewidthFromPython( BaseLineObject* so, bp::object line_setting ){
	VariantFloatPtr ptr = getFloat( line_setting );
	if( ptr ){
		return so->setLineWidth( ptr );
	}

	log_error("Could not setLineWidthFromPython: problem with python input" );
}

void setSizeFromPython( SphereObject* so, bp::object size_setting ){
	VariantFloatPtr ptr = getFloat( size_setting );
	if( ptr ){
		return so->setSize( ptr );
	}

	log_error("Could not setSizeFromPython: problem with python input" );
}

void setLocationFromPython( SolidObject* so, bp::object location_setting ){
	VariantVec3dPtr loc = getVec3d( location_setting );
	if( loc ){
		return so->setLocation( loc );
	}
	log_error("Could not setLocationFromPython: problem with python input");
}

void setBaseRadiusFromPython( CylinderObject* so, bp::object rad_setting ){
	if( VariantFloatPtr f = getFloat( rad_setting ) )
		return so->setBaseRadius( f );
	log_error( "Problem with python input" );
}

void setTopRadiusFromPython( CylinderObject* so, bp::object rad_setting ){
	if( VariantFloatPtr f = getFloat( rad_setting ) )
		return so->setTopRadius( f );
	log_error( "Problem with python input" );
}

void setAxisFromPython( CylinderObject* so, bp::object axis_setting ){
	if( VariantVec3dPtr a = getVec3d( axis_setting ) )
		return so->setAxis( a );
	log_error( "Problem with python input" );
}

vec3d getAxisFromPython( CylinderObject* so, double t ){
	return so->axis(t);
}

void SphereObject_setShader( SphereObject& sphere, int mode ){
	sphere.setShader( static_cast<SphereObject::ShadeMode>(mode) );
}

/**
 * Helper function for defining new members of the python version of the SceneGroup API.
 *
 * This defines a Python class for the given SceneObject type, and create the SceneGroup.addX method for
 * creating it.
 */
template < typename T, typename Bases, class Class, typename F, typename Args >
bp::class_<T, T*, Bases, boost::noncopyable >
export_SceneObject(const char* name, Class cl, F f, const char* doc, Args args ){

	bp::class_<T, T*, Bases, boost::noncopyable > newclass( name,
		(std::string("SceneObject subclass created using SceneGroup.") + (std::string("add")+name)).c_str(),
		bp::no_init
	);

	cl.def( (std::string("add")+name).c_str(), f,
	         doc, (bp::arg("self"), args), bp::return_value_policy<bp::reference_existing_object>() );

	return newclass;
}

/**
 * This SceneObject does not represent anything in the OpenGL scene.
 * A PyArtist that edits a non-Scene resource (like a matplotlib window)
 * can specify a cleanup action that will fire when the output
 * of the artist becomes invalid, or a draw action that will fire
 * whenever the currently visualized time changes.  Draw actions should
 * be used very sparingly as they are a substantial performance problem,
 * but it's the only way we have to animate Maptlotlib plots over time.
 *
 * Yep, this is totally icky.
 */
class PhantomSceneObject : public SceneObject {

public:
	bp::object cleanup_;
	bp::object draw_action_;
	bool has_draw_action_;
	double last_draw_action_time_;

	PhantomSceneObject( bp::object cleanup, bp::object draw_action = bp::object() ) :
		cleanup_(cleanup),
		draw_action_(draw_action),
		has_draw_action_(draw_action != bp::object()),
		last_draw_action_time_(-std::numeric_limits<double>::max())
	{}

	~PhantomSceneObject(){
		ScopedGIL gil;
		try{
			cleanup_();
		}
		catch( bp::error_already_set& ){
			log_error( "Python exception in ~PhantomSceneObject" );
			PyErr_Print();
		}
	}

	virtual void draw( double vistime, const I3Camera& camera ){
		ScopedGIL gil;
		draw_action_(vistime);
		last_draw_action_time_ = vistime;
	}

	// Only request a draw() if a draw action exists and needs to be re-run
	virtual bool isVisible( double vistime ){
		return has_draw_action_ && (last_draw_action_time_ != vistime );
	}

	virtual RenderPass pass( double vistime ){ return OVERLAY; }
	virtual bool isSelectable( double vistime ){ return false; }
	virtual float cameraDistance( double vistime, const I3Camera& camera ){ return 0.f; }
};

/**
 * Python version of ActiveImageOverlay, calling a given python method on each redraw
 */
class PyActiveImageOverlay : public ActiveImageOverlay {
public:
	bp::object redraw_;

	PyActiveImageOverlay( QGLWidget* w, bp::object redraw ) :
		ActiveImageOverlay( w ), redraw_(redraw)
	{}

	virtual QPixmap redraw( double vistime, const I3Camera& camera ){
		ScopedGIL gil;
		bp::tuple returned = bp::extract<bp::tuple>(redraw_( vistime ))();
		bp::object arg0 = returned[0];
		PyObject* str = arg0.ptr();
		int w = bp::extract<int>(returned[1])();
		int h = bp::extract<int>(returned[2])();
		return qpixmap_from_python( str, w, h );
	}

};

// further PySO specializations for overlays
template <>
struct PySO<ImageOverlay, int, int, PyObject* >{
	static ImageOverlay* make( PyArtistSceneGroup& output, int w, int h, PyObject* str ){
		return ret( output, new ImageOverlay( output.state_.gl,
		                                      qpixmap_from_python( str, w, h ) ) );
	}
};

template <>
struct PySO<PyActiveImageOverlay, bp::object >{
	static PyActiveImageOverlay* make( PyArtistSceneGroup& output, bp::object redraw ){
		return ret( output, new PyActiveImageOverlay( output.state_.gl, redraw ) );
	}
};


void export_scene_object_types(){
	using bp::arg;

	bp::class_<PyArtistSceneGroup, boost::noncopyable> classdef( "SceneGroup",
		"Output structure used within PyArtist.create().  Call methods on this object to create SceneObjects.",
		bp::no_init
	);

	bp::class_< SceneObject, SceneObject*, boost::noncopyable >( "SceneObject", bp::no_init );

	bp::class_< ColoredObject, ColoredObject*, boost::noncopyable >( "ColoredObject", bp::no_init )
		.def( "setColor", &setColorFromPython,
		      "Set this object's color as a RGB[A] tuple, a PyQColor, or a VariantPyQColor");

	bp::class_< BaseLineObject, BaseLineObject*, boost::noncopyable >( "BaseLineObject", bp::no_init )
		.def( "setLineWidth", &setLinewidthFromPython,
		      "Set this object's line width to a constant number or a VariantFloat")
		.def( "setStippleScale", &BaseLineObject::setStippleScale,
		      "Set this object's line stippling scale.  0 indicates no stippling." )
		.def( "setStipplePattern", &BaseLineObject::setStipplePattern,
		      "Set this object's line stippling pattern.")
		;

	bp::class_< SolidObject, SolidObject*, bp::bases<ColoredObject>, boost::noncopyable >( "SolidObject", bp::no_init )
		.def( "setLocation", &setLocationFromPython,
		      "Set this object's reference locaton to a Vec3d or a VariantVec3d" )
		.def( "setSelectionContent", &setSelectionContentFromPython,
		      "Set this object's selection content; OMKey is the only accepted type." );

	bp::class_< SceneOverlay, SceneOverlay*, bp::bases<SceneObject>, boost::noncopyable > ( "SceneOverlay", bp::no_init );

	export_SceneObject<PhantomSceneObject, bp::bases<SceneObject> >(
		"Phantom", classdef,
		PySO<PhantomSceneObject, bp::object, bp::object >::make,
		"Create a pseudo scene object that represents nothing in the visible scene.  When the SceneGroup this object "
		"is in is deleted, it will run the given cleanup action.  It will run the optional draw action whenever "
		"the current visible time changes, which allows PyArtists to hook into the animation system, at the expense "
		"of performance.  Both these features should be used sparingly (see the matplotlib artists for examples).",
		( arg("cleanup_action"), arg("draw_action") = bp::object() )
	);

	export_SceneObject<ImageOverlay, bp::bases< SceneOverlay > >(
		"PixmapOverlay", classdef,
		PySO<ImageOverlay, int, int, PyObject* >::make,
		"Create an RGB image overlay with the given width, height, and data",
		( arg("width"), arg("height"), arg("data" ) )
	);

	export_SceneObject<PyActiveImageOverlay, bp::bases< ImageOverlay > >(
		"ActivePixmapOverlay", classdef,
		PySO<PyActiveImageOverlay, bp::object>::make,
		"Add an overlay that redraws itself over time.  The redraw_function should accept an integer time paramter, "
		"and return a tuple (width, height, data) corresponding to the arguments of addPixmapOverlay",
		( arg("redraw_function") )
	);

	export_SceneObject<ZPlaneObject, bp::bases<SceneObject,ColoredObject,BaseLineObject> >(
		"ZPlane", classdef,
		PySO<ZPlaneObject,double,double,double,double,double>::make,
		"Add a z-oriented quadrangle represented by a grid of lines, e.g. as the surface of the ice sheet.  "
		"The arguments specify the width and center of the quad.",
		( arg( "z" ), arg("xwidth")=2000, arg("ywidth")=2000, arg("xctr") = 0, arg("yctr") = 0 )
	);

	export_SceneObject<StaticLineObject, bp::bases<SceneObject,ColoredObject,BaseLineObject> >(
		"StaticLines", classdef,
		PySO<StaticLineObject,const std::vector<vec3d>&>::make,
		"Add a set of motionless 3D lines by specifying a list of their vertices.",
		( arg( "vertices" ) )
	);

	export_SceneObject<DynamicLineObject, bp::bases<SceneObject, ColoredObject, BaseLineObject> >(
		"DynamicLines", classdef,
		PySO<DynamicLineObject, const std::vector<VariantVec3dPtr>&>::make,
		"Add a set of 3D lines by specifying a list of their vertices as VariantVec3ds.  This allows "
		"the lines to be animated.  addStaticLines should be used if a static object is required.",
		( arg( "variant_vertices" ) )
	);

	export_SceneObject<ParticlePath, bp::bases<StaticLineObject> >(
		"ParticlePath", classdef,
		PySO<ParticlePath>::make,
		"Add a ParticlePath object, a specialized form of DynamicLines optized to show the progress of "
		"a particle through space.",
		bp::detail::keywords<0>() // no arguments
	)
	.def( "addPoint", &ParticlePath::addPoint );

	export_SceneObject<TextOverlay, bp::bases<SceneOverlay> >(
		"TextOverlay", classdef,
		PySO<TextOverlay, const std::string&, const QFont&>::make,
		"Add a text label as an overlay with the given text and, optionally, a specific font",
		( arg( "text" ), arg( "font" ) = QFont() )
	)
	.def( "setFont", &TextOverlay::setFont );

	export_SceneObject<TextObject, bp::bases<SceneObject> >(
	    "Text", classdef,
	    PySO<TextObject, const std::string&, vec3d>::make,
	    "Add a text label at a specific position in 3D space.",
	    ( arg( "text" ), arg( "pos" ) )
	)
	.def( "setFont", &TextObject::setFont );

	export_SceneObject<OverlayLine, bp::bases<SceneObject,BaseLineObject,ColoredObject> >(
		"OverlayLine", classdef,
		PySO<OverlayLine, const SceneOverlay*, vec3d>::make,
		"Add an overlay line from the middle of start, which is a SceneOverlay object, and end, which is a vec3d.  "
		"This effectively draws a line from 2D pixel space to 3D object space.",
		( arg( "start" ), arg( "end" ) )
	);

	export_SceneObject<SphereObject, bp::bases<SolidObject> >(
		"Sphere", classdef,
		PySO<SphereObject, float, const vec3d&>::make,
		"Add a sphere with the given radius and vertex location.",
		( arg( "radius"), arg("location") )
	)
	.def( "setSize", &setSizeFromPython )
	.def( "setShader", SphereObject_setShader )
	;

	export_SceneObject<CylinderObject, bp::bases<SolidObject> >(
		"Cylinder", classdef,
		PySO<CylinderObject, vec3d, vec3d, float, float>::make,
		"Add a cylinder with the given parameters",
		( arg("vertex"), arg("axis"), arg("baserad"), arg("toprad") = 0.f )
	)
	.def( "setAxis", &setAxisFromPython,
	      "Set the main axis of this cylinder to a Vec3d or a VariantVec3d" )
	.def( "axis", &CylinderObject::axis,
	      "Get the main axis at the specified time")
	.def( "setBaseRadius", &setBaseRadiusFromPython,
	      "Set the base radius of this cylinder to a number or a VariantFloat" )
	.def( "setTopRadius", &setTopRadiusFromPython,
	      "Set the top radius of this cylinder to a number or a VariantFloat" )
	;

	export_SceneObject<ArrowObject, bp::bases<SceneObject, ColoredObject, BaseLineObject> >(
	    "Arrow", classdef,
	    PySO<ArrowObject, VariantVec3dPtr, VariantVec3dPtr, float, float>::make,
	    "Add an arrow with the given parameters",
	    ( arg("start_vertex"), arg("stop_vertex"), arg("head_angle"), arg("head_length") )
    )
    ;
}

}} // namespace scripting::shovelart
