#include <I3Test.h>
#include <cstdio> //for remove()
#include <fstream>
#include <boost/make_shared.hpp>
#include <icetray/I3SimpleLoggers.h>

TEST_GROUP(I3FileLogging)

TEST(LogToFile){
	auto testfile = I3Test::testfile("file_logging.log");
	SetIcetrayLogger(boost::make_shared<I3FileLogger>(testfile));
	
	static const char* unit="LogToFileTest";
	auto __icetray_logger_id=[]()->const char*{ return(unit); };
	GetIcetrayLogger()->SetLogLevelForUnit(unit,I3LOG_TRACE);
	
	log_trace("This is a trace level message");
	log_debug("This is a debug level message");
	log_info("This is an info level message");
	log_notice("This is a notice level message");
	log_warn("This is a warn level message");
	log_error("This is an error level message");
	try{
		log_fatal("This is a fatal level message");
		FAIL("log_fatal() should throw");
	}catch(...){/*squash*/}
		
	std::ifstream logfile(testfile);
	std::string line;
#ifndef I3_COMPILE_OUT_VERBOSE_LOGGING
	ENSURE(!!std::getline(logfile,line));
	ENSURE(line.find("This is a trace level message")!=std::string::npos);
	ENSURE(!!std::getline(logfile,line));
	ENSURE(line.find("This is a debug level message")!=std::string::npos);
#endif
	ENSURE(!!std::getline(logfile,line));
	ENSURE(line.find("This is an info level message")!=std::string::npos);
	ENSURE(!!std::getline(logfile,line));
	ENSURE(line.find("This is a notice level message")!=std::string::npos);
	ENSURE(!!std::getline(logfile,line));
	ENSURE(line.find("This is a warn level message")!=std::string::npos);
	ENSURE(!!std::getline(logfile,line));
	ENSURE(line.find("This is an error level message")!=std::string::npos);
	ENSURE(!!std::getline(logfile,line));
	ENSURE(line.find("This is a fatal level message")!=std::string::npos);
	
	std::remove(testfile.c_str());
}
