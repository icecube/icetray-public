#include <sstream>
#include <iostream>
#include <cstdio>
#include <unistd.h>

#include <QtCore>
#include <QWidget>
#include <boost/python.hpp>

#include <icetray/I3Logging.h>

#include "qmeta.h"
#include "embed.h"
#include "gil.h"
#include "pyerr.h"
#include "steamshovel/SteamshovelApp.h"

namespace scripting{

namespace bp = boost::python;

PyInterpreter::PyInterpreter( char* progname )
{
	scripting::QMeta::pre_init();
#if PY_MAJOR_VERSION < 3
	Py_SetProgramName(progname);
	Py_Initialize();
	PySys_SetArgv(1, &progname);
#else
	wchar_t *wprogname = new wchar_t[255];
	mbstowcs(wprogname, progname, 255);
	Py_SetProgramName(wprogname);
	Py_Initialize();
	PySys_SetArgv(1, &wprogname);
#endif
	PyEval_InitThreads();
	scripting::QMeta::post_init();

	// Load essential Python libraries now, so we can rely on their existence later.
	try{
		bp::import("icecube.dataio");
		bp::import("icecube.icetray");
		bp::import("icecube.dataclasses");
		bp::import("icecube.steamshovel");
		bp::import("icecube.shovelart");
	}
	catch( bp::error_already_set& e ){
		log_error_stream( "Problem loading icecube python bindings:\n" << e
		                  << "\nAre there unbuilt pybindings?" );
		throw;
	}

	// Also load some non-essentual libs for convenience
	try{ bp::import("icecube.recclasses"); }
	catch( bp::error_already_set& e){ PyErr_Clear(); }

	try{ bp::import("icecube.simclasses"); }
	catch( bp::error_already_set& e ){ PyErr_Clear(); }
}

PyInterpreter::~PyInterpreter()
{
	// We don't call Py_Finalize() here, because it causes a segfault...
}

PyConsole::PyConsole( const PyInterpreter& /* keep this to ensure order of calls */,
                      Type requested ):
	type_(None)
{
	// if either stdin or stdout are pipes, consoles do not work
	if( !isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO) ){
		if( requested == IPython_shell || requested == Vanilla )
			requested = None;
	}else{
		// Remember terminal state to restore it later
		if( 0 != tcgetattr( STDIN_FILENO, &stored_termios_state ) ){
			log_warn("Could not tcgetattr, errno=%u", errno);
		}
	}

	switch( requested ) {
		case Macapp_widget_or_nothing:
		case IPython_widget:{
			if( init_ipython_widget() ){
				type_ = IPython_widget;
				break;
			}
			if( requested == Macapp_widget_or_nothing ){
				// ipython widget failed and we are a mac app.  No ipython console anywhere.
				log_warn( "Cannot embed IPython Qt widget, so this app has no IPython active" );
				type_ = None;
				break;
			}
			else{
				log_warn( "Cannot embed IPython Qt widget, falling back to tty-based console" );
			}		
		}
		case IPython_shell:{
			if( init_python_shell( true ) ){
				type_ = IPython_shell;
				break;
			}
			log_warn( "Could not use IPython, falling back to vanilla python console" );
		}
		case Vanilla:{
			init_python_shell( false );
			type_ = Vanilla;
			break;
		}
		case None:
			type_ = None;
	}
}

bool PyConsole::init_ipython_widget(){
	bp::object module = boost::python::import("icecube.steamshovel.embed.console_widget");
	bp::object init_f = module.attr("init_embedded_kernel");
	try{
		bool has_embedded_kernel = init_f();
		if( has_embedded_kernel )
			return true;
	}
	catch( bp::error_already_set& e ){
		log_error_stream( "Exception from init_embedded_kernel:\n" << e );
	}
	return false;
}

bool PyConsole::init_python_shell( bool ipython ){
	try{
		bp::object module = bp::import("icecube.steamshovel.embed.console");
		if( ipython ){
			try{ bp::import("IPython"); }
			catch( bp::error_already_set& e ){
				log_debug_stream( "Error while importing IPython:\n" << e );
				return false;
			}
		}
		bp::object thread_f = module.attr("launchConsoleThread");
		thread_f( ipython );
		return true;
	}
	catch( bp::error_already_set& e ){
		log_error_stream( "Could not launch python console thread:\n" << e );
	}
	return false;
}

bool PyConsole::embedInto( QWidget* widget ) const
{
	if( type_ == PyConsole::IPython_widget ){
		ScopedGIL gil;
		try{
			bp::object module = bp::import("icecube.steamshovel.embed.console_widget");
			bp::object embed_f = module.attr("embed_shell_widget");
			embed_f( reinterpret_cast<long int>(widget) );
			return true;
		}
		catch( bp::error_already_set& e ){
			log_error_stream( "Could not embed the IPython Qt widget:\n" << e );
		}
	}
	return false;
}

void PyConsole::executeScript( const std::string& scriptpath ){
	log_debug( "Will execute the script file %s", scriptpath.c_str() );
	ScopedGIL gil;
	bp::object embed = bp::import("icecube.steamshovel.embed.console_widget");
	bp::object embed_func = embed.attr("execScript");
	embed_func(scriptpath);
}

void PyConsole::executeString( const std::string& dothis ){
	ScopedGIL gil;
	bp::object main = bp::import("__main__").attr("__dict__");
	bp::exec( dothis.c_str(), main, main );
}

PyConsole::~PyConsole()
{
	if( type_ == Vanilla || type_ == IPython_shell ){
		// Restore terminal attributes, in case readline has messed them up.
		if( 0 != tcsetattr( STDIN_FILENO, 0, &stored_termios_state ) ){
			log_warn( "Could not tcsetattr, errno=%u", errno );
		}
		// add a newline for good measure, because the shells don't
		std::cout << std::endl;
	}
}

} // namespace scripting
