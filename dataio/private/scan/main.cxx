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

#include <icetray/open.h>

#include <icetray/serialization.h>
#include <icetray/Utility.h>
#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Frame.h>
#include <icetray/load_project.h>

#include <dataio/I3File.h>

namespace po = boost::program_options;

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(cout, " ")); 
  return os;
}

po::options_description options("dataio-strip Options Summary");

void usage() {
  cerr << options << "\n";
}


int main(int argc, char* argv[])
{
  options.add_options()
    ("help,h", "this message")
    ("input-file,i", po::value< string >(), "input file")
    ;

  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv)
	      .options(options)
	      .run(), vm);

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

  std::string ifilename;
  if (!vm.count("input-file"))
    {
      usage();
      return 1;
    }
  ifilename = vm["input-file"].as<string>();
  boost::iostreams::filtering_istream ifs;
  I3::dataio::open(ifs, ifilename);
      
  std::cout << "scanning " << ifilename << "...\n";
  I3File i3f;
  i3f.open_file(ifilename);
  std::cout << "done.\n";
}

