#include <iostream>
#include <sstream>
#include <limits>

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QGLShader>

#include "SceneObject.h"
#include "Camera.h"
#include "Text.h"

ImageOverlay::ImageOverlay( QGLWidget* w, const QPixmap& pixmap,
                            const QSize& resize_to ) :
	SceneOverlay( w )
{
	texture_.update( pixmap, resize_to );
}

void ImageOverlay::draw( double vistime, const I3Camera& camera){
	glColor3f(1,1,1);
	glBindTexture( GL_TEXTURE_2D, texture_.texId() );
	float x = xanchor_;// * camera.scaling_factor;
	float y = yanchor_;// * camera.scaling_factor;
	float w = texture_.width();// * camera.scaling_factor;
	float h = texture_.height();// * camera.scaling_factor;
	glBegin(GL_QUADS);
	{
		glTexCoord2f( 0, 0 );
		glVertex2f( x, y );

		glTexCoord2f( 0, 1 );
		glVertex2f( x, y + h );

		glTexCoord2f( 1, 1 );
		glVertex2f( x + w, y + h );

		glTexCoord2f( 1, 0 );
		glVertex2f( x + w, y );
	}
	glEnd();
}

ActiveImageOverlay::ActiveImageOverlay( QGLWidget* w ) :
	ImageOverlay( w, QPixmap() ), last_vistime_( -std::numeric_limits<double>::max() )
{}

void ActiveImageOverlay::draw( double vistime, const I3Camera& camera ){
	if( last_vistime_ != vistime ){
		texture_.update( redraw(vistime, camera) );
		last_vistime_ = vistime;
	}
	ImageOverlay::draw( vistime, camera );
}

void TextOverlay::draw( double vistime, const I3Camera& camera ){
	if( needs_update_ || scaling_factor_ != camera.scaling_factor ){
		scaling_factor_ = camera.scaling_factor;
		QColor c = camera.bg_color.lightnessF() > .5 ? Qt::black : Qt::white;
		QPixmap pix = makeStringPixmap( text_, c, camera.bg_color, font_, scaling_factor_ );
		if( pix.width() > 2048 || pix.height() > 2048 ){
			std::ostringstream msg;
			msg << "Texture (" << pix.width() << ", " << pix.height() << ") too large to display";
			pix = makeStringPixmap( msg.str(), c, camera.bg_color, font_, scaling_factor_ );
		}
		texture_.update( pix );
		needs_update_ = false;
	}

	applyColor( vistime );
	glBindTexture( GL_TEXTURE_2D, texture_.texId() );
	const int height = texture_.height();
	const int width = texture_.width();
	float x = xanchor_;// * camera.scaling_factor;
	float y = yanchor_;// * camera.scaling_factor;

	glBegin(GL_QUADS);
	{
		glTexCoord2f( 0, 0 );
		glVertex2f( x, y );

		glTexCoord2f( 0, 1 );
		glVertex2f( x, y + height );

		glTexCoord2f( 1, 1 );
		glVertex2f( x + width, y + height );

		glTexCoord2f( 1, 0 );
		glVertex2f( x + width, y );
	}
	glEnd();

}

void OverlayLine::draw( double vistime, const I3Camera& camera ){
	int startx = start_->xpos() + (start_->width() / 2);
	int starty = start_->ypos() + (start_->height() / 2);

	double endx, endy, endz;
	gluProject( end_.x(), end_.y(), end_.z(),
	            camera.modelviewMat, camera.projectionMat, camera.viewportVec,
	            &endx, &endy, &endz );

	endy = camera.h - endy;

	glDisable(GL_TEXTURE_2D);
	applyLineWidth(vistime, camera);
	applyStipple(vistime, camera);
	applyColor(vistime);
	glBegin(GL_LINES);
	glVertex2i( startx, starty );
	glVertex2d( endx, endy );
	glEnd();
	glEnable(GL_TEXTURE_2D);

}

inline void ColoredObject::applyColor( double vistime ){
	if( color_ ){
		QColor color = color_->value(vistime);
		glColor4f( color.redF(), color.greenF(), color.blueF(), color.alphaF() );
	}
}

void ZPlaneObject::draw( double vistime, const I3Camera& camera ){
	applyColor(vistime);
	applyLineWidth(vistime, camera);
	applyStipple(vistime, camera);
	double x2 = xwidth_/2.0;
	double y2 = ywidth_/2.0;
	// corners
	vec3d c1( xctr_ - x2, yctr_ - y2, z_);
	//vec3d c2( xctr_ + x2, yctr_ - y2, z_);
	vec3d c3( xctr_ + x2, yctr_ + y2, z_);
	//vec3d c4( xctr_ - x2, yctr_ + y2, z_);

	glBegin(GL_LINES);
	const int steps = 15;
	for( int i = 0; i <= steps; ++i ){
		double d = i * (xwidth_ / steps);
		glVertex3f( c1.x() + d, c1.y(), c1.z() );
		glVertex3f( c1.x() + d, c3.y(), c3.z() );
	}
	for( int j = 0; j <= steps; ++j ){
		double d = j * (ywidth_ / steps);
		glVertex3f( c1.x(), c1.y() + d, c1.z() );
		glVertex3f( c3.x(), c1.y() + d, c3.z() );
	}
	glEnd();
}

void TextObject::draw( double vistime, const I3Camera& camera ){
	// smoothing factor for floating '3d' text objects
	// higher numbers create larger textures but better-looking labels
	const double text_scale_factor = 3.;
	if( (loc_-camera.loc).length() < 300 ){

		if( needs_update_ ){
			QColor c = camera.bg_color.lightnessF() > .5 ? Qt::black : Qt::white;
			texture_.update( makeStringPixmap( text_, c, camera.bg_color, font_, text_scale_factor ) );
			needs_update_ = false;
		}

		glColor3f(1,1,1);
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, texture_.texId() );

		const int height = texture_.height() / (2.*text_scale_factor);
		const int width = texture_.width() / (2.*text_scale_factor);

		vec3d right = camera.right * width;
		vec3d up = vec3d::crossProduct(( camera.pivot - camera.loc ).normalized(), camera.right ) * height;
		glBegin(GL_QUADS);
		{
			glTexCoord2i( 0, 0 );
			glVertex3d( loc_.x(), loc_.y(), loc_.z() );

			vec3d v = loc_ + up;
			glTexCoord2i( 0, 1 );
			glVertex3d( v.x(), v.y(), v.z() );

			v = v + right;
			glTexCoord2i( 1, 1 );
			glVertex3d( v.x(), v.y(), v.z() );

			v = loc_ + right;
			glTexCoord2i( 1, 0 );
			glVertex3d( v.x(), v.y(), v.z() );
		}
		glEnd();
		glDisable( GL_TEXTURE_2D );
	}
}


float TextObject::cameraDistance( double vistime, const I3Camera& camera ){
	return (camera.loc-loc_).length();
}

void BaseLineObject::applyLineWidth( double vistime, const I3Camera& camera ){
	glLineWidth( lineWidth_->value(vistime) * camera.scaling_factor );
}

bool BaseLineObject::stipple_enabled_ = false;

void BaseLineObject::applyStipple( double vistime, const I3Camera& camera ){
	if (stippleScale_ > 0 ) {
		glLineStipple( stippleScale_ * camera.scaling_factor, stipplePattern_);
		if( !stipple_enabled_ ){
			glEnable(GL_LINE_STIPPLE);
			stipple_enabled_ = true;
		}
	}
	else if(stipple_enabled_){
		glDisable(GL_LINE_STIPPLE);
		stipple_enabled_ = false;
	}
}

template <typename T>
void LineObject<T>::draw( double vistime, const I3Camera& camera ){
	int len = points_.size();
	QColor color = color_->value(vistime);
	glColor4f( color.redF(), color.greenF(), color.blueF(), color.alphaF() );
	applyLineWidth( vistime, camera );
	applyStipple(vistime, camera);
	glBegin(GL_LINE_STRIP);
	for( int i = 0; i < len; ++i ){
		vec3d v = get(i,vistime);
		glVertex3f( v.x(), v.y(), v.z() );
	}
	glEnd();
	glColor3f(1,1,1);
}

template <typename T>
float LineObject<T>::cameraDistance( double vistime, const I3Camera& camera )
{
	if( points_.size() >= 2 ){
		vec3d v = get(0,vistime) - get(1,vistime);
		v.normalize();
		return camera.loc.distanceToLine( get(0,vistime), v );

	}
	return 1e5f;
}

template<>
vec3d LineObject<vec3d>::get( int idx, double vistime ){
	return points_[idx];
}

template<>
vec3d LineObject<VariantVec3dPtr>::get( int idx, double vistime ){
	return points_[idx]->value(vistime);
}


/* Template instantiations */
template class LineObject<vec3d>;
template class LineObject<VariantVec3dPtr>;


void ParticlePath::draw( double vistime, const I3Camera& camera ){
	int npoints = points_.size();

	double t0 = (npoints >= 2) ? times_[0] : std::numeric_limits<double>::max();
	if( t0 > vistime )
		return;

	applyLineWidth(vistime, camera);
	applyStipple(vistime,camera);

	// now we know there are at least two points and something will be visible
	double t_last = 0; // will be initialized on first loop iteration
	bool stop = false;
	glBegin( GL_LINE_STRIP );
	for( int i = 0; !stop && i < npoints; ++i ){
		double t = times_[i];
		vec3d pt;
		QColor col;
		if( t <= vistime ){
			t_last = t;
			pt = points_[i];
			col = colors_[i];
		}
		else{
			double x = (vistime - t_last) / ( t - t_last );
			pt = (points_[i-1] * (1.-x)) + (points_[i] * x);
			QColor c0 = colors_[i-1];
			QColor c1 = colors_[i];
			float r = (c0.redF() * (1.-x)) + (c1.redF()*x);
			float g = (c0.greenF() * (1.-x)) + (c1.greenF()*x);
			float b = (c0.blueF() * (1.-x)) + (c1.blueF()*x);
			float a = (c0.alphaF() * (1.-x)) + (c1.alphaF()*x);
			col = QColor::fromRgbF(r,g,b,a);
			stop = true;
		}
		glColor4f( col.redF(), col.greenF(), col.blueF(), col.alphaF() );
		glVertex3f( pt.x(), pt.y(), pt.z() );
	}
	glEnd();
}

bool ParticlePath::isVisible( double vistime ){
	return times_.size() && vistime >= times_[0];
}

void SolidObject::draw( double vistime, const I3Camera& camera ){
	glPushMatrix();
	vec3d loc = loc_->value(vistime);
	glTranslated( loc.x(), loc.y(), loc.z() );
	applyColor(vistime);
	drawBaseObject(vistime,camera);
	glPopMatrix();
}

CylinderObject::CylinderObject( vec3d vertex, vec3d axis, float base, float top ):
	SolidObject( vertex ),
	axis_( new SceneConstant<vec3d>(axis) ),
	base_radius_( new SceneConstant<float>(base) ),
	top_radius_( new SceneConstant<float>(top) )
{
}

bool CylinderObject::isVisible( double vistime ){
	if( base_radius_->value(vistime) > 0 || top_radius_->value(vistime) > 0 ){
		return color_->value(vistime).alpha() > 0.;
	}
	return false;
}

float CylinderObject::cameraDistance( double vistime, const I3Camera& camera ){
	return (camera.loc - loc_->value(vistime)).length();
}

void CylinderObject::drawBaseObject(double vistime, const I3Camera& camera){
	//vec3d vertex = loc_->value(vistime);
	vec3d axis = axis_->value(vistime);
	vec3d axis_norm = axis.normalized();
	vec3d cross = vec3d::crossProduct( axis_norm, vec3d(0,0,1) );
	if( cross.length() > .00001 ){
		// determine rotation with respect to +z, which is where gluCylinder will put our geometry
		double dot = vec3d::dotProduct( axis_norm ,vec3d(0,0,1) );
		double a = acos( dot );
		a *= 180. / M_PI;
		glRotated( -a, cross.x(), cross.y(), cross.z() );
	}
	else if( axis_norm.z() < 0 ){
		// The cylinder is pointing at -z
		glRotated( 180, 1, 0, 0 );
	}


	double height = axis.length();
	double baserad = base_radius_->value(vistime);
	double toprad = top_radius_->value(vistime);

	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	gluCylinder( camera.gl_qobj, baserad, toprad, height, 256, 1 );
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
}

void ArrowObject::draw( double vistime, const I3Camera& camera ){
	const vec3d p0 = start_->value(vistime);
	const vec3d p1 = stop_->value(vistime);
	const vec3d vec = p1 - p0;
	const vec3d dir = vec.normalized();
	const bool draw_head = isHeadVisible( vistime );

	const double cone_height = std::min( (const double ) vec.length(),
	                                     draw_head ? double(head_length_) : 0.0 );
	const vec3d base_point = p1 - cone_height * dir;

	// draw the line
	applyColor( vistime );
	applyLineWidth( vistime, camera );
	applyStipple( vistime, camera );
	glBegin(GL_LINE_STRIP);
	glVertex3f( p0.x(), p0.y(), p0.z() );
	glVertex3f( base_point.x(), base_point.y(), base_point.z() );
	glEnd();

	if( !draw_head )
		return;

	// draw cone
	glPushMatrix();
	glTranslated( base_point.x(), base_point.y(), base_point.z() );
	const vec3d cross = vec3d::crossProduct( dir, vec3d(0,0,1) );
	if( cross.length() > .00001 ){
		// determine rotation with respect to +z, which is where gluCylinder will put our geometry
		double dot = vec3d::dotProduct( dir, vec3d(0,0,1) );
		double a = acos( dot );
		a *= 180. / M_PI;
		glRotated( -a, cross.x(), cross.y(), cross.z() );
	}
	else if( dir.z() < 0 ){
		// The cylinder is pointing at -z
		glRotated( 180, 1, 0, 0 );
	}
	const double baserad = std::tan( angle_) * cone_height;
	glDisable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	gluCylinder( camera.gl_qobj, baserad, 0.0, cone_height, 256, 1 );
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
	glPopMatrix();
}

float ArrowObject::cameraDistance( double vistime, const I3Camera& camera )
{
	const vec3d p0 = start_->value(vistime);
	const vec3d p1 = stop_->value(vistime);
	vec3d v = p1 - p0;
	v.normalize();
	return camera.loc.distanceToLine( p0, v );
}

bool ArrowObject::isVisible( double vistime ){
	const vec3d p0 = start_->value(vistime);
	const vec3d p1 = stop_->value(vistime);
	return (p1 - p0).length() > 0.0;
}

static
QGLShaderProgram* setupShader( QString vertex_path, QString fragment_path ){

	QGLShaderProgram* program = new QGLShaderProgram();
	bool okay;
	okay = program->addShaderFromSourceFile( QGLShader::Vertex, vertex_path );
	QString log = program->log();
	if( log.length() ){
		log_info_stream( "Log from vertex shader:\n" << qPrintable(log) );
		log = "";
	}
	okay = okay && program->addShaderFromSourceFile( QGLShader::Fragment, fragment_path );
	log = program->log();
	if( log.length() ){
		log_info_stream( "Log from fragment shader:\n" << qPrintable(log) );
		log = "";
	}
	if( !okay ){
		delete program;
		program = NULL;
		log_error( "Could not prepare shader" );
	}

	return program;

}

void ShaderManager::setupSphereShader( QString fragment_path, bool& attrib_locs_already_set ){
	// All sphere shaders have the same vertex shader with the same inputs.
	// We want to ensure that all their locations are identical, since the input structure is the same in each shader.
	// We assume they will be the same; asserts verify this.
	static const QString shader_path( ":/resources/shader/sphere_" );
	QGLShaderProgram* program = setupShader( shader_path+"billboard.vert", shader_path+fragment_path );
	if( !program ){
		log_warn_stream( "Cannot use " << qPrintable(fragment_path) << " shader; it will use the fallback renderer." );
		programs.push_back(NULL);
		return;
	}

	if( !attrib_locs_already_set ){
		// this is the first sphere shader, so update the locations
		program->link();
		SphereObject::CAMERA_UNIFORM = program->uniformLocation( "camera" );
		SphereObject::RADIUS_UNIFORM = program->uniformLocation( "radius" );
		SphereObject::OFFSET_ATTRIBUTE = program->attributeLocation( "offset" );
		attrib_locs_already_set = true;
	}
	else{
		// other sphere shaders have gone before, so reuse their uniform locations, asserting that they're
		// the same as the other shaders.  Attribute locations can be forced, but only before linking.
		program->bindAttributeLocation( "offset", SphereObject::OFFSET_ATTRIBUTE );
		program->link();
		assert( SphereObject::CAMERA_UNIFORM == program->uniformLocation( "camera" ) );
		assert( SphereObject::RADIUS_UNIFORM == program->uniformLocation( "radius" ) );
	}

	programs.push_back(program);
}

std::vector<QGLShaderProgram*> ShaderManager::programs;
int ShaderManager::active_program = -1;

#ifndef EBENEZER_SCROOGE
#include <QDate>
#include <unistd.h>
#endif

void ShaderManager::createShaders(){

	// don't initialize twice
	if( programs.size() )
		return;

	if( QGLShaderProgram::hasOpenGLShaderPrograms() ){
		bool attrib_locs_set = false; // will be set true by whichever call succeeds first
		setupSphereShader("shiny.frag", attrib_locs_set );
		setupSphereShader("matte.frag", attrib_locs_set );
		setupSphereShader("cheerio.frag", attrib_locs_set );
	}
	else{
		log_warn( "No graphics shaders available on this system; using simplified graphics" );
		programs.push_back(NULL);
		programs.push_back(NULL);
		programs.push_back(NULL);
	}
	active_program = -1;

#ifndef EBENEZER_SCROOGE
	QDate today = QDate::currentDate();
	int day = today.day();
	if( today.month() == 12 && day >= 24 && day <= 26 ){
		if( isatty( 0 ) && isatty( 1 ) ){
			char response;
			std::cout << "Detected Christmas.  Activate Christmas? (y/n)" << std::endl;
			std::cin >> response;
			if( response == 'y' || response == 'Y' ){
				std::cout << "Jolly!" << std::endl;
				QGLShaderProgram* swap = programs[1];
				programs[1] = programs[0];
				programs[0] = swap;
			}
			else{
				std::cout << "Humbug!" << std::endl;
			}
		}
	}

#endif
}

QGLShaderProgram* ShaderManager::ensureBound( ShaderType t ){
	QGLShaderProgram* p = currentlyBound();
	if( active_program != t ){
		release();
		active_program = t;
		p = currentlyBound();
		if( p ) p->bind();
	}
	return p;
}

void ShaderManager::release(){
	QGLShaderProgram* p = currentlyBound();
	if( p ){
		p->release();
	}
	active_program = -1;
}

int SphereObject::CAMERA_UNIFORM;
int SphereObject::RADIUS_UNIFORM;
int SphereObject::OFFSET_ATTRIBUTE;

bool SphereObject::isVisible( double vistime ){
	if( scale_->value(vistime) )
		return color_->value(vistime).alpha() > 0.;
	return false;
}

SphereObject::SphereObject( float size, const vec3d& loc ) :
	SolidObject(), shader_( new SceneConstant<ShadeMode>( MATTE ) )
{
	setSize( VariantFloatPtr(new SceneConstant<float>(size)) );
	setLocation( VariantVec3dPtr(new SceneConstant<vec3d>(loc)) );
}

void SphereObject::drawBaseObject(double vistime, const I3Camera& i3camera) {

	float scale = scale_->value(vistime);
	glScalef( scale, scale, scale );

	if( sphere_list == 0 ){

		/* Prepare backup sphere mesh first */
		sphere_list = glGenLists(1);
		glNewList( sphere_list, GL_COMPILE );
		//glutSolidSphere(1,12,12);
		gluSphere( i3camera.gl_qobj, 1, 12, 12 );
		glEndList();

	}

	QGLShaderProgram* program = ShaderManager::currentlyBound();
	if( !program ) {
		glCallList( sphere_list );
		return;
	}

	//std::cerr << cameraPos <<", " << radiusPos << std::endl;
	program->setUniformValue( CAMERA_UNIFORM, (i3camera.loc - loc_->value(vistime)).normalized() );
	GLfloat radius = scale_->value(vistime);
	program->setUniformValue( RADIUS_UNIFORM, radius );

	glBegin( GL_QUADS );
	program->setAttributeValue( OFFSET_ATTRIBUTE, vec2d(-1, -1) );
	glVertex3i( 0, 0, 0 );
	program->setAttributeValue( OFFSET_ATTRIBUTE, vec2d(-1, 1) );
	glVertex3i( 0, 0, 0 );
	program->setAttributeValue( OFFSET_ATTRIBUTE, vec2d(1, 1) );
	glVertex3i( 0, 0, 0 );
	program->setAttributeValue( OFFSET_ATTRIBUTE, vec2d(1, -1) );
	glVertex3i( 0, 0, 0 );
	glEnd();

}

float SphereObject::cameraDistance( double vistime, const I3Camera& camera ){
	return fabs((loc_->value(vistime) - camera.loc).length()) - scale_->value(vistime);
}


GLuint SphereObject::sphere_list = 0;
int SphereObject::cameraPos = 0;
int SphereObject::offsetPos = 0;
int SphereObject::radiusPos = 0;
