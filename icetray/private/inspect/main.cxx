/**
 *  $Id$
 *
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <icetray/I3Factory.h>

#include <icetray/serialization.h>
#include <icetray/load_project.h>

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
#include <boost/foreach.hpp>
#include <sys/wait.h>
#include <setjmp.h>

#include <boost/program_options.hpp>
#include <iterator>
#include <iomanip>
#include <string.h>
#include <signal.h>

#include <icetray/I3Context.h>
#include <icetray/I3Configuration.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>
#include <icetray/Utility.h>

#include "util.h"

bool xml = false;
bool with_svn = false;
unsigned modules_ok = 0;
unsigned modules_error = 0;

using namespace std;

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
  return os;
}
template <typename factory_t>
set<string>
get_classnames (const factory_t& factory)
{
  set<string> classnames;
  BOOST_FOREACH(typename factory_t::const_reference ref, factory)
    {
      classnames.insert(ref.first);
    }
  return classnames;
}

template <typename factory_t>
void
show_product_details(const string& project, const factory_t& factory)
{
  for (typename factory_t::const_iterator iter = factory.begin();
       iter != factory.end();
       iter++)
    {
      if (iter->second.project != project)
        continue;

      // This is all (sarcasm) brilliant stuff we have to do
      // because you can't find out what an T's configuration
      // space is without creating one.  In the days of
      // icetray+root-icetray this would have taken an entire
      // separate implementation of icetray and root-icetray.
      I3Context context;
      I3ConfigurationPtr config(new I3Configuration);
      config->ClassName(iter->first);
      context.Put(config);      

      // The following is necessary when creating
      // I3Modules... but not when creating I3ServiceFactories.
      shared_ptr<map<string, pair<FrameFifoPtr, I3ModulePtr> > > connections
        (new map<string, pair<FrameFifoPtr, I3ModulePtr> >);
      context.Put(connections, "OutBoxes");
      // (*connections)["OutBox"] = make_pair(FrameFifoPtr(), I3ModulePtr());

      try {
        // yeah, this is awful to have this here.  If we werent
        // forced to create objects to find out about their
        // configs, we wouldn't have to do this.
        if (iter->first == "I3TrayInfoServiceFactory")
          continue;

        factory.Create(iter->first)(context);

        const I3Configuration& filledconfig = context.template Get<I3Configuration>();
        if (!xml)
          {
            cout << filledconfig;
            cout << "---------------------------------------\n";          
          } 
        else 
          {
            cout << filledconfig.inspect();
          }
        modules_ok++;
      } catch (const std::exception& e) {
        cerr << "Error constructing \"" << iter->first << "\". Skipping.\n";
        modules_error++;
      } catch (const boost::python::error_already_set& e) {
        using namespace boost::python;
        cerr << "Error constructing \"" << iter->first << "\".\n"
             << "Caught exception from python:\n";

        std::string errtype, errvalue;

        PyObject *type = 0, *val = 0, *tb = 0;
        PyErr_Fetch(&type, &val, &tb);
        handle<> e_val(val), e_type(type), e_tb(allow_null(tb));

        try {
          object t = extract<object>(e_type.get());
          object t_name = t.attr("__name__");
          errtype = extract<std::string>(t_name);
        } catch (error_already_set const &) {
          cerr << "Internal error getting error type:\n";
          PyErr_Print();
        }

        try {
          object v = extract<object>(e_val.get());
          errvalue = extract<std::string>(v.attr("__str__")());
        } catch (error_already_set const &) {
          cerr << "Internal error getting value type:\n";
          PyErr_Print();
        }

        config->Add("*** Python Error ***", errtype, errvalue);
        if (!xml)
          {
            cout << *config;
            cout << "---------------------------------------\n";          
          } 
        else 
          {
            cout << config->inspect();
          }

        cerr << "Skipping.\n"; 
      }
    } 
}
namespace po = boost::program_options;
vector<string> libs_to_examine;
po::options_description options("I3 Library Inspector");

void usage() {
  cerr << options << "\n";
}

int 
main (int argc, char** argv)
{
  // I'm getting so sick of reclaiming my signals from ROOT.
  signal(SIGSEGV, SIG_DFL);
  signal(SIGBUS, SIG_DFL);
  signal(SIGINT, SIG_DFL);
  signal(SIGTSTP, SIG_DFL);
  signal(SIGHUP, SIG_DFL);
  signal(SIGCHLD, SIG_DFL);

  options.add_options()
    ("help,h", "this message")
    ("all,a", po::value<string>(), "Examine all available projects/libraries in DIRECTORY")
    ("xml,x", "Output xml")
    ("svn,s", "Show svn url/revision information (affects plaintext mode only)")
    ("projects,p", po::value< vector<string> >(), "Projects to inspect")
    ;

  po::positional_options_description p;
  p.add("projects", -1);

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
    
  if (vm.count("xml"))
    xml = true;

  if (xml)
    cout << "<?xml version='1.0'?>\n<icetray-inspect>\n";

  if (vm.count("svn"))
    with_svn = true;

  vector<string> projects;

  if (vm.count("all"))
    {
      string libdir = vm["all"].as<string>();
      if (libdir == "")
        usage();
      projects = all_projects(libdir);
    }
  else if (vm.count("projects"))
    projects = vm["projects"].as< vector<string> >();
  else
    usage();

  cerr << "Loading projects...";
  cerr.flush();

  for (unsigned i=0; i<projects.size(); i++)
    {
      cerr << "[" << projects[i] << "] ";
      cerr.flush();
      try {
        load_project(projects[i], false);
      } catch (const std::runtime_error& e) {
        cerr << "ignoring: " << e.what() << std::endl;
      } catch (const boost::python::error_already_set& e) {
        cerr << "Caught exception from python:\n";
        PyErr_Print();
      }
    }
  cerr << "\n";
      
  // not sure. ought one load all the libs, *then* get the
  // contents?  Or could you do it 
  for (unsigned i=0; i<projects.size(); i++)
    {
      if (xml)
        cout << "<project name=\"" << projects[i] << "\">\n";
      else
        cout << "*** " << projects[i] << " ***\n";
      show_product_details(projects[i], I3::Singleton<I3ModuleFactory>::get_const_instance());
      show_product_details(projects[i], I3::Singleton<I3ServiceFactoryFactory>::get_const_instance());
      if (xml)
        cout << "</project>\n";
      else
        cout << "\n";
    }

  if (xml)
    cout << "</icetray-inspect>\n";

  cerr << modules_ok << " module configurations reported.\n";
  if (modules_error)
    cerr << modules_error << " modules skipped due to error.\n";
}
