#include <boost/noncopyable.hpp>
#include <boost/python.hpp>
#include <boost/foreach.hpp>

#include <QLabel>
#include <QProgressDialog>
#include <QLayout>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QImageWriter>

#include "MovieEngine.h"
#include "moc_MovieEngine.cpp"

#include "scripting/gil.h"
#include "scripting/pyerr.h"
#include "scripting/qstring_converter.h"

#include "I3GLWidget.h"
#include "ui_movie_progress.h"

#include <icetray/I3Logging.h>

namespace bp = boost::python;

void MovieEngine::setPreframeHook( boost::python::object hook ){
	// a GIL is necessary here in case the existing hook is a valid object,
	// since its refcount may go to zero and invoke python
	scripting::ScopedGIL gil;
	log_debug("Setting the preframe hook");
	preframe_hook_ = hook;
}

class DialogMovieProgress : public MovieProgress, public boost::noncopyable {
public:
	DialogMovieProgress( MovieEngine* engine, int width, int height ) :
		dialog_(), ui_(), w_(width), h_(height)
	{
		ui_.setupUi( &dialog_ );
		dialog_.setModal(true);

		ui_.graphicsView->setScene( &scene_ );
		QObject::connect( ui_.buttonBox, SIGNAL( rejected() ), engine, SLOT( cancelCurrentProduction() ) );
	}

	virtual void start( int max ){
		dialog_.show();
		ui_.progressBar->setMinimum(0);
		ui_.progressBar->setMaximum(max);
		ui_.progressBar->setValue(0);
		ui_.graphicsView->fitInView( 0, 0, w_, h_, Qt::KeepAspectRatio );
	}

	virtual void step( int k, const QString& string, const QImage & image ){
		if( ui_.checkBox->isChecked() ){
			scene_.clear();
			scene_.addPixmap( QPixmap::fromImage(image) );
		}
		ui_.progressBar->setValue( k );
		ui_.progressBar->update();
		ui_.label->setText( string );
	}

	virtual void converting( ){
		ui_.label->setText( "Converting to video..." );
		ui_.progressBar->setValue(0);
		ui_.progressBar->setMaximum(0);
	}

	virtual void done(){
		dialog_.done( 0 );
	}

	QDialog dialog_;
	Ui::MovieProgressDialog ui_;
	QGraphicsScene scene_;
	int w_, h_;
};

void MovieEngine::cancelCurrentProduction(){
	cancelflag_ = true;
}

void MovieEngine::produce(){
	// initial file path checks
	if( movie_filename_.length()==0 && output_dir_.length()==0  ){
		log_error( "MovieEngine needs either an output dir or a movie filename" );
		return;
	}

	bool have_hook = false;
	bool have_temp_image_dir = false;
	{
		scripting::ScopedGIL gil;
		// check once, with the gil held, to see if we are using a preframe hook
		if( preframe_hook_ != bp::object() ){
			have_hook = true;
		}

		if( output_dir_.length() == 0 ){
			try{
				bp::object movieutils = bp::import( "icecube.steamshovel.util.movie" );
				bp::object tempdir = movieutils.attr( "tempImageDir" );
				output_dir_ = bp::extract<QString>( tempdir() )();
				have_temp_image_dir = true;
			}
			catch( bp::error_already_set& e ){
				log_error_stream( "Preparing movie production failed with Python exception:\n" << e);
				return;
			}
		}
	} // gil released

	WithRedrawDisabled disabled( gl_ );
	I3GLWidget::ScreenshotContext context( gl_,
	                                       int(width_*scale_factor_),
	                                       int(height_*scale_factor_),
	                                       scale_factor_ );

	TimeWindow initial_window = gl_->getTimeWindow();
	TimeWindow win = initial_window;
	win.setBounds( std::min(win.start(),starttime_), std::max(win.end(),endtime_) );

	double rotstep = rotPerFrame();

	int timestep = timeStepPerFrame();
	int current_time = starttime_;

	cancelflag_ = false;
	boost::shared_ptr<MovieProgress> progress( new NullMovieProgress() );
	if( GUI_progress_ )
		progress = boost::shared_ptr<MovieProgress>(
            new DialogMovieProgress(this, width_, height_));
	QImageWriter qw;
	qw.setGamma( 1.0 / 2.2 );

	progress->start( nframes_ );
	int output_count = 0;
	for( int i = 0; i < nframes_; ++i ){
		win.setCurrent( current_time );
		gl_->setTimeWindow( win );

		if( have_hook )
		{
			scripting::ScopedGIL gil;
			try{
				preframe_hook_( i );
			}
			catch( bp::error_already_set& e ){
				log_error_stream( "Python error in preframe hook:" << e );
				log_error( "Canceling the movie" );
				cancelflag_ = true;
			}
		}

		QImage frame = gl_->screenshotData( context );
		if( rescale_flag_ && scale_factor_ != 1.0 ){
			frame = frame.scaled( width_, height_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
		}
		QString filename = QString( "frame%1.png" ).arg( i, 8, 10, QChar('0') );

		progress->step( i, QString( "Saving %1" ).arg(filename), frame );

		QString filepath = output_dir_ + "/" + filename;
		qw.setFileName( filepath );
		qw.write( frame );
		output_count++;

		gl_->rotateCamera( rotstep );
		current_time += timestep;
		QApplication::processEvents();
		if( cancelflag_ ){
			break;
		}
	}

	log_info_stream( "Saved " << output_count << " images to " << output_dir_.toStdString() );

	if( movie_filename_.length() && !cancelflag_ ){
		progress->converting();
		scripting::ScopedGIL gil;
		try{
			bp::object movieutils = bp::import( "icecube.steamshovel.util.movie" );
			bp::object mclass = movieutils.attr("AsyncMovie");
			bp::object mobj = mclass( encoder_, output_dir_, movie_filename_, fps_ );
			bp::object isdone = mobj.attr("isDone");
			bool finished;
			while( !(finished = bp::extract<bool>( isdone() )()) ){
				QApplication::processEvents();
				if( cancelflag_ ){
					mobj.attr("stop")();
					break;
				}
			}
			if( !cancelflag_ ){
				log_info_stream( "Saved " << movie_filename_.toStdString() );
			}
		}
		catch( bp::error_already_set& e ){
			log_error_stream( "Movie production failed with Python exception:\n" << e);
		}
	}

	if( cancelflag_ ){
		log_info( "Movie production canceled by user" );
	}

	// clean up temporary directory
	if( have_temp_image_dir ){
		QDir d( output_dir_ );
		BOOST_FOREACH( QString x, d.entryList() )
			d.remove( x );
		d.rmdir( output_dir_ );
	}

	progress->done();
	gl_->setTimeWindow( initial_window );

}
