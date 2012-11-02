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

void usage(int, char** argv) {
  cout << argv[0] << ":\n"
       << "Scans an .i3 file with the I3File class\n"
       << "(used by glshovel and dataio-shovel)\n";
  cout << options << "\n";
}


int main(int argc, char* argv[])
{
  I3::init_icetray_lib();
  options.add_options()
    ("help,h", "this message")
    ("input-file,i", po::value< string >(), "input file")
    ("whole-frames,w", "Print whole frames")
    ("streams-only,s", "Print only stream types")
    ("count,c", "Print total number of frames")
    ;

  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv)
	      .options(options)
	      .run(), vm);

    po::notify(vm);
  } catch (const std::exception& e) {
    std::cout << "error: " << e.what() << "\n";
    usage(argc,argv);
    return 1;
  }

  if (argc == 1)
    {
      usage(argc, argv);
      return 1;
    }

  if (vm.count("help")) {
    usage(argc, argv);
    return 1;
  }

  std::string ifilename;
  if (!vm.count("input-file"))
    {
      usage(argc, argv);
      return 1;
    }
  ifilename = vm["input-file"].as<string>();
  boost::iostreams::filtering_istream ifs;
  I3::dataio::open(ifs, ifilename);
      
  I3File i3f;
  i3f.open_file(ifilename);

  if(vm.count("count"))
    std::cout << i3f.size() << "\n";
  
  if(vm.count("streams-only"))
    {
      for (size_t i=0; i<i3f.size(); i++)
	{
	  I3FramePtr frame = i3f.get_raw_frame(i);
	  std::cout << frame->GetStop().id();
	}
      std::cout << "\n";
    }
  if(vm.count("whole-frames"))
    {
      for (size_t i=0; i<i3f.size(); i++)
	{
	  I3FramePtr frame = i3f.get_raw_frame(i);
	  std::cout << *frame << "\n";
	}
      std::cout << "\n";
    }

  std::cout << "done.\n";
}

