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
#include <vector>
using namespace std;

#include <boost/assign.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/std/set.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/ref.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <sys/wait.h>
#include <setjmp.h>

#include <boost/program_options.hpp>
#include <iterator>
#include <iomanip>
#include <string.h>
#include <signal.h>
#include <dlfcn.h>

#include <dataio/FrameIO.h>

#include <icetray/serialization.h>
#include <icetray/Utility.h>
#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Frame.h>
#include <icetray/load_project.h>

namespace po = boost::program_options;

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
  return os;
}

po::options_description options("dataio-extract Options Summary");

void usage() {
  cerr << options << "\n";
}


int main(int argc, char* argv[])
{
  options.add_options()
    ("help,h", "this message")
    ("output-file,o", po::value< string >(), "output file")
    ("input-file,i", po::value< string >(), "input file")
    ("keys,k", po::value< vector<string> >(), "list of keys to xmlize")
    ("nframes,n", po::value< unsigned >(), "number of frames to display")
    ("pretty,p", "pretty-print frames (key:type only, no frame object contents)")
    ("load-libs,l", po::value< vector<string> >(), "list of libraries to load")
    ;

  po::positional_options_description p;
  p.add("keep", -1);

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

  //  bool include = false;
  //  bool exclude = false;

  if (argc == 1)
    {
      usage();
      return 1;
    }

  if (vm.count("help")) {
    usage();
    return 1;
  }

  unsigned nframes = 0;
  if (vm.count("nframes"))
    nframes = vm["nframes"].as<unsigned>();

  std::string ifilename;
  if (!vm.count("input-file"))
    {
      usage();
      return 1;
    }
  ifilename = vm["input-file"].as<string>();
  boost::iostreams::filtering_istream ifs;
  I3::dataio::open(ifs, ifilename);
      
  std::string ofilename;
  if (!vm.count("output-file"))
    {
      usage();
      return 1;
    }
  ofilename = vm["output-file"].as<string>();
  ofstream ofs(ofilename.c_str(), ios::binary);

  assert(ofs.good());

  set<std::string> keyset;

  if (vm.count("keys")) 
    {
      const vector<string>& keys =  vm["keys"].as< vector<string> >();
      cerr << "Will extract frame objects with following keys: ";
      for (unsigned i=0; i<keys.size(); i++)
	{
	  cerr << keys[i] << " ";
	  keyset.insert(keys[i]);
	}
      cerr << endl;
    }

  if (vm.count("load-libs"))
    {
      cerr << "Loading libraries: ";
      cerr.flush();
      const vector<string>& projects = vm["load-libs"].as< vector<string> >();
      for (unsigned i=0; i<projects.size(); i++)
	{
	  cerr << "[" << projects[i] << "] ";
	  cerr.flush();
	  if (load_project(projects[i]))
	    {
	      usage();
	      exit(1);
	    }
	}
      cerr << " done." << endl;
    }

  cerr << "Extracting from " << ifilename << "..." << endl;

  unsigned frameno = 0;
  static const char* spinners = "\\|/-";

  while (ifs.peek() != EOF && nframes && (frameno < nframes))
    {
      frameno++;
      
      if (frameno % 10 == 0)
	{
	  cerr << "\r" << spinners[int(frameno / 10) % 4];
	  cerr.flush();
	}
      
      I3Frame f;

      try {
	f.load(ifs);
      } catch (const std::exception& e) {
	log_fatal("caught exception %s", e.what());
	break;
      }

      if (vm.count("pretty"))
	{
	  std::cout << f;
	  continue;
	}

      for (set<string>::const_iterator key_iter = keyset.begin();
	   key_iter != keyset.end();
	   key_iter++)
	{
	  if (!f.Has(*key_iter))
	    continue;

	  ofs << prettify_xml(f.as_xml(*key_iter));
	}
    }
  ofs.flush();
  ofs.close();
  cerr << endl;
}

