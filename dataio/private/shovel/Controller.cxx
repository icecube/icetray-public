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
#include <cstdlib>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/load_project.h>

#include <algorithm>
#include <map>
#include <string>
#include <fstream>

#include <boost/program_options.hpp>

#include <shovel/color.h>
#include <shovel/View.h>
#include <shovel/Model.h>

#include <dlfcn.h>

namespace po = boost::program_options;

class I3FileLogger : public I3Logger{
private:
  std::string path;
  std::ofstream out;
  bool TrimFileNames;
public:
  explicit I3FileLogger(std::string path):path(path),out(path.c_str()),TrimFileNames(true){
    if(!out)
      throw std::runtime_error("Failed to open "+path+" for logging");
  }
  
  void Log(I3LogLevel level, const std::string &unit,
           const std::string &file, int line, const std::string &func,
           const std::string &message)
  {
    if (LogLevelForUnit(unit) > level)
      return;
    std::string trimmed_filename;
	size_t lastslash = file.rfind('/');
	if (lastslash != std::string::npos && TrimFileNames)
      trimmed_filename = file.substr(lastslash+1);
	else
      trimmed_filename = file;
    switch (level) {
      case I3LOG_TRACE:
		out << "TRACE";
		break;
      case I3LOG_DEBUG:
		out << "DEBUG";
		break;
      case I3LOG_INFO:
		out << "INFO";
		break;
      case I3LOG_NOTICE:
        out << "NOTICE";
        break;
      case I3LOG_WARN:
		out << "WARN";
		break;
      case I3LOG_ERROR:
		out << "ERROR";
		break;
      case I3LOG_FATAL:
		out << "FATAL";
		break;
      default:
		out << "UNKNOWN";
		break;
	}
    out << " (" << unit << "):" << message << " (" << trimmed_filename << ':'
    << line << " in " << func << ')' << std::endl;
  }
};

void
shovel_usage(const std::string& progname)
{
  std::cerr << "Usage: " << progname << " [options] file.i3 [proj1 proj2 ... projN]\n"
       << "\n"
       << "  file.i3             path to the .i3 file to browse.\n\n"
       << "  [proj1 ... projN]   (optional) load these projects'\n"
       << "                      libraries at startup.\n"
       << "\n\nOptions:\n"
       << "  --frames=N, -f N    Scan/Display only N frames\n\n"
       << "Example:\n  " << progname << " -f 100 -n data.i3 muon-llh-reco linefit dipolefit\n"
       << std::endl;
  exit(1);
}

int main (int argc, char *argv[])
{
  I3::init_icetray_lib();

  po::options_description opt("options");

  opt.add_options()
    ("help,?", "this message")
    ("i3file", "the .i3 file")
    ("frames,f", po::value<unsigned>(), "scan only this many frames of file")
    ("projects", po::value< std::vector<std::string> >(), "libs to load before opening .i3 file")
    ("log", "write log messages to a file")
    ;

  po::positional_options_description popt;
  popt.add("i3file", 1);
  popt.add("projects", -1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv)
              .options(opt)
              .positional(popt)
              .run(), vm);
    po::notify(vm);
  } catch (const std::exception& e) {
    std::cout << argv[0] << ": " << e.what() << "\n";
    shovel_usage(argv[0]);
    return 1;
  }
  
  std::map<int, std::string> keybindings={
    {'k',"up"},
    {'[',"fast_reverse"},
    {'{',"first_frame"},
    {'h',"left"},
    {'l',"right"},
    {']',"fast_forward"},
    {'}',"last_frame"},
    {'-',"fast_up"},
    {'=',"fast_down"},
    {'j',"down"},
    {'?',"help"},
    {'a',"about"},
    {'g',"goto_frame"},
    {'p',"pretty_print"},
    {'s',"save_xml"},
    {'w',"write_frame"},
    {'W',"write_frame_with_dependencies"},
    {'x',"xml"},
    {'q',"quit"},
    {KEY_ENTER,"xml"},
    {KEY_UP,   "up"},
    {KEY_DOWN, "down"},
    {KEY_LEFT, "left"},
    {KEY_RIGHT,"right"},
    {KEY_HOME, "first_frame"},
    {KEY_END,  "last_frame"},
    {KEY_PPAGE,"fast_up"},
    {KEY_NPAGE,"fast_down"}
  };

  if (vm.count("help"))
    {
      shovel_usage(argv[0]);
      return 1;
    }
  
  if (vm.count("log"))
    {
      SetIcetrayLogger(boost::make_shared<I3FileLogger>("dataio-shovel.log"));
      GetIcetrayLogger()->SetLogLevelForUnit("dataio-shovel::Model",I3LOG_INFO);
    }
  else
    {
      GetIcetrayLogger()->SetLogLevelForUnit("dataio-shovel::Model",I3LOG_WARN);
    }

  if (vm.count("projects"))
    {
      std::cout << "Loading project libraries: ";
      std::cout.flush();
      const std::vector<std::string>& projects = vm["projects"].as<std::vector<std::string>>();
      for (unsigned i=0; i<projects.size(); i++)
        {
          std::cout << "[" << projects[i] << "] ";
          std::cout.flush();
          if (load_project(projects[i]))
            shovel_usage(argv[0]);
        }
      std::cout << " done." << std::endl;
    }

  std::string thefile;
  if (vm.count("i3file"))
    thefile = vm["i3file"].as<std::string>();

  if (!thefile.length())
    shovel_usage(argv[0]);

  Model model(View::Instance());

  log_trace("starting view");

  // whole thing is blocked with a try/catch.. the catch
  // just endwin()s and rethrows
  try {

    View::Instance().start();
    log_trace("setting model");
    View::Instance().model(&model);

    log_trace("opening file");

    model.open_file(thefile,
                    vm.count("frames") ? vm["frames"].as<unsigned>() : boost::optional<unsigned>()
                    );

    model.notify();
    log_trace("done starting up");
    std::map<std::string, std::function<void(void)>> actions;

    actions["up"] = [&](){model.move_y(-1);};
    actions["down"] = [&](){model.move_y(1);};
    actions["right"] = [&](){model.move_x(1);};
    actions["left"] = [&](){model.move_x(-1);};
    actions["fast_forward"] = [&](){model.move_x(COLS/2-2);};
    actions["fast_reverse"] = [&](){model.move_x(-(COLS/2-2));};
    actions["fast_up"] = [&](){model.move_y(-(LINES-12));};
    actions["fast_down"] = [&](){model.move_y(LINES-12);};
    actions["first_frame"] = [&](){model.move_first();};
    actions["last_frame"] = [&](){model.move_last();};
    actions["help"] = [](){View::Instance().do_help();};
    actions["about"] = [](){View::Instance().do_about();};
    actions["pretty_print"] = [&](){model.pretty_print();};
    actions["write_frame"] = [&](){model.write_frame();};
    actions["write_frame_with_dependencies"] = [&](){model.write_frame_with_dependencies();};
    actions["save_xml"] = [&](){model.save_xml();};
    actions["goto_frame"] = [&](){model.do_goto_frame();};
    actions["xml"] = [&](){model.show_xml();};

    while (true)
      {
        int c = getch();     /* refresh, accept single keystroke of input */
#ifdef __APPLE__             /* add other keys to the swich below */
        if (c == 0x1B) {
          c = getch();
          if ((c == 0x4F) or (c == 0x5B)) {
            c = getch();
            switch (c) {
            case 0x41:
              c = KEY_UP;
              break;
            case 0x42:
              c = KEY_DOWN;
              break;
            case 0x43:
              c = KEY_RIGHT;
              break;
            case 0x44:
              c = KEY_LEFT;
              break;
            default:
              break;
            }
          }
        }
        else if (c==0x01)
          c = KEY_HOME;
        else if (c==0x05)
          c = KEY_END;
        else if (c==0x0A)
          c = KEY_ENTER;
#endif

        // skip if key not bound
        if (keybindings.find(c) == keybindings.end())
          continue;

        std::string action = keybindings[c];
        
        if (action == "quit")
          break;
        
        if (actions.find(action) == actions.end())
          {
            endwin();
            log_fatal("unable to find code for keystroke \'%c\', action %s",
                      c, action.c_str());
          }

        actions[action]();

        model.notify();
      }
  }
  catch (const boost::python::error_already_set& e) {
    clear();
    endwin();
    std::cerr << "Fatal python error:\n";
    PyErr_Print();
    return(1);
  }
  catch (const std::exception& e) {
    clear();
    endwin();
    std::cerr << "Caught exception:\n" << e.what() << '\n';
    return(1);
  }
}
