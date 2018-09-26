#ifndef I3_SHOVEL_RENDERING_CAMERA_H
#define I3_SHOVEL_RENDERING_CAMERA_H

#include <cmath>
#include <QtGlobal>
#include "graphicsmath.h"
#include "gl.h"

/**
 * The camera, or viewing point, in a 3D shovelart scene
 */
class I3Camera
{

public:
	/// camera's location in space
	vec3d loc;
	/// camera's focus point, around which it rotates
	vec3d pivot;
	/// unit vector pointing to the right of camera direction
	vec3d right;
	/// field of vision angle
	double fovy;
	/// width and height of the rendered view
	double w, h;
	/// near and far z-clipping planes
	double near, far;

	// rendering settings
	/// currently active shader
	int shader;
	/// perspective vs. orthogonal projection flag
	bool perspective;
	/// DPI scaling factor
	double scaling_factor;
	/// Current background color
	QColor bg_color;

	/// quadric evaluator for the current GL context
	GLUquadricObj* gl_qobj;

	// stash for 3D projection parameters
	mutable GLint viewportVec[4];
	mutable GLdouble projectionMat[16];
	mutable GLdouble modelviewMat[16];

	I3Camera() :
		fovy(45.), w(100.), h(100.), near(10.0), far(1000000.0)
		, shader(2)
		, perspective(true)
		, scaling_factor(1.0)
		, gl_qobj(NULL)
	{
		reset();
	}

	/// Reset the camera to its default position
	void reset(){
		loc = vec3d(1000.,1000.,1000.);
		pivot = vec3d(0., 0., 0.);
		right = vec3d::crossProduct( -loc, vec3d( 0, 0, 1 ) ).normalized();
	}

	vec3d upVec() const {
		return vec3d::crossProduct( right, pivot-loc ).normalized();
	}

	vec3d forwardVec() const {
		return loc - pivot;
	}

	void setAspect( int width, int height )
	{
		w = width;
		h = height;
		glGetIntegerv( GL_VIEWPORT, viewportVec );
	}

	void doProjection() const {
		if( perspective ){
			gluPerspective( fovy, w/h, near, far );
		}
		else{
			double d = ((loc-pivot).length()) / 2.0;
			double yfov = d * (h/w);
			double xfov = d;
			glOrtho( -xfov,xfov,-yfov,yfov, near, far );
		}
		glGetDoublev( GL_PROJECTION_MATRIX, projectionMat );
	}

	void doLookAt() const {
		vec3d up = upVec();
		gluLookAt( loc.x(), loc.y(), loc.z(),
		           pivot.x(), pivot.y(), pivot.z(),
		           up.x(), up.y(), up.z() );
		glGetDoublev( GL_MODELVIEW_MATRIX, modelviewMat );
	}

	void doScreenOrtho() const {
		glOrtho( 0, w, h, 0, -1, 10 );
	}

	/**
	 * Perform a standard rotation on the basis of dragging the cursor from p1 to p2
	 * x-axis motion generates rotations around +Z, and y-axis motion rotates around
	 * the camera's right vector; rotations are relative to the camera's pivot
	 */
	void rotate( QPoint p1, QPoint p2 )
	{
		const static vec3d z(0,0,1);
		vec3d tvec = loc - pivot;
		tvec = rotate_around( tvec, right, p1.y() - p2.y() );
		tvec = rotate_around( tvec, z, p1.x() - p2.x() );
		loc = tvec + pivot;
		right = rotate_around( right, z, p1.x() - p2.x() );
	}

	/**
	 * Rotate theta radians around +Z
	 */
	void rotate( double theta ){
		const static vec3d z(0,0,1);
		vec3d tvec = loc - pivot;
		tvec = rotate_around( tvec, z, theta );
		loc = tvec + pivot;
		right = rotate_around( right, z, theta );
	}

	void zoom( int delta )
	{
		vec3d tvec = loc - pivot;
		double scale = 1.0 - ((double)delta) / 1200.0;
		tvec *= scale;
		loc = tvec + pivot;
	}

	void moveBy( int dx, int dy )
	{
		vec3d up = upVec();
		vec3d delta = (dx * -right) + (dy * up);
		pivot += delta;
		loc += delta;
	}

	/**Move the camera to a new location, keeping the pivot point the same */
	void setLoc( double x, double y, double z ){
		vec3d new_loc(x,y,z);
		// avoid changing the right vector if we're z-axis-aligned with the pivot
		// FIXME: this check implicitly assumes the pivot exists along z=0
		vec3d norm = new_loc.normalized();
		if( !(qFuzzyCompare( (double) fabs(norm.z()), 1.0 )) ){
			right = vec3d::crossProduct( pivot-new_loc, vec3d(0,0,1) ).normalized();
		}
		loc = new_loc;
	}

	/** Manually set the right vector to the given values
	 *
	 * This is an ugly workaround for the case when setLoc is called for a position that
	 * is z-axis-aligned with the pivot point.
	 */
	void setOrientation( double x, double y, double z ){
		right = vec3d(x,y,z).normalized();
	}
};

#endif /* I3_SHOVEL_RENDERING_CAMERA_H */
