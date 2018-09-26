#include "Texture.h"
#include <QPixmap>
#include <QSize>
#include <QGLWidget>

void Texture::update( const QPixmap& pixmap,
                      const QSize& resize_to )
{
	if( texid_ ){
		glwidget_->deleteTexture( texid_ );
	}

	if( pixmap.isNull() )
		return;

	if( resize_to.isValid() ){
		pix_ = pixmap.scaled( resize_to, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
	}else{
		pix_ = pixmap;
	}

	texid_ = glwidget_->bindTexture( pix_, GL_TEXTURE_2D, format_, QGLContext::NoBindOption );
}

Texture::~Texture(){
	if( texid_ ){
		glwidget_->deleteTexture( texid_ );
	}
}
