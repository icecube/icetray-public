#ifndef I3_SHOVEL_RENDERING_SCENE_OBJECT_H
#define I3_SHOVEL_RENDERING_SCENE_OBJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include <QImage>
#include <QFont>

#include "SceneVariant.h"
#include "graphicsmath.h"
#include "gl.h"
#include "Texture.h"

class I3Camera;
class QGLWidget;

class QGLShaderProgram;

/** Static shader compiler and loader */
class ShaderManager{

private:
	static std::vector<QGLShaderProgram*> programs;
	static int active_program;

	static void setupSphereShader( QString, bool& );

public:
	// all available shaders have a public identifier here
	enum ShaderType{
		NONE = -1,
		SHINY_SPHERE = 0,
		MATTE_SPHERE = 1,
		RING_SPHERE = 2
	};

	static void createShaders();
	static bool shadersAreAvailable(){ return programs.size() && programs[0] != NULL; }

	/**
	 * Ensure the current shader type is active, if that is possible.
	 * Returns NULL if not.
	 */
	static QGLShaderProgram* ensureBound( ShaderType t );

	/**
	 * Release the current shader if any
	 **/
	static void release();

	static QGLShaderProgram* currentlyBound(){
		if( active_program != NONE ){
			return programs.at(active_program);
		}
		return NULL;
	}

};

// ------------------------- SceneObject Abstract Base class -------------------------

/**
 * Base class for things that can be drawn by Artists and displayed in a Scene
 *
 * Defines the essential virtual functions that a thing must define to participate
 * in OpenGL drawing.  Most functions work in terms of a integer 'vistime,' which
 * is the nanosecond of the event currently being displayed.
 */
class SceneObject{

public:
	/// the passes of normal rendering, in order; each SceneObject will be drawn in one of these
	enum RenderPass{
		SOLID = 0,
		LINE = 1,
		TRANSPARENT = 2,   // solid objects with alpha != 1
		SHADERTRANSPARENT = 3, // shader-drawn solid objects with alpha != 1
		OVERLAY = 4
	};
	virtual ~SceneObject(){}
	/** Actally draw the object to OpenGL at the given vistime */
	virtual void draw( double vistime, const I3Camera& camera ) = 0;
	/** Is the object visible at the given time? */
	virtual bool isVisible( double vistime ) = 0;
	/** What render pass should the object be drawn on? */
	virtual RenderPass pass( double vistime ) = 0;
	/** Can the object be mouse selected at the given time? */
	virtual bool isSelectable( double vistime ) = 0;
	/**
	 *  Return an estimate of the distance from the camera to this object.
	 *  This is used to sort transparent objects back-to-front
	 */
	virtual float cameraDistance( double vistime, const I3Camera& camera ) = 0;
	/**
	 *  Define the selection content to be used if the user clicks on this object;
	 *  this function is ignored for nonselectable objects
	 */
	virtual boost::any selectionContent(){ return boost::any(); }

	/**
	 * The shader that needs to be active to draw this object, if any
	 */
	virtual ShaderManager::ShaderType shader(double vistime){ return ShaderManager::NONE; }

};

// ------------------------- Mix-ins for SceneObjects -------------------------

/**
 * Mixin for SceneObjects that have a color
 */
class ColoredObject{

public:
	ColoredObject() :
		color_( new SceneConstant<QColor>( Qt::white ) )
	{}

	void setColor( VariantQColorPtr::element_type* c ){ setColor( VariantQColorPtr( c ) ); }
	void setColor( VariantQColorPtr c ){ color_ = c; }
	void setColor( QColor c ){ color_ = VariantQColorPtr(new SceneConstant<QColor>( c )); }
	QColor color( double vistime ) const { return color_->value(vistime); }

protected:
	void applyColor( double vistime );
	SceneObject::RenderPass getPassByAlpha( double vistime );
	VariantQColorPtr color_;

};

inline SceneObject::RenderPass ColoredObject::getPassByAlpha(double vistime){
	return (color_->value(vistime).alphaF() < 1) ? SceneObject::TRANSPARENT : SceneObject::SOLID;
}


/**
 * Mixin for SceneObjects that are made of lines, and thus can define
 * a gl line width and stipple pattern
 */
class BaseLineObject{

protected:
	static bool stipple_enabled_;
	VariantFloatPtr lineWidth_;
	GLint stippleScale_;
	GLushort stipplePattern_;

	BaseLineObject():
		lineWidth_( new SceneConstant<float>(1.0f) ),
		stippleScale_(0),
		stipplePattern_(0x5555)
	{}

	void applyLineWidth( double vistime, const I3Camera& camera );
	void applyStipple( double vistime, const I3Camera& camera );

public:
	void setLineWidth( VariantFloatPtr::element_type* w ){ setLineWidth( VariantFloatPtr( w ) ); }
	void setLineWidth( VariantFloatPtr w ){ lineWidth_ = w; }
	void setStippleScale( GLint scale ){ stippleScale_ = scale; }
	void setStipplePattern( GLushort pattern ){ stipplePattern_ = pattern; }
	float lineWidth( double vistime ) const { return lineWidth_->value(vistime); }

};

// ------------------------- Overlays -------------------------

/**
 * Superclass for SceneObjects that are drawn in 2D coordinates on top of the 3D scene
 * Such objects are unique in being able to be dragged and resized by the user.
 *
 * SceneObjects are allowed to ignore resize commands (e.g. TextOverlay).  They may
 * also be flagged as having a fixed aspect ratio, which allows resizing but only
 * without changing their shape.
 */
class SceneOverlay : public SceneObject {
protected:
	Texture texture_;
	int xanchor_, yanchor_;
	bool fixed_aspect_;

public:
	/**
	 * Represents the size and position of an overlay on the screen.  These are remembered
	 * by the SceneState so that overlays resume their previous size and location when
	 * they are re-created later in the program.
	 */
	struct SizeHint{
		int x, y;
		int width, height;
		SizeHint( int X, int Y, int w, int h ):
			x(X), y(Y), width(w), height(h)
		{}

		SizeHint():x(0),y(0),width(0),height(0){}

		/** Equality operator, necessary because this type participates in std::vectors
		 * that are exported via boost::python::vector_indexing_suite
		 */
		bool operator==( const SizeHint& hint ) const {
			return (hint.x == x) && (hint.y == y ) && ( hint.width == width ) && (hint.height == height);
		}

		QSize toQSize(){ return QSize( width, height ); }

		SizeHint scaled( double factor ){ return SizeHint(x*factor, y*factor, width*factor, height*factor); }
	};

	SceneOverlay( QGLWidget* w, GLint format = GL_RGBA ) :
		texture_(w, format),
		xanchor_(10),
		yanchor_(10),
		fixed_aspect_(false) {}

	//virtual void draw( double vistime, const I3Camera& camera );
	virtual bool isVisible( double vistime ){ return true; }
	virtual RenderPass pass(double vistime){ return OVERLAY; }
	virtual bool isSelectable( double vistime ){ return true; }
	virtual boost::any selectionContent(){ return boost::any(this); }
	virtual float cameraDistance( double vistime, const I3Camera& camera ){ return 0.f; }

	void move( int dx, int dy ){ xanchor_ += dx; yanchor_ += dy; }
	void setAnchor( int x, int y ){ xanchor_ = x; yanchor_ = y; }
	void setFixedAspect( bool x ){ fixed_aspect_ = x; }
	void resize( int dx, int dy ){
		if(fixed_aspect_){
			int w = width() + dx;
			double multiplier = double(w) / double(width());
			int h = lround( height() * multiplier );
			texture_.setSize( w, h );
		}
		else{
			texture_.setSize( width() + dx, height() + dy );
		}
	}

	int xpos() const { return xanchor_; }
	int ypos() const { return yanchor_; }
	int width() const { return texture_.width(); }
	int height() const { return texture_.height(); }

	SizeHint makeHint() const{ return SizeHint(xanchor_,yanchor_,width(),height()); }
	void applyHint( const SizeHint& hint ){
		xanchor_ = hint.x;
		yanchor_ = hint.y;
		texture_.setSize( hint.width, hint.height );
	}
};

/**
 * A textual label overlay
 */
class TextOverlay : public SceneOverlay, public ColoredObject {
protected:
	std::string text_;
	QFont font_;
	double scaling_factor_;
	bool needs_update_;

public:
	TextOverlay( QGLWidget* w, std::string text, QFont font=QFont() ) :
		SceneOverlay(w),
		text_(text),
		font_(font),
		scaling_factor_(1.0),
		needs_update_(true)
	{}

	virtual void draw( double vistime, const I3Camera& camera );

	void setFont( const QFont& font )
	{ font_ = font; needs_update_ = true; }
};

/**
 * An overlay for an arbitrary image
 */
class ImageOverlay : public SceneOverlay {

public:
	// if resize_to is specified, the image data will be smoothly scaled to the requested size
	ImageOverlay( QGLWidget*w, const QPixmap& data, const QSize& resize_to = QSize() );

	virtual void draw( double vistime, const I3Camera& camera );
};

/**
 * An ImageOverlay that redraws itself whenever the vistime changes
 */
class ActiveImageOverlay : public ImageOverlay {

public:
	ActiveImageOverlay( QGLWidget* w );

	/** This method will be called for each draw() invocation that has a novel vistime */
	virtual QPixmap redraw( double vistime, const I3Camera& camera ) = 0;
	virtual void draw( double vistime, const I3Camera& camera );

private:
	double last_vistime_;

};

/**
 * A 3D-to-2D indicator line -- draws a line from a point in 3D space to a screen position
 */
class OverlayLine : public SceneObject, public BaseLineObject, public ColoredObject {

private:
	const SceneOverlay* start_;
	vec3d end_;

public:
	OverlayLine( const SceneOverlay* start, vec3d end ) :
		start_(start), end_(end)
	{}

	virtual bool isVisible( double vistime ){ return true; }
	virtual RenderPass pass(double vistime){ return OVERLAY; }
	virtual bool isSelectable( double vistime ){ return false; }
	// give a virtual camera distance greater than 0, the standard SceneOverlay distance,
	// to ensure that OverlayLines are drawn before the overlays they decorate
	virtual float cameraDistance( double vistime, const I3Camera& camera ){ return 1.f; }
	virtual void draw( double vistime, const I3Camera& camera );

};

// ------------------------- Concrete SceneObjects -------------------------

/**
 * SceneObject representing a camera-facing text label in 3D space
 */
class TextObject : public SceneObject {
protected:
	Texture texture_;
	std::string text_;
	vec3d loc_;
	QFont font_;
	bool needs_update_;

public:
	TextObject( QGLWidget* w, const std::string& text, const vec3d& loc ):
		texture_(w),
		text_(text),
		loc_(loc),
		needs_update_(true)
	{}

	virtual void draw( double vistime, const I3Camera& camera );
	virtual bool isVisible( double vistime ){ return true; }
	virtual RenderPass pass(double vistime){ return TRANSPARENT; }
	virtual bool isSelectable( double vistime ){ return false; }
	virtual float cameraDistance( double vistime, const I3Camera& camera );

	void setFont( const QFont& font )
	{ font_ = font; needs_update_ = true; }
};

/**
 * SceneObject representing a z-oriented grid plane, used to draw large surfaces
 * like the top of the ice
 */
class ZPlaneObject : public SceneObject, public ColoredObject, public BaseLineObject {
	public:
		ZPlaneObject ( double z, double xwidth=2000, double ywidth=2000, double xctr = 0, double yctr = 0 ):
			z_(z), xctr_(xctr), yctr_(yctr), xwidth_(xwidth), ywidth_(ywidth)
		{
			setColor(new SceneConstant<QColor>(QColor::fromRgbF(0,0,.5) ) );
		}

	virtual void draw( double vistime, const I3Camera& camera );
	virtual bool isVisible( double vistime ){ return true; }
	virtual RenderPass pass(double vistime){ return SOLID; }
	virtual bool isSelectable( double vistime ){ return false; }
	virtual float cameraDistance( double vistime, const I3Camera& camera ){ return 1.f; }

	protected:
		double z_, xctr_, yctr_, xwidth_, ywidth_;
};


/**
 * SceneObject representing a set of sequential lines to be drawn by GL_LINE_STRIP
 */
template <typename PointType>
class LineObject : public SceneObject, public ColoredObject, public BaseLineObject {

protected:
	std::vector<PointType> points_;

	vec3d get( int idx, double vistime );

public:
	LineObject( const std::vector<PointType>& points ) : SceneObject(),
		points_(points)
	{}

	virtual void draw( double vistime, const I3Camera& camera );
	virtual bool isVisible( double vistime ){ return true; }
	virtual RenderPass pass(double vistime){ return LINE; }
	virtual bool isSelectable( double vistime ){ return false; }
	virtual float cameraDistance( double vistime, const I3Camera& camera );


};

/** A LineObject whose geometry does not change with time */
typedef LineObject<vec3d> StaticLineObject;
/** A LineObject who geometry changes with time */
typedef LineObject<VariantVec3dPtr> DynamicLineObject;


/** Customized line object for multi-point traveling particle */
class ParticlePath : public LineObject<vec3d> {

private:
	std::vector< double > times_;
	std::vector< QColor > colors_;

public:
	ParticlePath() :
		LineObject<vec3d>( std::vector<vec3d>() )
	{}

	/**
	 * Add a time / location / color point to this path.  Points should be added
	 * in order by time; the leading edge of the path will be linearly interpolated
	 * between the points.
	 */
	void addPoint( double t, const vec3d& pt, const QColor& c ){
		times_.push_back(t);
		points_.push_back(pt);
		colors_.push_back(c);
	}

	virtual void draw( double vistime, const I3Camera& camera );
	virtual bool isVisible( double vistime );

};

/**
 * Representative type for SceneObjects that are normal geometry types,
 * having a location and a selection content.  The draw() method wraps
 * the pure virtual drawBaseObject() method, and sets up a transformation
 * for the location.
 */
class SolidObject : public SceneObject, public ColoredObject {

protected:
	SolidObject( vec3d location = vec3d() ) :
		SceneObject(),
		loc_( new SceneConstant<vec3d>( location ))
	{}

public:
	virtual void draw( double vistime, const I3Camera& camera );
	virtual RenderPass pass( double vistime ){ return getPassByAlpha(vistime); }
	virtual bool isSelectable( double vistime ){ return !(select_.empty()) && isVisible(vistime); }
	virtual float cameraDistance( double vistime, const I3Camera& camera ) = 0;
	virtual void drawBaseObject(double vistime, const I3Camera&) = 0;

	void setLocation( VariantVec3dPtr::element_type* p ){ setLocation( VariantVec3dPtr(p) ); }
	void setLocation( VariantVec3dPtr p ){ loc_ = p; }
	vec3d location( double vistime ) const { return loc_->value(vistime); }

	void setSelectionContent( boost::any c ){
		select_ = c;
	}
	virtual boost::any selectionContent(){ return select_; }


protected:
	VariantVec3dPtr loc_;
	boost::any select_;

};

/**
 * Cylinder or cone object
 */
class CylinderObject : public SolidObject {

protected:
	VariantVec3dPtr axis_;
	VariantFloatPtr base_radius_;
	VariantFloatPtr top_radius_;

public:
	CylinderObject( vec3d vertex, vec3d axis, float base, float top = 0 );
	virtual bool isVisible( double vistime );
	virtual float cameraDistance( double vistime, const I3Camera& camera );
	virtual void drawBaseObject(double vistime, const I3Camera&);

	void setAxis( VariantVec3dPtr::element_type* p ){ setLocation( VariantVec3dPtr(p) ); }
	void setAxis( VariantVec3dPtr p ){ axis_ = p; }
	vec3d axis( double vistime ) const { return axis_->value(vistime); }

	void setBaseRadius( VariantFloatPtr::element_type* s ){ setBaseRadius( VariantFloatPtr( s ) ); }
	void setBaseRadius( VariantFloatPtr s ){ base_radius_ = s; }
	float baseRadius( double vistime ) const { return base_radius_->value(vistime); }

	void setTopRadius( VariantFloatPtr::element_type* s ){ setTopRadius( VariantFloatPtr( s ) ); }
	void setTopRadius( VariantFloatPtr s ){ top_radius_ = s; }
	float topRadius( double vistime ) const { return top_radius_->value(vistime); }
};

/**
 * Spheres, drawn using shaders where available
 */
class SphereObject : public SolidObject {

public:
	/// Available sphere shading methods
	enum ShadeMode{
		UNSHADED_GEOM = 0, // fixed-funtion OpenGL spheres
		SHINY, // glshovel-like shiny Christmas ornaments, mostly unused
		MATTE, // default smooth shading
		RING   // hollow O-shaped token, more circle than sphere
	};

	typedef SceneVariant<ShadeMode> ShadingVariant;
	I3_POINTER_TYPEDEFS( ShadingVariant );

protected:
	// uniform / attribute locations are set by ShaderManager
	static int CAMERA_UNIFORM;
	static int RADIUS_UNIFORM;
	static int OFFSET_ATTRIBUTE;

private:
	ShaderManager::ShaderType shaderid( ShadeMode m ){
		switch(m){
			case UNSHADED_GEOM: return ShaderManager::NONE;
			case SHINY: return ShaderManager::SHINY_SPHERE;
			case MATTE: return ShaderManager::MATTE_SPHERE;
			case RING: return ShaderManager::RING_SPHERE;
		}
		return ShaderManager::NONE;
	}

	static GLuint sphere_list;
	// shader positions of input variables
	static int cameraPos, offsetPos, radiusPos;

	void prepShader( QGLShaderProgram* program );

protected:

	VariantFloatPtr scale_;
	ShadingVariantPtr shader_;

public:
	SphereObject( float r, const vec3d& loc );

	virtual bool isVisible( double vistime );
	void drawBaseObject(double vistime, const I3Camera& i3camera);
	float cameraDistance( double vistime, const I3Camera& camera );

	void setSize( VariantFloatPtr::element_type* s ){ setSize( VariantFloatPtr( s ) ); }
	void setSize( VariantFloatPtr s ){ scale_ = s; }
	float size( double vistime ) const { return scale_->value(vistime); }

	void setShader( ShadingVariantPtr::element_type* s) { setShader( ShadingVariantPtr( s ) ); }
	void setShader( ShadingVariantPtr s ){ shader_ = s; }
	void setShader( const ShadeMode& mode ){ setShader( new SceneConstant<ShadeMode>( mode ) ); }

	virtual ShaderManager::ShaderType shader( double vistime ){ return shaderid(shader_->value(vistime)); }

	friend class ShaderManager;
};


/**
 * SceneObject representing a line to be drawn by GL_LINE_STRIP
 * with arrow head drawn by gluCylinder
 */
class ArrowObject : public SceneObject, public ColoredObject, public BaseLineObject {
	VariantVec3dPtr start_;
	VariantVec3dPtr stop_;
	float angle_;
	float head_length_;

public:
	ArrowObject( VariantVec3dPtr start,
	             VariantVec3dPtr stop,
	             float angle,
	             float head_length ) :
		start_( start ),
		stop_( stop ),
		angle_( angle ),
		head_length_( head_length )
	{}

	virtual void draw( double vistime, const I3Camera& camera );
	virtual bool isVisible( double vistime );
	virtual bool isHeadVisible( double vistime ){ return true; }
	virtual RenderPass pass(double vistime){ return getPassByAlpha(vistime); }
	virtual bool isSelectable( double vistime ){ return false; }
	virtual float cameraDistance( double vistime, const I3Camera& camera );
};

#endif /* I3_SHOVEL_RENDERING_SCENE_OBJECT_H */

