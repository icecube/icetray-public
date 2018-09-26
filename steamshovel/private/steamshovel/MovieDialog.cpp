#include <boost/preprocessor/cat.hpp>
#include <boost/python.hpp>
#include "scripting/gil.h"
#include "scripting/qstringlist_converter.h"

#include <QGraphicsScene>
#include <QFileDialog>

#include "shovelart/timewindow.h"
#include "I3GLWidget.h"
#include "MovieEngine.h"
#include "MovieDialog.h"
#include "moc_MovieDialog.cpp"

MovieDialog::MovieDialog( I3GLWidget* gl, const TimeWindow& window, MovieEngine* engine ) :
	QDialog(), gl_(gl), engine_(engine), initial_window_( window )
{
	ui.setupUi(this);
	QObject::connect( ui.previewButton, SIGNAL(clicked()), SLOT(updatePreview()) );
	/*ui.graphicsView->setScene( scene_ );*/

	QObject::connect( engine, SIGNAL(fpsChanged(int)), SLOT(updateDurationLabel()) );
	QObject::connect( engine, SIGNAL(nframesChanged(int)), SLOT(updateDurationLabel()) );

	QObject::connect( engine, SIGNAL(widthChanged(int)), SLOT(updateOutputLabel()) );
	QObject::connect( engine, SIGNAL(heightChanged(int)), SLOT(updateOutputLabel()) );
	QObject::connect( engine, SIGNAL(scaleFactorChanged(double)), SLOT(updateOutputLabel()) );
	QObject::connect( engine, SIGNAL(rescaleFlagChanged(bool)), SLOT(updateOutputLabel()) );

	// connect this dialog to the values of the MovieEngine.  SIGNAL and SLOT are not amenable to preprocessor tricks,
	// so we're just going to do this the verbose way.
	QObject::connect( engine, SIGNAL(widthChanged(int)), ui.widthSpinner, SLOT(setValue(int)) );
	QObject::connect( ui.widthSpinner, SIGNAL(valueChanged(int)), engine, SLOT(setWidth(int)) );
	ui.widthSpinner->setValue( engine->width() );

	QObject::connect( engine, SIGNAL(heightChanged(int)), ui.heightSpinner, SLOT(setValue(int)) );
	QObject::connect( ui.heightSpinner, SIGNAL(valueChanged(int)), engine, SLOT(setHeight(int)) );
	ui.heightSpinner->setValue( engine->height() );

	QObject::connect( engine, SIGNAL(fpsChanged(int)), ui.fpsSpinner, SLOT(setValue(int)) );
	QObject::connect( ui.fpsSpinner, SIGNAL(valueChanged(int)), engine, SLOT(setFps(int)) );
	ui.fpsSpinner->setValue( engine->fps() );

	QObject::connect( engine, SIGNAL(nframesChanged(int)), ui.nframesSpinner, SLOT(setValue(int)) );
	QObject::connect( ui.nframesSpinner, SIGNAL(valueChanged(int)), engine, SLOT(setNframes(int)) );
	ui.nframesSpinner->setValue( engine->nframes() );

	QObject::connect( engine, SIGNAL(starttimeChanged(double)), ui.starttimeSpinner, SLOT(setValue(double)) );
	QObject::connect( ui.starttimeSpinner, SIGNAL(valueChanged(double)), engine, SLOT(setStarttime(double)) );
	ui.starttimeSpinner->setValue( engine->starttime() );

	QObject::connect( engine, SIGNAL(endtimeChanged(double)), ui.endtimeSpinner, SLOT(setValue(double)) );
	QObject::connect( ui.endtimeSpinner, SIGNAL(valueChanged(double)), engine, SLOT(setEndtime(double)) );
	ui.endtimeSpinner->setValue( engine->endtime() );

	QObject::connect( engine, SIGNAL(rotationChanged(double)), ui.rotationSpinner, SLOT(setValue(double)) );
	QObject::connect( ui.rotationSpinner, SIGNAL(valueChanged(double)), engine, SLOT(setRotation(double)) );
	ui.rotationSpinner->setValue( engine->rotation() );

	QObject::connect( engine, SIGNAL(scaleFactorChanged(double)), ui.scaleSpinner, SLOT(setValue(double)) );
	QObject::connect( ui.scaleSpinner, SIGNAL(valueChanged(double)), engine, SLOT(setScaleFactor(double)) );
	ui.scaleSpinner->setValue( engine->scaleFactor() );

	QObject::connect( engine, SIGNAL(rescaleFlagChanged(bool)), ui.resampleCheckBox, SLOT(setChecked(bool)) );
	QObject::connect( ui.resampleCheckBox, SIGNAL(clicked(bool)), engine, SLOT(setRescaleFlag(bool)) );
	ui.resampleCheckBox->setChecked( engine->rescaleFlag() );

	views_[0] = ui.firstView;
	views_[1] = ui.midView;
	views_[2] = ui.lastView;

	for( int i = 0; i < 3; ++i ){
		scenes_[i] = new QGraphicsScene();
		views_[i]->setScene( scenes_[i] );
	}

	QSize maxdim = gl_->maxScreenshotDims();
	ui.widthSpinner->setMaximum( maxdim.width() );
	ui.heightSpinner->setMaximum( maxdim.height() );

	ui.starttimeSpinner->setMinimum( window.start() );
	ui.starttimeSpinner->setMaximum( window.end() );
	ui.starttimeSpinner->setValue( window.colorStart() );

	ui.endtimeSpinner->setMinimum( window.start() );
	ui.endtimeSpinner->setMaximum( window.end() );
	ui.endtimeSpinner->setValue( window.colorEnd() );

	QStringList encoders;
	{
		using namespace boost::python;
		scripting::ScopedGIL gil;
		object movieutils = import( "icecube.steamshovel.util.movie" );
		object result = movieutils.attr( "getMovieEncoders" )();
		QStringListConverter(); // activate converter for next extract
		encoders = extract<QStringList>( result );
	}
	ui.encoderComboBox->addItems( encoders );
	ui.encoderComboBox->addItem( "png images" ); // a fallback

	updateDurationLabel();
	updateOutputLabel();
	updatePreview();
}

MovieDialog::~MovieDialog(){
	for( int i = 0; i < 3; ++i ){
		delete scenes_[i];
	}
}

void MovieDialog::accept(){
	if( ui.encoderComboBox->currentText() == "png images" ){
		QString directory = QFileDialog::getExistingDirectory(
			this, "Choose image output directory" , engine_->imageOutputDir() );
		if( directory.length() ){
			engine_->setMovieFileName( QString() );
			engine_->setImageOutputDir( directory );
			QDialog::accept();
		}
	}
	else{
		engine_->setEncoder( ui.encoderComboBox->currentText() );
		QString moviename = engine_->movieFileName();
		if( moviename.isEmpty() )
			moviename += "./Shovelfilm.mp4";
		moviename = QFileDialog::getSaveFileName(
			this, "Save .mp4 movie", moviename, "Movies (*.mp4)" );
		if( moviename.length() ){
			engine_->setImageOutputDir( QString() );
			engine_->setMovieFileName( moviename );
			QDialog::accept();
		}
	}
}

void MovieDialog::updatePreview(){
	WithRedrawDisabled disable(gl_);

	double scale = engine_->scaleFactor();
	int w = int(engine_->width() * scale);
	int h = int(engine_->height() * scale);
	viewsize_ = QSize(w,h);

	I3GLWidget::ScreenshotContext context(gl_, w, h, scale);

	vec3d starting_loc = gl_->getCameraLoc();
	vec3d starting_orientation = gl_->getCameraOrientation();

	// we use a deliberately inefficient series of calls to rotateCamera/setTimeWindow in order
	// to simulate the rotations that the movie generation code will actually perform

	// frame number for each of the three views
	int frames = ui.nframesSpinner->value();
	int halframe = frames / 2;
	int frame_counts[3] = {0, halframe, frames};
	int current = frame_counts[0];

	// time step between each frame
	int framestep = engine_->timeStepPerFrame();
	TimeWindow win(initial_window_);
	win.setCurrent( ui.starttimeSpinner->value() );
	gl_->setTimeWindow(win);

	// rotation amount between each frame
	double rot_per_frame = engine_->rotPerFrame();

	for(int i = 0; i < 3; ++i ){
		while( current < frame_counts[i] ){
			++current;
			gl_->rotateCamera( rot_per_frame );
			win.setCurrent( win.current() + framestep );
			gl_->setTimeWindow(win);
		}
		QImage img = gl_->screenshotData( context );
		scenes_[i]->clear();
		scenes_[i]->addPixmap( QPixmap::fromImage(img) );
		views_[i]->fitInView( 0, 0, w, h, Qt::KeepAspectRatio );
	}

	gl_->setCameraLoc( starting_loc );
	gl_->setCameraOrientation( starting_orientation );
	gl_->setTimeWindow( initial_window_ );
}

void MovieDialog::resizeEvent( QResizeEvent* e ){
	QDialog::resizeEvent(e);
	for( int i = 0; i < 3; ++i ){
		views_[i]->fitInView( 0, 0, viewsize_.width(), viewsize_.height(), Qt::KeepAspectRatio );
	}
}

void MovieDialog::showEvent( QShowEvent* e ){
	QDialog::showEvent(e);
	for( int i = 0; i < 3; ++i ){
		views_[i]->fitInView( 0, 0, viewsize_.width(), viewsize_.height(), Qt::KeepAspectRatio );
	}
}

void MovieDialog::updateDurationLabel(){
	double duration = engine_->duration();
	QString msg = QString( "Duration: %1 seconds" ).arg( duration );
	ui.durationLabel->setText( msg );
}

void MovieDialog::updateOutputLabel(){
	double s = engine_->scaleFactor();
	int w = engine_->width();
	int h = engine_->height();
	int sw = int(s * w);
	int sh = int(s * h);
	bool rescale = engine_->rescaleFlag();

	QString msg = QString( "Frames will be created at %1 X %2 pixels" ).arg( sw ).arg( sh );
	if( rescale && ((sw != w) || (sh != h)) ){
		msg = msg + QString( " and smoothly resized to %1 X %2" ).arg( w ).arg( h );
	}
	ui.outputLabel->setText( msg );
}
