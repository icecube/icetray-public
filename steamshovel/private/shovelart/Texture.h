#ifndef I3_SHOVEL_GL_TEXTURE_H
#define I3_SHOVEL_GL_TEXTURE_H

#include <QPixmap>
#include <QSize>

#include <boost/noncopyable.hpp>
#include "gl.h"

class QGLWidget;

/**
 * Convenience class for creating OpenGL textures out of Qt image data
 * OpenGL allocation / deallocation is handled in ctor/dtor
 */
class Texture : private boost::noncopyable {
	QGLWidget* glwidget_;
	QPixmap pix_;
	GLint format_;
	GLuint texid_;

public:
	/* Create a handle to a texture of the given pixmap */
	Texture( QGLWidget* w, GLint format = GL_RGBA ):
		glwidget_(w), format_(format), texid_(0)
	{}

	// texture resources are released on destruction
	~Texture();

	void update( const QPixmap& pixmap,
	             const QSize& resize_to = QSize() );

	void setSize( int width, int height )
	{ update( pix_, QSize(width, height) ); }

	int width() const { return pix_.width(); }
	int height() const { return pix_.height(); }

	GLuint texId() const { return texid_; }
};

#endif
