#include "scripting/data.h"
#include "scripting/embed.h"
#include "SteamshovelApp.h"
#include "FileService.h"
#include "ShovelLogger.h"
#include <icetray/I3Logging.h>

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <QGLFormat>
#include <QSettings>
#include <QDir>
#include <QString>
#include <QStringList>

namespace po = boost::program_options;

namespace {
	void warn_unrecognized( const QList<QString>& missed )
	{
		for( QList<QString>::const_iterator i = missed.begin(); i != missed.end(); ++i ){
			log_warn_stream( "Unknown Qt option: " << i->toStdString() );
		}
	}

	char** getQargs( const char* argv0, const std::vector< std::string >& args ){
		char** array = new char*[args.size()+1];
		array[0] = strdup(argv0);
		int idx = 1;
		for( std::vector<std::string>::const_iterator i = args.begin(); i!=args.end(); ++i ){
			char* v = strdup( i->c_str() );
			array[idx++] = v;
		}
		return array;
	}

	// In MacOS application bundles, the entry point receives a command line argument -psn_(stuff)
	// indicating the process serial number.
	// boost::program_options doesn't seem to have an easy way to handle this syntax,
	// so we use this custom parser to convert it into an imaginary '--macapp'
	std::pair<std::string, std::string> handle_mac_psn_arg( const std::string& s ){
		if( s.find("-psn_") == 0 ){
			log_trace_stream( "Ignoring osx process sn argument:" << s );
			return std::make_pair<std::string, std::string>("macapp", "");
		}
		return std::make_pair<std::string, std::string>("","");
	}

	void logging(const po::variables_map& vm,
	             const std::string& type,
	             const std::vector<std::string>& units){
		const I3LogLevel level = type == "debug" ? I3LOG_DEBUG : I3LOG_TRACE;
		if( vm.count(type.c_str()) ){
			BOOST_FOREACH( const std::string& u, units ){
				if( u == "all" ){
					log_info_stream( "global loglevel " << level );
					GetIcetrayLogger()->SetLogLevel(level);
				}else{
					log_info_stream( "loglevel " << level << " for " << u );
					GetIcetrayLogger()->SetLogLevelForUnit(u.c_str(), level);
				}
			}
		}
	}
}

int main(int argc, char* argv[]){

	I3LoggerPtr logger( new ShovelLogger );
	SetIcetrayLogger( logger );

	using namespace scripting;

	// debugging sanity check: ensure no shared library has initialized python
	if( Py_IsInitialized() ){
		log_warn("Python initialized prior to main(), there may be trouble ahead");
	}

	/*
	 *  Dealing with arguments:
	 *    - First parse all options that steamshovel recognizes.  This allows us to take
	 *      specified actions, e.g. print the help text and exit, before embarking on Qt's
	 *      initialization.  Error on unknown args.
	 *    - Pass --Qarg=* options to Qt in a fake argv; warn about unconsumed args.
	 */
	typedef std::vector<std::string> string_vector;
	string_vector Qargs, files, scripts,
	              debug_units, trace_units,
	              ignored_keys;

	po::options_description main_opts("Command-line options");
	main_opts.add_options()
		("help,h", "Produce help message")
		("debug,D", po::value< string_vector >(&debug_units)->composing(),
		            "Show debug messages for a unit, use 'all' to set global level")
		("trace,T", po::value< string_vector >(&trace_units)->composing(),
		            "Show trace messages for a unit, use 'all' to set global level")
		("console", "Use the shell console instead of the interactive GUI console")
		("vanillaconsole", "Use the plain python shell console, without ipython")
		("noconsole", "Do not use any form of interactive python")
		("Qarg", po::value< string_vector >(&Qargs)->composing(),
		         "Pass argument to Qt" )
		("script,s", po::value< string_vector >(&scripts)->composing(),
		             "Script to execute" )
		("batch", "Batch execution: do not display a window (*experimental*)")
		("reset", "Reset all of steamshovel's stored settings" )
		("ignore", po::value< string_vector >(&ignored_keys)->composing(),
		           "keys to not load from files into frames")
		("input", po::value< string_vector >(&files)->composing(),
		          "Path to a .i3(.gz|.bz2|.zst) file to load" )
		("testmode", "Run in test mode. Session memory is off and shell console is active." )
		;

	po::options_description hidden_opts("Hidden");
	hidden_opts.add_options()
		("macapp", "Behave as if invoked as a mac app")
	;

	po::options_description all_opts("All options");
	all_opts.add( main_opts ).add( hidden_opts );

	po::positional_options_description positional;
	positional.add( "input", -1 );

	// parse our options
	po::variables_map vm;
	try{
		po::parsed_options p =
			po::command_line_parser(argc, argv).
				options(all_opts).
				positional(positional).
				extra_parser(handle_mac_psn_arg).
				run();
		po::store( p, vm );
		po::notify( vm );
	}
	catch( std::exception& e ){
		std::cerr << "Command line error: " << e.what() << std::endl;
		std::cerr << main_opts << std::endl;
		return 1;
	}
	if( vm.count("help") ){
		std::cout << main_opts << std::endl;
		return 0;
	}

	if( vm.count("testmode") ){
		// store settings in a temporary location when in test mode,
		// temporary configuration location is later reset
		QString tmp_cfg_path = QDir::temp().path() + "/SteamshovelTestConfig";
		QSettings::setPath( QSettings::NativeFormat,
		                    QSettings::UserScope,
		                    tmp_cfg_path );
	}

	PyConsole::Type requested_console = 
		scripts.empty() ? PyConsole::IPython_widget : PyConsole::IPython_shell;
	if( vm.count("macapp") )
		requested_console = PyConsole::Macapp_widget_or_nothing;
	if( vm.count("console")  )
		requested_console = PyConsole::IPython_shell;
	if( vm.count("vanillaconsole") )
		requested_console = PyConsole::Vanilla;
	if( vm.count("noconsole") )
		requested_console = PyConsole::None;

	// "trace" overwrites "debug"
	logging(vm, "debug", debug_units);
	logging(vm, "trace", trace_units);

	log_debug( "Initializing python" );
  
  PyInterpreter* pyinter;
  PyConsole* pyconsole;
  try {
    pyinter =  new PyInterpreter(argv[0]);
    // may downgrade requested_console to what's available;
    // also: consoles get deactivated if STDIN or STDOUT are not tty
    pyconsole = new PyConsole(*pyinter, requested_console);
  }catch( boost::python::error_already_set& ) {
    PyErr_Print();
    return 123213321; //just picked something at random here
  }
  
  int return_code = 0;
	{
		log_debug( "Creating SteamshovelApp" );

		char** qt_specific_args = getQargs( argv[0], Qargs );
		int qargc = Qargs.size()+1;
		SteamshovelApp app( qargc, qt_specific_args, *pyconsole );

		if( vm.count("reset") || vm.count("testmode") ){
			// must be after SteamshovelApp is constructed
			QSettings settings;
			settings.clear();
			if( vm.count("testmode") ){
				// suppress the update dialog in testmode,
				settings.setValue( "last_revision", SHOVEL_REVISION );
			}
		}

		if( vm.count("ignore") ){
			QStringList keys;
			BOOST_FOREACH( const std::string& s, ignored_keys )
				if( !s.empty() )
					keys.push_back( QString::fromStdString(s) );
			app.files().setFilters( keys );
		}

		// warn the user about unrecognized options
		QList<QString> qt_remaining_options = app.arguments();
		qt_remaining_options.removeFirst(); // the program name
		warn_unrecognized( qt_remaining_options );

		app.init( scripts, vm.count("batch") > 0 );

		BOOST_FOREACH( const std::string& fn, files )
			app.files().openLocalFile( fn );

		// Create a new window
		QGLFormat f/*( QGL::SampleBuffers )*/;
		QGLFormat::setDefaultFormat( f );
		app.newWindow( vm.count("batch") == 0 );

		// unblock python threads; equivalent to Py_UNBLOCK_THREADS with return value ignored
		// Each thread in steamshovel must grab the GIL explicitly before touching python.
		Py_BEGIN_ALLOW_THREADS

		// Run the main event loop
		return_code = app.exec();

		// go back to a single thread before calling all destructors
		Py_END_ALLOW_THREADS
	}

	// Put a hint into the log-file to indicate a clean shutdown
	static_cast<ShovelLogger*>( logger.get() )->exitMarker();

	// PyConsole attempts to cleanly shut down the python interaction system here
	return return_code;
}

// STJ : SDG
