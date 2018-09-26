#include "SteamshovelApp.h"
#include "moc_SteamshovelApp.cpp"

#include "ShovelMainWindow.h"
#include "ShovelLogger.h"
#include "FileService.h"
#include "scripting/qmeta.h"
#include "scripting/gil.h"
#include "scripting/pyerr.h"
#include "scripting/data.h"
#include "shovelart/Artist.h"
#include "shovelart/ArtistRegistry.h"
#include "shovelart/BindSignal.h"

#include <QSettings>
#include <QFont>
#include <QTimer>
#include <QMessageBox>
#include <QCryptographicHash>

#include <sstream>
#include <fstream>

#include <boost/python.hpp>
#include <boost/foreach.hpp>

// POSIX-headers for kill
#include <sys/types.h>
#include <signal.h>

SteamshovelApp::SteamshovelApp( int& argc, char* argv[],
                                scripting::PyConsole& console ) :
	QApplication( argc, argv ),
	console_( console ),
	logviewer_pid_( 0 )
{
	QCoreApplication::setOrganizationName( "The IceCube Collaboration");
	QCoreApplication::setOrganizationDomain( "icecube.wisc.edu" );
	QCoreApplication::setApplicationName( "Steamshovel" );

	setQuitOnLastWindowClosed( false );

	QObject::connect( this, SIGNAL(extras(bool)), 
	                  &ArtistRegistry::global(),
	                  SLOT(setExtras(bool)) );
	QObject::connect( this, SIGNAL(lastWindowClosed()),
	                  SLOT(quit()) );
}

SteamshovelApp::~SteamshovelApp()
{
	BOOST_FOREACH( ShovelMainWindow* w, windows_ )
		delete w;
	windows_.clear();
	QSettings settings;
	settings.setValue( "font", QFont() );
	settings.setValue( "autostart_logviewer",
		static_cast<ShovelLogger*>(
			GetIcetrayLogger().get()
		)->autoStartViewer()
    );
}

void SteamshovelApp::runStartupScripts()
{
	console_.executeScript( startup_script_.fileName().toStdString() );
}

void SteamshovelApp::startLogViewer()
{
	QStringList args( static_cast<ShovelLogger*>(
		GetIcetrayLogger().get() )->fileName() 
	);

	// check whether log-viewer is already running,
	// see http://stackoverflow.com/questions/12601759
	if( logviewer_pid_ && ::kill( logviewer_pid_, 0 ) == 0 )
		return;

	if( !QProcess::startDetached( "steamshovel-logviewer", args,
	                              QString(), &logviewer_pid_ ) )
		QMessageBox::critical( 0, "Steamshovel: Error",
		                       "Could not start steamshovel-logviewer.\n"
		                       "This should never happen, please send a bug-report." );
}

void SteamshovelApp::init( const std::vector<std::string>& startup_scripts,
                           bool batch )
{
        setWindowIcon(QPixmap(":/resources/IceCubeIcon.png"));
	QFont defaultFont;
#if defined(QT_WS_MAC) || defined(Q_OS_MACOS)
	// avoids text clipping on Mac
	defaultFont.setFamily( "Arial" );
#endif	
	QSettings settings;
	setFont( settings.value( "font", defaultFont ).value<QFont>() );

	static_cast<ShovelLogger*>(
		GetIcetrayLogger().get()
	)->setAutoStartViewer( settings.value( "autostart_logviewer", true ).toBool() );

	// uses QSettings and thus must be initialized here
	files_.reset( new FileService(this) );

	// setup scripting
	scripting::QMeta::exportObject( this );
	scripting::QMeta::addToGlobalNamespace( this, "app" );
	scripting::QMeta::exportObject( files_.get() );
	scripting::QMeta::addToParent( this, files_.get(), "files" );

	// register internal Python artists
	ArtistRegistry::global().registerPythonArtists();

	// concatenate python scripts to guarantee that
	// commands are executed in order
	if( !startup_script_.open() )
		log_fatal( "cannot open temporary file" );

	if( !startup_scripts.empty() || batch ){
		BOOST_FOREACH( const std::string& s, startup_scripts ){
			std::ifstream is( s.c_str() );
			char buf[4096];
			while( !is.eof() ){
				is.read(buf, 4096);
				startup_script_.write( buf, is.gcount() );
			}
			startup_script_.write( "\n" );
		}
		if( batch ) // automatically quit after running scripts
			startup_script_.write( "app.quit()\n" );
		startup_script_.flush();

		// put commands it in the queue
		QTimer::singleShot(0, this, SLOT(runStartupScripts()));
	}
}

bool SteamshovelApp::notify( QObject* receiver, QEvent* event ){
	try{
		return QApplication::notify( receiver, event );
	}
	catch( std::exception& e ){
		log_error_stream( "Uncaught exception in event loop:\n"
		                  << e.what() );
	}
	catch( boost::python::error_already_set& e ){
		log_error_stream( "Uncaught python exception in event loop:\n"
		                  << e );
	}
	return false;
}

void SteamshovelApp::quit( int return_code ){
	log_debug("quit with return code %d", return_code);
	BOOST_FOREACH( ShovelMainWindow* w, windows_ )
		w->close();
	QApplication::exit( return_code );
}

ShovelMainWindow* SteamshovelApp::newWindow( bool show ){
	// First window will be exported as 'window';
	// further windows get named 'windowN'
	std::stringstream name;
	name << "window";
	if( !windows_.empty() )
		name << windows_.size();

	windows_.push_back( new ShovelMainWindow( QString::fromStdString(name.str()) ) );

	scripting::QMeta::exportObject( windows_.back() );
	scripting::QMeta::addToGlobalNamespace( windows_.back(), name.str() );
	windows_.back()->initScripting( console_ );
	windows_.back()->setBatchMode( !show );
	QObject::connect( this, SIGNAL(extras(bool)),
	                  windows_.back(), SLOT(doExtras(bool)) );
	if( show )
		windows_.back()->show();
	return windows_.back();
}

QStringList SteamshovelApp::getWindowNames()const{
	QStringList result;
	BOOST_FOREACH( const ShovelMainWindow* w, windows_ )
		result.push_back( w->objectName() );
	return result;
}

QString SteamshovelApp::getSecret() const {
	return QString("You find a scroll with the letters MWPP on it. "
	               "Maybe a passphrase opens the scroll?");
}

void SteamshovelApp::setSecret( QString s ){
#if QT_VERSION >= 0x050000
	QByteArray hash =
		QCryptographicHash::hash( s.remove('.').toLatin1(),
	                              QCryptographicHash::Md5 ).toHex();
#else
	QByteArray hash =
		QCryptographicHash::hash( s.remove('.').toAscii(), 
	                              QCryptographicHash::Md5 ).toHex();
#endif
	Q_EMIT extras( QString("a5e64c44a56bb48a3adce2a7352be01a") == hash );
}
