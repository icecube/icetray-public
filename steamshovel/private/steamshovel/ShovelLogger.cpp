#include "ShovelLogger.h"
#include "SteamshovelApp.h"
#include <stdexcept>
#include <vector>
#include <QDir>
#include <QMutexLocker>

ShovelLogger::ShovelLogger() :
	I3PrintfLogger(),
	mutex_( QMutex::Recursive ),
	auto_start_viewer_( true )
{
	QString filename =
		QString( "/tmp/steamshovel_%1.log" )
		.arg( QCoreApplication::applicationPid() );
	file_.setFileName( filename );
	if( !file_.open( QIODevice::WriteOnly | QIODevice::Text ) )
		throw std::runtime_error( "Could not make temporary file" );
}

void ShovelLogger::Log( I3LogLevel level,
                        const std::string &unit,
                        const std::string &file, int line,
                        const std::string &func,
                        const std::string &message )
{
	QMutexLocker guard( &mutex_ );

	I3PrintfLogger::Log( level, unit,
	                     file, line,
	                     func, message );

	// If log level is too low, ignore message
	// This also results in the Log Viewer (see end of function) not being shown if the level
	// is above I3LOG_ERROR since this would show a possibly empty window.
	if (level < LogLevelForUnit(unit))
		return;

	const char* log_description = 0;
	switch (level) {
	case I3LOG_TRACE:
		log_description = "<font color=\"blue\">TRACE</font>";
		break;
	case I3LOG_DEBUG:
		log_description = "<font color=\"green\">DEBUG</font>";
		break;
	case I3LOG_INFO:
		log_description = "INFO";
		break;
	case I3LOG_NOTICE:
		log_description = "NOTICE";
		break;
	case I3LOG_WARN:
		log_description = "<font color=\"orange\">WARN</font>";
		break;
	case I3LOG_ERROR:
		log_description = "<font color=\"red\">ERROR</font>";
		break;
	case I3LOG_FATAL:
		log_description = "<font color=\"red\">FATAL</font>";
		break;
	default:
		log_description = "UNKNOWN";
		break;
	}

	std::string trimmed_filename;
	size_t lastslash = file.rfind('/');
	if (lastslash != std::string::npos)
		trimmed_filename = file.substr(lastslash+1);

	const char* format = "<big><b>%s (%s)</b></big>: %s:%d in <b>%s</b><pre>%s</pre><br>\n";

	std::vector<char> buf( 1024 );
	int size = snprintf( &buf.front(), 1023, format,
	    log_description, unit.c_str(),
	    trimmed_filename.c_str(), line,
	    func.c_str(), message.c_str() );

	if( size > 1023 ){
		buf.resize(size + 1);
		sprintf( &buf.front(), format,
		         log_description, unit.c_str(),
		         trimmed_filename.c_str(), line,
		         func.c_str(), message.c_str() );
	}

	file_.write( &buf.front(), size );
	file_.flush();

	if( level >= I3LOG_ERROR && auto_start_viewer_ )
		SteamshovelApp::instance()->startLogViewer();
}

void ShovelLogger::exitMarker(){
	file_.write( "<!-- Steamshovel Normal Exit -->\n" );
	file_.flush();
}
