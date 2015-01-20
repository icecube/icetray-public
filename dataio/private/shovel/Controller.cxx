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

#include <algorithm>
#include <map>
#include <string>
#include <fstream>

#include <boost/function.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/program_options.hpp>

#include <shovel/color.h>
#include <shovel/View.h>
#include <shovel/Model.h>

#include <dlfcn.h>

using namespace std;

using namespace boost::assign;
namespace po = boost::program_options;

//simple implementation of lambda functions with zero or one parameter
//single argument case
template<typename TargetType, typename ArgType, typename ActualArgType=ArgType>
struct simple_lambda{
  typedef void(TargetType::*FuncType)(ArgType);
  TargetType& target;
  FuncType func;
  ActualArgType arg;
  simple_lambda(TargetType& t, FuncType f, ActualArgType a):target(t),func(f),arg(a){}
  void operator()(){ (target.*func)(ArgType(arg)); }
};
//no argument case
template<typename TargetType>
struct simple_lambda<TargetType,void>{
  typedef void(TargetType::*FuncType)();
  TargetType& target;
  FuncType func;
  simple_lambda(TargetType& t, FuncType f):target(t),func(f){}
  void operator()(){ (target.*func)(); }
};
//helper functions for making lambdas
template<typename TargetType, typename ArgType, typename ActualArgType>
simple_lambda<TargetType,ArgType,ActualArgType>
make_lambda(TargetType& t, void(TargetType::*f)(ArgType), ActualArgType a){
  return(simple_lambda<TargetType,ArgType,ActualArgType>(t,f,a));
}
template<typename TargetType>
simple_lambda<TargetType,void>
make_lambda(TargetType& t, void(TargetType::*f)()){
  return(simple_lambda<TargetType,void>(t,f));
}
//little wrapper object for figuring out how much to fast forward
struct fast_forwarder{
  bool reverse; //whether to go backward or forward
  explicit fast_forwarder(bool r):reverse(r){}
  operator int() const{ return((COLS/2-2)*(reverse?-1:1)); }
};

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

    actions["up"] = make_lambda(model,&Model::move_y,-1);
    actions["down"] = make_lambda(model,&Model::move_y,1);
    actions["right"] = make_lambda(model,&Model::move_x,1);
    actions["left"] = make_lambda(model,&Model::move_x,-1);
    fast_forwarder ff(false),fr(true);
    actions["fast_forward"]=make_lambda(model,&Model::move_x,ff);
    actions["fast_reverse"] = make_lambda(model,&Model::move_x,fr);
    actions["first_frame"] = make_lambda(model,&Model::move_first);
    actions["last_frame"] = make_lambda(model,&Model::move_last);
    actions["help"] = make_lambda(View::Instance(),&View::do_help);
    actions["about"] = make_lambda(View::Instance(),&View::do_about);
    actions["pretty_print"] = make_lambda(model,&Model::pretty_print);
    actions["toggle_infoframes"] = make_lambda(model,&Model::toggle_infoframes);
    actions["write_frame"] = make_lambda(model,&Model::write_frame);
    actions["save_xml"] = make_lambda(model,&Model::save_xml);
    actions["goto_frame"] = make_lambda(model,&Model::do_goto_frame);
    actions["xml"] = make_lambda(model,&Model::show_xml);

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
  } catch (const std::exception& e) {
    clear();
    endwin();
    throw;
  }
}
