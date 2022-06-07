/**
 *  $Id$
 *
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */
#include <cstdlib>
#include <ncurses.h>
#include <signal.h>
#include <unistd.h>

#include <icetray/I3Tray.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/load_project.h>
#include <icetray/I3SimpleLoggers.h>

#include <algorithm>
#include <map>
#include <string>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/python.hpp>

#include <dataio/I3FileStager.h>

#include <shovel/color.h>
#include <shovel/View.h>
#include <shovel/Model.h>

#include <dlfcn.h>

namespace po = boost::program_options;

void
shovel_usage(const std::string& progname)
{
  std::cerr << "Usage: " << progname << " [options] file.i3 [proj1 proj2 ... projN]\n"
       << "\n"
       << "  file.i3 [file2.i3 ...]  path to the .i3 file to browse.\n"
       << "\nOptions:\n"
       << "  --frames=N, -f N                  Scan only N frames on startup\n"
       << "  --projects=project, -l project    Load these project's\n"
       << "                                    libraries on startup\n"
       << "  --log                             Write debug information to a log file\n\n"
       << "Example:\n  " << progname << " -f 100 -l linefit -l dipolefit data.i3\n"
       << std::endl;
  exit(1);
}

void
do_pyshell(char* argv[], Model& model, View& view){
  struct suspend_curses{
    View& view;
    suspend_curses(View& view):view(view){
      clear();
      endwin();
    }
    ~suspend_curses(){
      view.notify();
      refresh();
    }
  } suspender(view);
  
  namespace bp = boost::python;
  wchar_t program[255];
  mbstowcs(program, argv[0], 255);
  Py_SetProgramName(program);
  Py_Initialize();
  // #2405: segfault with this and python 3.7+
  //        It doesn't appear to be required, so commented out for now
  //PySys_SetArgvEx(1, (wchar_t**)&program, 0);
  PySys_SetArgvEx(0, NULL, 0);

  //load things users will probably need
  bp::dict ns;
  try{
    ns["icetray"]=bp::import("icecube.icetray");
    ns["dataclasses"]=bp::import("icecube.dataclasses");
    ns["dataio"]=bp::import("icecube.dataio");
  }
  catch( bp::error_already_set& e ){
    log_error_stream("Problem loading icecube python bindings:\n"
                     << "Are there unbuilt pybindings?");
    PyErr_Clear();
    return;
  }
  try{ ns["recclasses"]=bp::import("icecube.recclasses"); }
  catch( bp::error_already_set& e){ PyErr_Clear(); }
  try{ ns["simclasses"]=bp::import("icecube.simclasses"); }
  catch( bp::error_already_set& e ){ PyErr_Clear(); }
  //inject the frame
  ns["frame"]=model.current_frame();

  bp::object main_module = bp::import("__main__");
  bp::dict main_ns = bp::extract<bp::dict>(main_module.attr("__dict__"));
  main_ns.update(ns);

  try{
    //try to use IPython if it exists
    bp::object ipython_embed=bp::import("IPython");
    bp::tuple arguments;
    bp::dict options;
    options["user_ns"] = ns; // need this for first invocation
    ipython_embed.attr("start_ipython")(*arguments,**options);
  }
  catch( bp::error_already_set& e ){
    log_warn("Unable to load IPython");
    PyErr_Clear();

    //try using the regular python shell

    //handle things like arrow keys properly
    try{ bp::import("readline"); }
    catch( bp::error_already_set& e ){ PyErr_Clear(); }

    bp::object code_module;
    try{ code_module=bp::import("code"); }
    catch( bp::error_already_set& e ){
      log_error("Unable to import 'code'");
      PyErr_Clear();
      return;
    }

    try{
      auto console=code_module.attr("InteractiveConsole")(main_ns);
      console.attr("interact")(R"(
      Interactive python shell
      The current frame is available as `frame`
      Press ^D to return to the dataio-shovel interface
      )");
    }catch( bp::error_already_set& e ){
      log_error("Python error");
      PyErr_Clear();
    }
  }
  
  // Not finalizing python, as that destroys the frame
}

///Attempt to load a project which may not be available
///\param proj the name of the project to load
///\return whether the project was loaded
bool try_load_project(const std::string& proj){
  try{
    load_project(proj);
  } catch(std::runtime_error& err){
    log_debug_stream("Failed to load project " << proj);
    return false;
  }
  return true;
}

int main (int argc, char *argv[])
{
  try{
    I3::init_icetray_lib();
  }catch (const boost::python::error_already_set& e) {
    clear();
    endwin();
    std::cerr << "Fatal python error:\n";
    PyErr_Print();
    return(1);
  }

  po::options_description opt("options");

  opt.add_options()
    ("help,?", "this message")
    ("i3file", po::value< std::vector<std::string> >(), "the .i3 file(s) to load")
    ("frames,f", po::value<unsigned>(), "scan only this many frames of file")
    ("projects,l", po::value< std::vector<std::string> >(), "libs to load before opening .i3 file")
    ("log", "write log messages to a file")
    ;

  po::positional_options_description popt;
  popt.add("i3file", -1);

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
    {'L',"load_project"},
    {'i',"interactive_shell"},
    {'e',"find_event"},
    {KEY_ENTER,"pretty_print"},
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

  try_load_project("simclasses");
  try_load_project("recclasses");
  
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

  std::vector<std::string> files;
  if (vm.count("i3file"))
    files=vm["i3file"].as<std::vector<std::string>>();
  if(files.empty())
    shovel_usage(argv[0]);

  log_trace("starting view");

  // whole thing is blocked with a try/catch.. the catch
  // just endwin()s and rethrows
  try {
    View::Instance().start();
    
    log_trace("opening file(s)");
    Model model(View::Instance(),files,
                vm.count("frames") ? vm["frames"].as<unsigned>() : boost::optional<unsigned>());

    log_trace("setting model");
    View::Instance().model(&model);


    model.notify();
    log_trace("done starting up");
    std::map<std::string, std::function<void(void)>> actions;

    actions["up"] = [&]{model.move_y(-1);};
    actions["down"] = [&]{model.move_y(1);};
    actions["right"] = [&]{model.move_x(1);};
    actions["left"] = [&]{model.move_x(-1);};
    actions["fast_forward"] = [&]{model.move_x(COLS/2-2);};
    actions["fast_reverse"] = [&]{model.move_x(-(COLS/2-2));};
    actions["fast_up"] = [&]{model.move_y(-(LINES-12));};
    actions["fast_down"] = [&]{model.move_y(LINES-12);};
    actions["first_frame"] = [&]{model.move_first();};
    actions["last_frame"] = [&]{model.move_last();};
    actions["help"] = []{View::Instance().do_help();};
    actions["about"] = []{View::Instance().do_about();};
    actions["pretty_print"] = [&]{model.pretty_print();};
    actions["write_frame"] = [&]{model.write_frame();};
    actions["write_frame_with_dependencies"] = [&]{model.write_frame_with_dependencies();};
    actions["save_xml"] = [&]{model.save_xml();};
    actions["goto_frame"] = [&]{model.do_goto_frame();};
    actions["find_event"] = [&]{model.do_find_event();};
    actions["xml"] = [&]{model.show_xml();};
    actions["load_project"] = [&]{
      boost::optional<std::string> proj =
        View::Instance().dialog<std::string>("  Project to load: ");
      if(proj){
        try{
          load_project(*proj);
        }catch(std::runtime_error& err){
          log_error_stream(err.what());
        }
      }
    };
    actions["interactive_shell"] = [&]{ do_pyshell(argv,model,View::Instance()); };

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
        if (c==0x01)
          c = KEY_HOME;
        else if (c==0x05)
          c = KEY_END;
        else if (c==0x0A)
          c = KEY_ENTER;

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
