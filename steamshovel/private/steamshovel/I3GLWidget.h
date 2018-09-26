#ifndef I3_SHOVEL_GLWIDGET_H
#define I3_SHOVEL_GLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <QColor>

#include <icetray/I3Logging.h>

#include "shovelart/gl.h"
#include "shovelart/graphicsmath.h"
#include "shovelart/OMKeySet.h"
#include "shovelart/Camera.h"
#include "shovelart/timewindow.h"
#include "shovelart/Scene.h"

#include <ctime>

class Scenario;
class SceneOverlay;
class QGLFramebufferObject;
class QStackedWidget;

class I3GLWidget : public QGLWidget
{

  Q_OBJECT ;
  SET_LOGGER( "I3GLWidget" );

  Q_PROPERTY( Scenario* scenario READ getScenario );
  // vec3d properties: we have to name their original Qt type here to keep Qt's meta system happy
  Q_PROPERTY( QVector3D cameraPivot READ getCameraPivot WRITE setCameraPivot );
  Q_PROPERTY( QVector3D cameraOrientation READ getCameraOrientation WRITE setCameraOrientation );
  Q_PROPERTY( QVector3D cameraLoc READ getCameraLoc WRITE setCameraLoc );
  Q_PROPERTY( bool cameraLock READ cameraLocked WRITE setCameraLock );
  Q_PROPERTY( bool perspectiveView READ hasPerspective WRITE setPerspective );
  Q_PROPERTY( QString debugInfo READ getDebugInfo );
  Q_PROPERTY( int referenceDpi READ referenceDpi WRITE setReferenceDpi );
  Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor );


public:

	I3GLWidget( QWidget* parent = NULL );

	void mouseMoveEvent( QMouseEvent* ev );
	void mousePressEvent( QMouseEvent* ev );
	void mouseReleaseEvent( QMouseEvent* ev );
	void wheelEvent( QWheelEvent* ev );
	void keyPressEvent( QKeyEvent* ev );

	Scene* getScene() const { return scene_.get(); }
	Scenario* getScenario() const { return scenario_; }

	const vec3d& getCameraLoc() const ;
	const vec3d& getCameraOrientation() const ;
	const vec3d& getCameraPivot() const ;
	bool hasPerspective() const { return camera_->perspective; }
	bool cameraLocked() const { return camera_locked_; }
	const TimeWindow& getTimeWindow() const{ return time_window_; }

	QString getCameraPosStr() const;

	// Estimate the maximum screenshot dimensions.  In practice the maxima are almost certainly lower than the return value,
	// but the return value will at least provide a halfway sane upper bound.
	QSize maxScreenshotDims() const;

	// RAII class for re-using a screenshot buffer multiple times, e.g. for movie frames
	struct ScreenshotContext : public boost::noncopyable{
		// create a context with the given dimensions
		ScreenshotContext( I3GLWidget* w, int width, int height, double scaling_factor = 1.0 );
		~ScreenshotContext();

		private:
		I3GLWidget* w_;
		int xdim, ydim;
		int old_xdim, old_ydim;
		QGLFramebufferObject* buf;
		Scene::WithOverlaysScaled* scaling;
		friend class I3GLWidget;

	};

	QImage screenshotData( int xdim, int ydim, double scaling_factor = 1.0 );
	QImage screenshotData( const ScreenshotContext& );

	bool spinEnabled() const;

	QString getDebugInfo() const;

	QColor backgroundColor() const;
	void setBackgroundColor( QColor c );

	int referenceDpi() const{ return ref_dpi_; }
	void setReferenceDpi( int d ){ ref_dpi_ = d; }


protected Q_SLOTS:
	void spinStep();

public Q_SLOTS:

	void setTimeWindow( const TimeWindow& );
	void setCameraPivot( double, double, double );
	void setCameraPivot( const vec3d& );
	void setCameraLoc( double, double, double );
	void setCameraLoc( const vec3d& );
	void setCameraOrientation( double, double, double );
	void setCameraOrientation( const vec3d& );
	void rotateCamera( double );
	void focusCameraOn( const vec3d& );
	void resetCamera();
	void setPerspective( bool );
	void setCameraLock( bool );
	
	// the fundamental screenshot API, called by the various screenshot slots below
	void screenshot_engine( int xdim, int ydim, const std::string& name,
	                        int dpi, double scaling_factor = 1.0, double gamma_factor = 1.0 );

	// screenshot API.  Note that qmeta can only handle one arity per slot name
	// slots named "screenshot" do not have dpi or scaling features
 	void screenshot( int xdim, int ydim, const std::string& name = "ShovelScreen.png",
	                 double gamma_factor = 1.0 )
	{ screenshot_engine( xdim, ydim, name, 0, gamma_factor ); }
	void screenshot( const std::string& name ){ screenshot( 0, 0, name ); }
	void screenshot(){ screenshot( 0, 0 ); }

	// slots for specifying a DPI and inferring size and scaling from the current screen dimensions
	void screenshotDpi(int dpi, const std::string& name = "ShovelScreen.png",
	                   double gamma_factor = 1.0 );

	// slots for specifying final size in inches plus a dpi density
	void screenshotInches( double xsize, double ysize, int dpi,
	                       const std::string& name = "ShovelScreen.png",
	                       double gamma_factor = 1.0 );

	void toggleTVMode();
	void toggleSpin( bool );
	void doSetBackgroundColor();

Q_SIGNALS:
	void cameraMoved();
	void perspectiveChanged( bool );
	void cameraLockChanged( bool );
	void statusMessage( QString, int );

protected:
	void initializeGL();
	void resizeGL( int w, int h );
	void paintGL();

private:
	void boxSelectedDOMs();
	QImage createScreenshotData( int xdim, int ydim, QGLFramebufferObject& buf );

	boost::shared_ptr<I3Camera> camera_;
	boost::shared_ptr<Scene> scene_;
	Scenario* scenario_;

	// mouse / selection status
	QPoint startMouse_;
	QPoint mouse_;
	SceneOverlay* dragging_overlay_;
	OMKeySet selected_doms_;
	bool camera_locked_;

	//
	TimeWindow time_window_;

	QStackedWidget* grandparent_;
	QTimer spin_timer_;

	int ref_dpi_;

	// to compute fps
	int nframes_; // -2 = off, -1 = start
	std::clock_t lastTime_;

	friend class withRedrawDisabled;
};

class WithRotationDisabled : public boost::noncopyable {
	I3GLWidget* w_;
	bool spin_;
public:
	WithRotationDisabled( I3GLWidget* w ) : w_(w), spin_( w->spinEnabled() )
	{
		if( spin_ ) w_->toggleSpin(false);
	}

	~WithRotationDisabled(){
		if( spin_ ) w_->toggleSpin(true);
	}
};

class WithRedrawDisabled : public boost::noncopyable {
	I3GLWidget* w_;
public:
	WithRedrawDisabled( I3GLWidget* w ) : w_(w)
	{
		QObject::disconnect( w_, SIGNAL(cameraMoved()), w_, SLOT(updateGL()) );
	}

	~WithRedrawDisabled(){
		QObject::connect( w_, SIGNAL(cameraMoved()), w_, SLOT(updateGL()) );
		w_->updateGL();
	}
};

#endif /* I3_SHOVEL_GLWIDGET_H */
