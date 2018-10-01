/* -*- mode: c++; -*- vim: set ft=cpp: */
/**
   copyright  (C) 2005
   the icecube collaboration
   $Id$

   @version $Revision$
   @date $Date$
   @author troy d. straszheim

   Main testing routine for I3Test
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/assign.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/std/set.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/ref.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#ifndef NO_ICETRAY
	#include <boost/python.hpp>
#endif
#include <sys/wait.h>
#include <setjmp.h>

#include <I3Test.h>
#ifndef NO_ICETRAY
	#include <icetray/init.h>
	#include <icetray/I3Logging.h>
#endif
#include <boost/program_options.hpp>
#include <iterator>
#include <iomanip>
#include <string.h>
#include <signal.h>

using namespace I3Test;
namespace I3Test 
{
  const std::string 
  testfile(const std::string& fname, const std::string& project)
  {
    const char* i3_work = getenv("I3_BUILD");
    ENSURE(i3_work != NULL);
  
    const std::string dirname = i3_work + string("/") 
      + project + "/testdata";

#if BOOST_VERSION > 104100
    const boost::filesystem::path datadir(dirname);
#else
    const boost::filesystem::path datadir(dirname, boost::filesystem::native);
#endif
    boost::filesystem::create_directories(datadir);
    return dirname + std::string("/") + fname;
  }

  bool show_spew = false;
  bool fork_units = false;

  int signal_returned;
  int child_pid;
  sigjmp_buf jmp_env;

  ofstream rout;

  unsigned unit_test_time_limit = 0; // no time limit by default

  void handle_signal(int i)
  {
    char message[255];
    snprintf(message, 255, "Signal (%d) received: %s", i, strsignal(i));
    std::cout << message << std::endl;
    signal_returned = i;
    siglongjmp(jmp_env, 1);
  }

  void 
  trap_signals()
  {
    signal(SIGSEGV, handle_signal); // segmentation violation
    signal(SIGBUS, handle_signal);  // bus error
    signal(SIGABRT, handle_signal); // abort
    signal(SIGALRM, handle_signal); // timeout
    signal_returned = 0;
  }

  void
  untrap_signals()
  {
    signal(SIGSEGV, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
    signal(SIGALRM, SIG_DFL); // timeout
  }

  string encode_entities(const std::string&); // defined in I3TestMain.ixx

  test_failure::test_failure(const string& file_, unsigned line_, 
			     const string& predicate_, const string& message_) 
    : I3TestException(message_), 
      line(line_), file(file_), predicate(predicate_), message(message_) { }
  
  string test_failure::to_string(const string& name) const 
  {
    ostringstream os;
    os << file << ":" << line << ": FAIL\n"
       << "\n\tFile:      " << file 
       << "\n\tLine:      " << line 
       << "\n\tPredicate: " << predicate 
       << "\n\tMessage:   " << message << "\n" << endl;
    return os.str();
  }

  void 
  test_group::run(const string &unit, bool xml)
  {
    if (unit.empty())
      runtests();
    else
      {
	map<string, voidfunc>::iterator i = units.find(unit);
	if (i == units.end())
	  {
	    list("");
	    throw I3TestException(string("no such unit test \"") + unit + "\"\nUse --help to see help.");
	  }

	if (fork_units)
	  child_pid = fork();
	else
	  child_pid = 0; // we are always the child

	if (child_pid) // parent
	  {
	    int child_status;
	    wait(&child_status);
	    if (child_status == 0)
	      successes.insert(unit);
	    else
	      failures[i->first] = boost::shared_ptr<test_failure>(new test_failure(unit, -1, "fork", "Forked child failed"));
	      // Note: text in above never examined
	  }
	else
	  {
	    if (xml)
	      rout << "<unit>\n\t<name>" << unit << "</name>\n\t"; // don't close tag: attributes specifies pass/fail/output
	    else
	      {
		cout << boost::format("  %-60s") % boost::io::group(std::setfill('.'), unit);
		cout.flush();
	      }
	    ostringstream oss;
	    streambuf* cout_buf = cout.rdbuf();
	    streambuf* cerr_buf = cerr.rdbuf();
	    if (!show_spew)
	      {
		cout.rdbuf(oss.rdbuf());
		cerr.rdbuf(oss.rdbuf());
	      }
	    try {
	      //
	      // this next call actually runs one test routine
	      //
	      //		sigset_t alarm_set;
	      //		sigemptyset(&alarm_set);
	      //		sigaddset(&alarm_set, SIGALRM);
	      trap_signals();
	      //		sigprocmask(SIG_UNBLOCK, &alarm_set, 0);
	      if (! sigsetjmp(jmp_env, 1))
		{
		  alarm(unit_test_time_limit);
		  i->second();
		}
	      alarm(0);
	      untrap_signals();
	      if (signal_returned)
		{
		  char message[255];
		  if (signal_returned == SIGALRM)
		    {
		      snprintf(message, 255, "TIMEOUT: test killed by I3Test after %d seconds.", 
			       unit_test_time_limit);
		      throw test_failure("unknown", 0, "Test timed out.", message);
		    }
		  else
		    {
		      snprintf(message, 255, "Signal (%d) received: %s", 
			       signal_returned, strsignal(signal_returned));
		      throw test_failure("unknown", 0, "unexpected signal received", message);
		    }
		}
	      // restore cout/cerr
	      cout.rdbuf(cout_buf);
	      cerr.rdbuf(cerr_buf);

	      //	  cout << " success in " << unit << endl;
	      //	  cout << "captured: >>>>"<<  oss.str() << "<<<<" << endl;
	      successes.insert(unit);

	      string output=oss.str();

	      if (xml)
		rout << "<status>pass</status>\n\t<output>\n" << encode_entities(output) << "\n\t</output>\n</unit>" << endl;
	      else
		cout << "   ok" << endl;
	  
	    } catch (const test_failure& failure) {
	      untrap_signals();
	      cout.rdbuf(cout_buf);
	      cerr.rdbuf(cerr_buf);
	      failures[i->first] = boost::shared_ptr<test_failure>(new test_failure(failure));
	      if (xml)
		rout << "\n\t<status>fail</status>\n\t<file>" << failure.file << "</file>"
		     << "\n\t<line>" << failure.line << "</line>\n\t<predicate>" << encode_entities(failure.predicate) << "</predicate>"
		     << "\n\t<message>" << encode_entities(failure.message) << "</message>\n\t<output>" << encode_entities(oss.str()) << "\n\t</output>\n</unit>" << endl;
	      else
		cout << " FAIL\n" << failure.to_string(unit) << endl;
	    } catch (const exception& e) {
	      untrap_signals();
	      cout.rdbuf(cout_buf);
	      cerr.rdbuf(cerr_buf);
	      if (xml)
		rout << "<status>fail</status>\n\t<what>uncaught exception:\n" 
		     << encode_entities(e.what()) << "</what>\n\t<output>"
		     << encode_entities(oss.str()) << "</output>\n</unit>\n";
	      else
		cout << " UNCAUGHT:" << e.what() << endl;
	      failures[i->first] = boost::shared_ptr<test_failure>(new test_failure("?", 0, "uncaught exception", "uncaught exception"));
#ifndef NO_ICETRAY
	    } catch (const boost::python::error_already_set& e) {
	      PyErr_Print();
	      untrap_signals();
	      cout.rdbuf(cout_buf);
	      cerr.rdbuf(cerr_buf);
	      if (xml)
		rout << "<status>fail</status>\n\t<what>uncaught exception:\n" 
		     << "some python error" << "</what>\n\t<output>"
		     << encode_entities(oss.str()) << "</output>\n</unit>\n";
	      else
		; //cout << " UNCAUGHT:" << e.what() << endl;
	      failures[i->first] = boost::shared_ptr<test_failure>(new test_failure("?", 0, "uncaught exception", "uncaught exception"));
#endif
	    } catch (...) {
	      untrap_signals();
	      cout.rdbuf(cout_buf);
	      cerr.rdbuf(cerr_buf);
	      if (xml)
		rout << "<status>fail</status>\n\t<what>test framework error.\nCompletely unknown exception.</what>\n\t<output>\n"
		     << encode_entities(oss.str()) << "</output>\n</unit>\n";
	      else
		cout << " FATAL, something not a std::exception thrown." << endl;
	    }
	    if (fork_units)
	      exit(failures.find(i->first) != failures.end()); // child
	  }
      }
  }
  
  void 
  test_group::runtests(bool xml)
  {
    for (map<string, voidfunc>::iterator i = units.begin();
	 i != units.end();
	 i++)
      {
	run(i->first, xml);
      }
  }

  void 
  test_group::first(const string &s, const pair<string, voidfunc>& p) 
  {
    cout << s << "/" << p.first << endl;
  }

  void 
  test_group::list(const string& s)
  {
    for_each(units.begin(), units.end(), bind(first,s,_1));
  }


  void test_suite::runtests(bool xml) 
  {
    for (map<string, test_group*>::iterator i = groups.begin();
	 i != groups.end();
	 i++)
      {
	if (xml)
	  rout << "<group>\n\t<name>" << i->first << "</name>\n" << endl;
	else
	  cout << i->first << "..." << endl;
	
	i->second->runtests(xml);
	if (xml)
	  rout << "</group>" << endl;
      }
  }
  
  bool test_suite::report() 
  {
    unsigned successes = 0;
    unsigned failures = 0;

    for (map<string, test_group*>::iterator i = groups.begin();
	 i != groups.end();
	 i++)
      {
	successes += i->second->successes.size();
	failures += i->second->failures.size();
	//	  i->second->report(i->first);
      }

    cout << boost::format("%-67s") % boost::io::group(std::setfill('='), "");
    cout << "\nPass: " << successes << "\nFail: " << failures << "\n";
    if (failures){
      cout << " ***** THESE TESTS FAIL *****\n";
      for (map<string, test_group*>::iterator i = groups.begin();
           i != groups.end(); i++)
        for(std::map<std::string, boost::shared_ptr<test_failure> >::const_iterator
              failure = i->second->failures.begin();
            failure != i->second->failures.end();
            failure++)
          cout << "    " << i->first << '/' << failure->first << '\n';
      cout << endl;
    }

    if (failures)
      return true;
    else
      return false;
  }

  void 
  test_suite::first(const pair<string, test_group*>& p) 
  {
    p.second->list(p.first);
    cout << endl;
  }

  void 
  test_suite::list()
  {
    cout << "The part before the dot is the group name.\n"
	 << "You can run everything in the group by specifying that.\n"
	 << "You can specify and individual routine with groupname.unitname\n\n"
	 << "Tests available:\n";

    for_each(groups.begin(), groups.end(), first);
  }

  void 
  test_suite::make_dartfiles(const string& path)
  {
    cout << "Making dartfiles in directory " << path << "\n";

    for_each(groups.begin(), groups.end(), first);
  }

  void 
  test_suite::run (const string& s) 
  {
    string::size_type idx = s.find('/');
    string group_to_run = s.substr(0,idx);
    string unit_to_run = s.substr(idx+1);
    if (idx == string::npos)
      unit_to_run = "";
    
    cout << group_to_run << "...\n";

    map<string, test_group*>::iterator iter = groups.find(group_to_run);
    if (iter != groups.end())
      iter->second->run(unit_to_run);
    else
      throw I3TestException(string("Could not find test group \"") 
			    + group_to_run 
			    + "\"\nUse \"--help\" to see help.");
  }

  //
  // do conversions like & => &amp; so that an xml parser won't get
  // confused by messages and test suite spew
  //
  std::string encode_entities(const string &encode_me)
  {
    std::string xmlized;
    for (unsigned i=0; i<encode_me.length(); i++)
      {
	switch (encode_me[i])
	  {
	  case '<':
	    xmlized += "&lt;";
	    break;
	  case '>':
	    xmlized += "&gt;";
	    break;
	  case '&':
	    xmlized += "&amp;";
	    break;
	  case '\"':
	    xmlized += "&quot;";
	    break;
	  case '\'':
	    xmlized += "&apos;";
	    break;
	  default:
	    xmlized.push_back(encode_me[i]);
	  }
      }
    return xmlized;
  }

  test_suite& suite() {
    static test_suite the_suite;
    return the_suite;
  }

  test_registerer::test_registerer(const string& group, const string& unit, voidfunc f)
  {
    if (suite().groups.find(group) == suite().groups.end())
      throw std::runtime_error("bad initialization order?  group is not there");
    suite().groups[group]->units[unit] = f;
  }

  group_registerer::group_registerer(test_group* group, const string& name)
  {
    if (!group)
      {
	cout << "attempt to register null group!" << endl;
	throw std::runtime_error("attempt to register null group");
      }
    suite().groups[name] = group;
  }

}
vector<string> tests_to_run;

#ifndef NO_ICETRAY
void validate(boost::any &v, const std::vector<std::string>& values,
    I3LogLevel* target_type, int)
{
    using namespace boost::program_options;
    
    // Make sure no previous assignment to 'a' was made.
    validators::check_first_occurrence(v);
    // Extract the first string from 'values'. If there is more than
    // one string, it's an error, and exception will be thrown.
    const string& s = validators::get_single_string(values);
    std::string supper;
    std::transform(s.begin(), s.end(), std::back_inserter(supper), ::toupper);
    
    if (supper == "TRACE")
        v = boost::any(I3LOG_TRACE);
    else if (supper == "DEBUG")
        v = boost::any(I3LOG_DEBUG);
    else if (supper == "INFO")
        v = boost::any(I3LOG_INFO);
    else if (supper == "NOTICE")
        v = boost::any(I3LOG_NOTICE);
    else if (supper == "WARN")
        v = boost::any(I3LOG_WARN);
    else if (supper == "ERROR")
        v = boost::any(I3LOG_ERROR);
    else if (supper == "FATAL")
        v = boost::any(I3LOG_FATAL);
    else
#if BOOST_VERSION < 104200
        throw validation_error("invalid value");
#else
        throw validation_error(validation_error::invalid_option_value);
#endif
}
#endif //!NO_ICETRAY

namespace po = boost::program_options;

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
  return os;
}

po::options_description options("I3 Test Suite Options Summary");

void usage() {
  cerr << options << "\n"
       << "You may also specify a list of tests to run, of the form:\n"
       << "groupname1.testname1 groupname2.testname2 ... groupnameN.testnameN\n"
       << "Or specify only a groupname to run that entire group.\n\n";
}

int main(int argc, char* argv[])
{
  string xmlfile;
  string dartpath;
#ifndef NO_ICETRAY
  I3::init_icetray_lib();
#endif
  try {
    options.add_options()
      ("help,h", "this message")
      ("fork,f", "fork() before each test (all tests will run even if one dumps core)")
      ("all,a", "run all tests")
      ("xml,x", po::value< string >(&xmlfile), "run all tests, output xml to FILE (implies --fork and --timeout 600)")
      ("make-dartfiles,m", po::value< string >(&dartpath), "make dartfiles in directory PATH")
      ("show-spew,s", "pass along any thing the test spews to cout/cerr (you want to see it)")
      ("list,l", "list tests and groups in this suite")
      ("run-tests,r", po::value< vector<string> >(), "list of tests to run")
      ("timeout,t", po::value< int >(), "timeout unit tests after this many seconds")
#ifndef NO_ICETRAY
      ("log-level", po::value<I3LogLevel>(), "set the global logging level")
#endif
      ;

    po::positional_options_description p;
    p.add("run-tests", -1);

    po::variables_map vm;

    try {
      po::store(po::command_line_parser(argc, argv)
		.options(options)
		.positional(p).run(), vm);

      po::notify(vm);
    } catch (const std::exception& e) {
      cout << argv[0] << ": " << e.what() << "\n";
      usage();
      return 1;
    }

    if (argc == 1)
      {
	usage();
	return 1;
      }

    if (vm.count("help")) {
      usage();
      return 1;
    }

    if (vm.count("show-spew"))
      show_spew = true;

    if (vm.count("fork"))
      fork_units = true;

    if (vm.count("timeout")) {
      unit_test_time_limit = vm["timeout"].as< int >();
    }
#ifndef NO_ICETRAY
    if (vm.count("log-level")) {
        if (I3LoggerPtr logger = GetIcetrayLogger()) {
            logger->SetLogLevel(vm["log-level"].as<I3LogLevel>());
        }
    }
#endif
    if (vm.count("all")) {
      cout << "Running all tests:" << endl;
      suite().runtests();
      if (suite().report())
	return 1;
      else
	return 0;
    }

    if (vm.count("xml")) {
      if (!vm.count("timeout"))
	unit_test_time_limit = 450; // 7.5 minutes
      fork_units = true;
      rout.open(xmlfile.c_str());
      suite().runtests(true);
      return 0; // this always succeeds... it's for snowblower
    }

    if (vm.count("list")) {
      suite().list();
      return 0;
    }

    if (vm.count("make-dartfiles"))
      {
	suite().make_dartfiles(vm["make-dartfiles"].as<string>());
	return 0;
      }

    if (vm.count("run-tests")) 
      {
	const vector<string>& tests =  vm["run-tests"].as< vector<string> >();
	string first = *(tests.begin());
	//
	// Legacy interface
	//
	if (first == "help") {
	  usage();
	  return 1;
	}
	else if (first == "list") {
	  suite().list();
	  return 1;
	}
	else if (first == "all") {
	  suite().runtests();
	} else {
	  for (vector<string>::const_iterator i = tests.begin();
	       i!=tests.end();
	       i++)
	    {
	      suite().run(*i);
	    }
	}
	cout << "\nTest Results:\n";
	if (suite().report())
	  {
	    cout << "Returning 1\n";
	    return 1;
	  }
	else
	  {
	    cout << "Returning 0\n";
	    return 0;
	  }
      }
  } catch (const std::exception& e) {
    cerr << "Uncaught Exception: " << e.what() << endl;
    return 1;
  } catch (const I3Test::I3TestException& e) {
    cerr << "I3TestException: " << e.what() << endl;
    return 1;
  } catch (...) {
    cerr << "Miserable error.  No idea what kind of exception was thrown.  Contact offlinebug@icecube.umd.edu\n";
    return 1;
  }

}

