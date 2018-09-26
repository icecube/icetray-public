#include <iostream>
#include <QGLFramebufferObject>
#include <QImageWriter>
#include <QColorDialog>
#include <QStackedWidget>
#include <QLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <boost/foreach.hpp>

#include "shovelart/Scene.h"
#include "shovelart/SceneObject.h"
#include "shovelart/Artist.h"
#include "shovelart/Scenario.h"
#include "FileService.h"
#include "SteamshovelApp.h"
#include "I3GLWidget.h"
#include "moc_I3GLWidget.cpp"

I3GLWidget::I3GLWidget( QWidget* parent ) :
	QGLWidget( parent ), camera_(new I3Camera), scene_(new Scene),
	scenario_( new Scenario( this, scene_ ) ),
	dragging_overlay_(NULL),
	camera_locked_( false ),
	grandparent_( NULL ),
	nframes_(-2)
{
	scene_->setState( this, &time_window_ );
	QObject::connect( this, SIGNAL(cameraMoved()), this, SLOT(updateGL()) );
	QObject::connect( scenario_, SIGNAL(outputsChanged()), this, SLOT(updateGL()) );
	QObject::connect( &spin_timer_, SIGNAL(timeout()), this, SLOT(spinStep()) );
	setFocusPolicy( Qt::ClickFocus );
	camera_->bg_color = QColor::fromRgbF(.15,.15,.15,1.0);
	ref_dpi_ = physicalDpiX();
}


QString I3GLWidget::getCameraPosStr() const{
	QString str("%1, %2, %3");
	str = str.arg(camera_->loc.x()).arg(camera_->loc.y()).arg(camera_->loc.z());
	return str;
}

void I3GLWidget::mouseMoveEvent( QMouseEvent* ev )
{
	int dx = ev->pos().x() - mouse_.x();
	int dy = ev->pos().y() - mouse_.y();
	if( dragging_overlay_ ){
		if( (ev->buttons() & Qt::RightButton) || (ev->modifiers() & Qt::MetaModifier) ){
			dragging_overlay_->resize( dx, dy );
		}
		else{
			dragging_overlay_->move( dx, dy );
		}
		Q_EMIT cameraMoved();
	}
	else if( !camera_locked_ ) {
		if( ev->modifiers() & Qt::ShiftModifier ){
			camera_->moveBy( dx, dy );
		}
		else{
			camera_->rotate( mouse_, ev->pos() );
		}
		Q_EMIT cameraMoved();
	}
	mouse_ = ev->pos();
}

void I3GLWidget::mousePressEvent( QMouseEvent* ev )
{
	startMouse_ = mouse_ = ev->pos();

	if( scene_ ){

		boost::any pick = scene_->pick( time_window_.current(), *camera_.get(), ev->x(), ev->y() );
		try{
			dragging_overlay_ = boost::any_cast<SceneOverlay*>( pick );
			log_debug_stream( "Grabbed an overlay" );
			return;
		}
		catch( const boost::bad_any_cast& ){}
		log_debug_stream( "Click without selection" );
	}
}

void I3GLWidget::mouseReleaseEvent( QMouseEvent* ev ){
	if( scene_ ){
		if( dragging_overlay_ && ((ev->button() == Qt::RightButton) || ( ev->modifiers() & Qt::MetaModifier )) ){
			// inform the Scene that an overlay has potentially changed size
			ArtistPtr source = scene_->overlaySource( dragging_overlay_ );
			scenario_->forceRegen(source);
		}
		else if( startMouse_ == ev->pos() ){
			boost::any pick = scene_->pick( time_window_.current(), *camera_.get(), ev->x(), ev->y() );
			try{
				OMKey key = boost::any_cast<OMKey>( pick );
				log_info_stream( "Selected DOM: " << key );
				if( !(ev->modifiers() & Qt::ShiftModifier) ){
					selected_doms_.clear();
				}
				OMKeySet::iterator ki;
				if( (ki=std::find( selected_doms_.begin(), selected_doms_.end(), key )) != selected_doms_.end() ){
					selected_doms_.erase(ki);
				}
				else{
					selected_doms_.push_back( key );
				}
				scenario_->selectOMKeys(selected_doms_);
				Q_EMIT cameraMoved();
				return;
			}
			catch( const boost::bad_any_cast& ){}
		}
	}
	dragging_overlay_ = NULL;
	startMouse_ = QPoint(-1,-1);
}

void I3GLWidget::wheelEvent( QWheelEvent* ev )
{
	camera_->zoom( ev->delta() );
	Q_EMIT cameraMoved();
}

void I3GLWidget::keyPressEvent( QKeyEvent* ev ){
	if( ev->key() == Qt::Key_1 ){
		camera_->shader = 0;
		log_debug( "Shader disabled" );
		Q_EMIT cameraMoved();
	}
	else if( ev->key() == Qt::Key_2 ){
		camera_->shader = 1;
		log_debug( "Shader set to 1" );
		Q_EMIT cameraMoved();
	}
	else if( ev->key() == Qt::Key_0 ){
		if( nframes_ == -2 ){
			nframes_ = -1;
			Q_EMIT statusMessage( "FPS ---", 10000 );
		}
		else{
			nframes_ = -2;
			Q_EMIT statusMessage( QString(), 0 );
		}
	}
	else if( ev->key() == Qt::Key_Equal || ev->key() == Qt::Key_Plus ){
		camera_->zoom( 10 );
		Q_EMIT cameraMoved();
	}
	else if( ev->key() == Qt::Key_Minus || ev->key() == Qt::Key_Underscore ){
		camera_->zoom( -10 );
		Q_EMIT cameraMoved();
	}
	// toggle tv mode to off if it is currently enabled
	else if( grandparent_ && ev->key() == Qt::Key_Escape ){
		toggleTVMode();
	}
	else
		QGLWidget::keyPressEvent(ev);
}

void I3GLWidget::initializeGL()
{
	log_trace("Initialized GL widget");
	glClearColor( camera_->bg_color.redF(),
	              camera_->bg_color.greenF(),
	              camera_->bg_color.blueF(),
	              camera_->bg_color.alphaF() );

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_MULTISAMPLE_ARB);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	// quadrics, used for drawing cylinders and fallback spheres
	camera_->gl_qobj = gluNewQuadric();
	gluQuadricDrawStyle( camera_->gl_qobj, GLU_FILL );
	gluQuadricNormals( camera_->gl_qobj, GLU_SMOOTH );

	// lighting is mostly disabled.  But cylinders are drawn with lighting, so the following settings are applied
	// color both inside and outside of objects
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

	// create a light and some materials; again, these are only used when GL_LIGHTNING is turned on,
	// which is currently only within cylinder rendering
	GLfloat light0_posn[4] = {0.0, 0.0, 0.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light0_posn);
	glEnable(GL_LIGHT0);

	GLfloat diffuse_coeff[4] = {0.3, 0.3, 0.3, 1.0};
	GLfloat spec_coeff[4] = {0.010, 0.010, 0.010, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE, diffuse_coeff);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, spec_coeff);
	glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,12);

	ShaderManager::createShaders();

}
void I3GLWidget::resizeGL(int w, int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	camera_->setAspect(w,h);
}

static QVector2D sceneProject( I3Camera& cam, vec3d pt ){
	double x, y, z;
	gluProject( pt.x(), pt.y(), pt.z(),
	            cam.modelviewMat, cam.projectionMat, cam.viewportVec,
	            &x, &y, &z );
	return QVector2D( x, y );
}

void I3GLWidget::boxSelectedDOMs(){

	for( OMKeySet::const_iterator omkey = selected_doms_.begin();
	     omkey != selected_doms_.end(); ++omkey )
	{
		OMKey key = *omkey;
		std::vector<SphereObject*> v =
			scene_->selectionsByKey<SphereObject>( key, time_window_.current() );
		if( v.size() == 0 ){
			log_trace_stream( "Selected DOM is not selectable in scene: " << key );
			continue;
		}

		// assume all the spheres for a given dom have the same center
		vec3d loc = v[0]->location(time_window_.current());
		float size = 0;
		// find the largest sphere for this dom
		for( std::vector<SphereObject*>::iterator j = v.begin(); j!=v.end(); ++j){
			size = std::max( size, (*j)->size( time_window_.current() ) );
		}

		// now use the camera to determine the screen space bounding box for this DOM
		vec3d up = camera_->upVec() * size;
		vec3d camright = camera_->right * size;

		QVector2D left = sceneProject( *camera_, loc - camright );
		QVector2D right = sceneProject( *camera_, loc + camright );
		QVector2D top = sceneProject( *camera_, loc + up );
		QVector2D bot = sceneProject( *camera_, loc - up );
		int h = camera_->h;

		const static int space = 2;
		glBegin(GL_LINE_STRIP);
		glVertex2f( left.x() - space, h-top.y() - space );
		glVertex2f( right.x() + space, h-top.y() - space );
		glVertex2f( right.x() + space, h-bot.y() + space );
		glVertex2f( left.x() - space, h-bot.y() + space );
		glVertex2f( left.x() - space, h-top.y() - space );
		glEnd();
	}
}

void I3GLWidget::paintGL()
{
	GLenum errCode;
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	if( (errCode = glGetError()) != GL_NO_ERROR ){
		log_debug( "paintGL() called in error state (possible known "
		           "OSX issue, skipping this frame)\nGL error: %s",
		           (const char*)gluErrorString(errCode) );
		return;
	}

	if( scene_ ){
		scene_->draw( time_window_.current(), *camera_.get() );
		glColor3f(1,0,0);
		glLineWidth(1.0);
		boxSelectedDOMs();
	}
	glFlush();

	while( (errCode = glGetError()) != GL_NO_ERROR ){
		log_error( "GL error while rendering: %s",
		           (const char*)gluErrorString(errCode) );
	}

	if( nframes_ >= -1 ){
		++nframes_;
		if( nframes_ == 0 )
			lastTime_ = std::clock();

		if( nframes_ > 100 ){
			const double dt = double(std::clock() - lastTime_) / CLOCKS_PER_SEC;
			QString s = QString("FPS %1").arg( int(double(nframes_) / dt) );
			Q_EMIT statusMessage( s, int(2e-3 * dt) );
			nframes_ = -1;
		}
	}
}

void I3GLWidget::setTimeWindow( const TimeWindow& win ){
	time_window_ = win;
	Q_EMIT cameraMoved();
}

void I3GLWidget::setCameraPivot( double x, double y, double z ){
	vec3d d(x,y,z);
	camera_->pivot = d;
	camera_->loc += d;
	Q_EMIT cameraMoved();
}

void I3GLWidget::setCameraPivot( const vec3d& v ){
	camera_->pivot = v;
	camera_->loc += v;
	Q_EMIT cameraMoved();
}

void I3GLWidget::setCameraLoc( double x, double y, double z ){
	camera_->setLoc(x,y,z);
	Q_EMIT cameraMoved();
}

void I3GLWidget::focusCameraOn( const vec3d& v ){
	vec3d view = camera_->loc - camera_->pivot;
	camera_->pivot = v;
	camera_->loc = v + view;
	Q_EMIT cameraMoved();
}

void I3GLWidget::resetCamera(){
	camera_->reset();
	Q_EMIT cameraMoved();
}

const vec3d& I3GLWidget::getCameraLoc() const{
	return camera_->loc;
}

const vec3d& I3GLWidget::getCameraOrientation() const{
	return camera_->right;
}

const vec3d& I3GLWidget::getCameraPivot() const {
	return camera_->pivot;
}

void I3GLWidget::setCameraLoc( const vec3d& v ){
	camera_->loc = v;
	Q_EMIT cameraMoved();
}

void I3GLWidget::setCameraOrientation( const vec3d& v ){
	camera_->right = v;
	Q_EMIT cameraMoved();
}

void I3GLWidget::setCameraOrientation( double x, double y, double z ){
	camera_->setOrientation(x,y,z);
	Q_EMIT cameraMoved();
}

void I3GLWidget::setPerspective( bool perspective ){
	camera_->perspective = perspective;
	Q_EMIT cameraMoved();
	Q_EMIT perspectiveChanged( perspective );
}

void I3GLWidget::setCameraLock( bool lock ){
	camera_locked_ = lock;
	Q_EMIT cameraLockChanged( lock );
}

void I3GLWidget::rotateCamera( double a ){
	camera_->rotate( a );
	Q_EMIT cameraMoved();
}

void I3GLWidget::spinStep(){
	camera_->rotate( 0.18 );
	updateGL();
}

QColor I3GLWidget::backgroundColor()const{
	return camera_->bg_color;
}

void I3GLWidget::setBackgroundColor( QColor c ){
	if( !c.isValid() )
		return;
	c.setAlphaF(1.0f);
	camera_->bg_color = c;
	glClearColor( c.redF(), c.greenF(), c.blueF(), 1.0f );
	scenario_->forceRegen();
	Q_EMIT cameraMoved();
}

void I3GLWidget::doSetBackgroundColor(){
	QColor c = QColorDialog::getColor( camera_->bg_color, this );
	setBackgroundColor( c );
}

void I3GLWidget::screenshot_engine( int xdim, int ydim, const std::string& filename,
                                    int dpi, double scaling_factor, double gamma_factor )
{
	log_debug_stream( "screeshot_engine(" << xdim << ", " << ydim << ", "
	                  << filename << ", " << dpi << ", " << scaling_factor << ", "
	                  << gamma_factor << ")");

	if( dpi < 0 ){
		log_error( "Negative dpi %d is not sensible", dpi );
		return;
	}

	if( scaling_factor <= 0. ){
		log_error( "Scaling factor %e is not sensible", scaling_factor );
		return;
	}

	QImageWriter wr;
	// Set gamma correction in PNG format.
	// It is an open question whether to set this or not.
	// This seems to be a big mess: According to Steve Jackson,
	// viewers on Macs in 2013 needed a value of 1.0 / 2.2 for
	// good results. In 2015, results seem more consistent on
	// Mac and Linux systems if the gamma correction is not set.
	// More info here: https://hsivonen.fi/png-gamma
	if( gamma_factor != 1.0 )
		wr.setGamma( gamma_factor );

	QImage image;
	image = screenshotData( xdim, ydim, scaling_factor );

	if( dpi ){
		const static double in_per_m = 39.37007874;
		image.setDotsPerMeterX( in_per_m * dpi );
		image.setDotsPerMeterY( in_per_m * dpi );
	}

	wr.setFileName( filename.c_str() );
	wr.write( image );
	Q_EMIT cameraMoved();
}

QSize I3GLWidget::maxScreenshotDims() const {
	GLint maxdim[2];
	glGetIntegerv( GL_MAX_VIEWPORT_DIMS, maxdim );
	return QSize( maxdim[0], maxdim[1] );
}

QImage I3GLWidget::createScreenshotData( int xdim, int ydim, QGLFramebufferObject& fbuffer ){
	QImage ret;
	log_debug("Taking a screenshot of size %u x %u", xdim, ydim );

	if( !fbuffer.bind() ){
		log_error( "Could not bind frame buffer" );
	}

	resizeGL(xdim,ydim);
	paintGL();
	glFinish();

	// grab the raw image
	QImage screen = fbuffer.toImage();

	// use a second image to work around a formatting problem:
	//https://bugreports.qt-project.org/browse/QTBUG-27329>
	QImage argb( screen.bits(), screen.width(), screen.height(), QImage::Format_ARGB32 );

	// suppress the alpha channel so the resulting image does not display as oddly dim
	ret = argb.convertToFormat( QImage::Format_RGB32 );

	if( !fbuffer.release() ){
		log_error( "Could not release frame buffer" );
	}
	return ret;
}

I3GLWidget::ScreenshotContext::ScreenshotContext( I3GLWidget* w, int width, int height,
                                                  double scaling_factor ) :
	w_(w), xdim(width), ydim(height)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	old_xdim = viewport[2];
	old_ydim = viewport[3];

	QGLFramebufferObjectFormat fmt;
	fmt.setAttachment( QGLFramebufferObject::Depth );
	buf = new QGLFramebufferObject( xdim, ydim, fmt );

	w->camera_->scaling_factor = scaling_factor;
	scaling = new Scene::WithOverlaysScaled( w->scene_, w->scenario_, scaling_factor );

	// attempt to resize to detect problems
}

I3GLWidget::ScreenshotContext::~ScreenshotContext(){
	w_->resizeGL( old_xdim, old_ydim );
	delete buf;
	delete scaling;
	w_->camera_->scaling_factor = 1.0;
}

QImage I3GLWidget::screenshotData( const ScreenshotContext& context ){
	WithRotationDisabled disable( this );
	QImage ret;

	// use current screen size as a default if either dimension is zero
	bool use_current_size = (context.xdim <= 0 || context.ydim <= 0 );
	bool has_fbuffers = QGLFramebufferObject::hasOpenGLFramebufferObjects();
	if( !use_current_size && !has_fbuffers ){
		log_warn( "OpenGL lacks frame buffers, so I can only take a screenshot of the size of the current GL frame" );
		use_current_size = true;
	}

	if( !use_current_size ){
		QSize maxDim = maxScreenshotDims();
		if( maxDim.width() < context.xdim || maxDim.height() < context.ydim){
			log_warn( "Epic screenshot size request exceeds OpenGL viewport max (%d,%d).  Using the current frame size instead.", maxDim.width(), maxDim.height() );
			use_current_size = true;
		}
	}

	GLint viewport[4];
	if( !use_current_size ){
		glGetIntegerv(GL_VIEWPORT, viewport);
		log_debug_stream( "Viewport: " << viewport[2] << " " << viewport[3] );
		if( (viewport[2] == context.xdim) && (viewport[3] == context.ydim) ){
			// the user happened to request the current size
			use_current_size = true;
		}
	}

	// fast path: no dimension requested, or dimension matches existing size
	if( use_current_size )
	{
		log_debug( "Taking a quick screenshot" );
		ret = grabFrameBuffer( false );
	}
	else{
		QGLFramebufferObjectFormat fmt;
		fmt.setAttachment( QGLFramebufferObject::Depth );
		QGLFramebufferObject fbuffer( context.xdim, context.ydim, fmt );
		ret = createScreenshotData( context.xdim, context.ydim, fbuffer );
		resizeGL( viewport[2], viewport[3] );
	}
	return ret;
}

QImage I3GLWidget::screenshotData( int xdim, int ydim, double scaling_factor ){

	ScreenshotContext c( this, xdim, ydim, scaling_factor );
	return screenshotData( c );
}

void I3GLWidget::toggleTVMode(){
	// This widget always lives inside an otherwise empty container widget to
	// avoid problems with reparenting a QGLWidget. Setting TV mode works by
	// nulling out the grandparent widget to make the parent a top-level window,
	// then showing that window as full screen. The grandparent pointer is cached
	// to reverse this switch later on.
	QWidget* parent = qobject_cast<QWidget*>( this->parent() );
	if( grandparent_ ){
		// If grandparent_ is non-null, then it is the grandparent widget as it
		// appeared before TV mode was enabled.
		parent->showNormal();
		grandparent_->insertWidget(0, parent);
		grandparent_->setCurrentIndex(0);
		grandparent_ = NULL;
	}
	else{
		// grandparent_ is null, so TV mode is not currently enabled.  Turn it on.
		grandparent_ = qobject_cast< QStackedWidget* >( parent->parent() );
		grandparent_->removeWidget( parent );
		parent->setParent( NULL );
		parent->showFullScreen();
		setFocus();
	}
}

void I3GLWidget::toggleSpin(bool go){
	if( !go )
		spin_timer_.stop();
	else
		spin_timer_.start(20);
}

bool I3GLWidget::spinEnabled() const {
	return spin_timer_.isActive();
}

QString I3GLWidget::getDebugInfo() const {
	QString msg = QString( "Physical DPI x=%1 y=%2, Logical DPI x=%3 y=%4\n" ).arg( physicalDpiX() ).arg( physicalDpiY() )
	                                                                          .arg( logicalDpiX() ).arg( logicalDpiY() );
	GLdouble range[2];
	glGetDoublev( GL_ALIASED_LINE_WIDTH_RANGE, range );

	msg += QString( "Line width range = (%1, %2)\n" ).arg( range[0] ).arg( range[1] );
	QSize md = maxScreenshotDims();
	msg += QString( "Est. max screenshot size = (%1, %2)\n" ).arg( md.width() ).arg( md.height() );

	return msg;

}

void I3GLWidget::screenshotDpi( int dpi, const std::string& name, double gamma_factor ){
	const double factor = double(dpi) / ref_dpi_;
	screenshot_engine( width() * factor, height() * factor, name,
	                   dpi, factor, gamma_factor );
}

void I3GLWidget::screenshotInches( double xsize, double ysize, int dpi,
                                   const std::string& name, double gamma_factor ){
	return screenshot_engine( xsize * dpi, ysize * dpi, name,
	                          dpi, double(dpi) / ref_dpi_, gamma_factor );
}
