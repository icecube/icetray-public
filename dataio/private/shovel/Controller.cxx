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
#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/load_project.h>

#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/ref.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <fstream>
#include <boost/assign/list_inserter.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <shovel/color.h>

#include <shovel/View.h>
#include <shovel/Model.h>
#include <shovel/shovelrc.h>

#include <dlfcn.h>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::lambda;
using boost::lambda::bind;
using boost::ref;

using namespace boost::assign;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

void
shovel_usage(const std::string& progname)
{
  cerr << "Usage: " << progname << " [options] file.i3 [proj1 proj2 ... projN]\n"
       << "\n"
       << "  file.i3             path to the .i3 file to browse.\n\n"
       << "  [proj1 ... projN]   (optional) load these projects'\n"
       << "                      libraries at startup.\n"
       << "\n\nOptions:\n"
       << "  --no_trayinfo, -n   Do not display/count trayinfo frames\n"
       << "  --frames=N, -f N    Scan/Display only N frames\n\n"
       << "Example:\n  " << progname << " -f 100 -n data.i3 muon-llh-reco linefit dipolefit\n"
       << endl;
  exit(1);
}

void
shovel_exit()
{
  exit(0);
}

void
write_default_shovelrc(const std::string path, const map<char, string>& keybindings)
{
  ofstream ofs(path.c_str());
  ofs << "#\n# dataio-shovel keybindings\n#\n";

  for (map<char, string>::const_iterator iter = keybindings.begin();
       iter!=keybindings.end(); iter++)
    {
      ofs << iter->first << " = " << iter->second << "\n";
      ;
    }
  ofs << "\n\n";
  ofs.close();
}


map<char, string> keybindings;

int main (int argc, char *argv[])
{
  I3::init_icetray_lib();

  log_trace("%s", __PRETTY_FUNCTION__);
  po::options_description opt("options");

  opt.add_options()
    ("help,?", "this message")
    ("i3file", "the .i3 file")
    ("frames,f", po::value<unsigned>(), "scan only this many frames of file")
    ("no_trayinfo,n", "skip trayinfo frames")
    ("projects", po::value< vector<string> >(), "libs to load before opening .i3 file")
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
    cout << argv[0] << ": " << e.what() << "\n";
    shovel_usage(argv[0]);
    return 1;
  }

  insert(keybindings)
    ('k', "up")
    ('[', "fast_reverse")
    ('<', "first_frame")
    ('h',"left")
    ('l',"right")
    (']', "fast_forward")
    ('>', "last_frame")
    ('j',"down")
    ('?',"help")
    ('a',"about")
    ('g',"goto_frame")
    ('p',"pretty_print")
    ('s',"save_xml")
    ('t',"toggle_infoframes")
    ('w',"write_frame")
    ('x',"xml")
    ('q',"quit");

  string rcfile_path = getenv("HOME");
  rcfile_path += "/.shovelrc";
  if (!fs::exists(fs::path(rcfile_path, fs::no_check)))
    write_default_shovelrc(rcfile_path, keybindings);

  dataio::shovel::parse_rcfile(rcfile_path, keybindings);

  insert(keybindings)
    (KEY_ENTER, "xml")
    (KEY_UP, "up")
    (KEY_DOWN, "down")
    (KEY_LEFT, "left")
    (KEY_RIGHT, "right")
    (KEY_PPAGE, "fast_reverse")
    (KEY_NPAGE, "fast_forward");

  if (vm.count("help"))
    {
      shovel_usage(argv[0]);
      return 1;
    }

  if (vm.count("projects"))
    {
      cout << "Loading project libraries: ";
      cout.flush();
      const vector<string>& projects = vm["projects"].as< vector<string> >();
      for (unsigned i=0; i<projects.size(); i++)
        {
          cout << "[" << projects[i] << "] ";
          cout.flush();
          if (load_project(projects[i]))
            shovel_usage(argv[0]);
        }
      cout << " done." << endl;
    }

  string thefile;
  if (vm.count("i3file"))
    thefile = vm["i3file"].as<string>();

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

    vector<I3Frame::Stream> skipvector;

    if (vm.count("no_trayinfo"))
      skipvector.push_back(I3Frame::TrayInfo);

    model.open_file(thefile,
                    skipvector,
                    vm.count("frames") ? vm["frames"].as<unsigned>() : boost::optional<unsigned>()
                    );

    model.notify();
    log_trace("done starting up");
    map<string, boost::function<void(void)> > actions;

    actions["up"] = bind(&Model::move_y, ref(model), -1);
    actions["down"] = bind(&Model::move_y, ref(model), 1);
    actions["right"] = bind(&Model::move_x, ref(model), 1);
    actions["left"] = bind(&Model::move_x, ref(model), -1);
    // specify boost::lamba::bind, otherwise something odd happens with
    // that call to var()
    actions["fast_forward"] = boost::lambda::bind(&Model::move_x, ref(model), var(COLS)-4);
    actions["fast_reverse"] = boost::lambda::bind(&Model::move_x, ref(model), -(var(COLS)-4));
    actions["first_frame"] = bind(&Model::move_first, ref(model));
    actions["last_frame"] = bind(&Model::move_last, ref(model));
    actions["help"] = bind(&View::do_help, ref(View::Instance()));
    actions["about"] = bind(&View::do_about, ref(View::Instance()));
    actions["pretty_print"] = bind(&Model::pretty_print, ref(model));
    actions["toggle_infoframes"] = bind(&Model::toggle_infoframes, ref(model));
    actions["write_frame"] = bind(&Model::write_frame, ref(model));
    actions["save_xml"] = bind(&Model::save_xml, ref(model));
    actions["goto_frame"] = bind(&Model::do_goto_frame, ref(model));
    actions["xml"] = bind(&Model::show_xml, ref(model));
    actions["quit"] = &shovel_exit;

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
#endif

        // skip if key not bound
        if (keybindings.find(c) == keybindings.end())
          continue;

        std::string action = keybindings[(char)c];

        if (actions.find(action) == actions.end())
          {
            endwin();
            log_fatal("unable to find code for keystroke \'%c\', action %s",
                      c, action.c_str());
          }

        actions[action]();

        model.notify();
      }
  } catch (const std::exception& e) {
    clear();
    endwin();
    throw;
  }
}

