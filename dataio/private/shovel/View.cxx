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
#include <form.h>
#include <fstream>
#include <signal.h>
#ifdef __APPLE_CC__
#define TIOCGWINSZ
#endif

#include <sys/ioctl.h>

#include "color.h"
#include "View.h"
#include "Model.h"

#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

#include <icetray/Utility.h>

#include <ncurses.h>
#include <cdk/cdk.h>

using namespace boost::assign;
using namespace std;
using boost::optional;

static 
void finish(int sig)
{
  endwin();

  /* do your non-curses wrapup here */

  exit(0);
}

static 
void resize(int sig)
{
  //  log_trace(__PRETTY_FUNCTION__);

  // apparently we have to call the ioctls and update the globals
  // ourselves....  Wish I knew that earlier, woulda done things
  // different.

#ifdef TIOCGSIZE
    struct ttysize win;
#else
#ifdef TIOCGWINSZ
    struct winsize win;
#endif /* TIOCGWINSZ */
#endif /* TIOCGSIZE */

#ifdef TIOCGSIZE
    if (ioctl(0, TIOCGSIZE, &win) == 0) {
        if (win.ts_lines != 0) {
            LINES = win.ts_lines;
        }
        if (win.ts_cols != 0) {
            COLS = win.ts_cols;
        }
    }
#else
#ifdef TIOCGWINSZ
    if (ioctl(0, TIOCGWINSZ, &win) == 0) {
        if (win.ws_row != 0) {
            LINES = win.ws_row;
        }
        if (win.ws_col != 0) {
            COLS = win.ws_col;
        }
    }
#endif /* TIOCGWINSZ */
#endif /* TIOCGSIZE */

    //  ungetch('%');
  //  log_trace("ioctl: %d x %d", LINES, COLS);
  resizeterm(LINES,COLS);
  erase();
  touchwin(stdscr);
  standend();
  endwin();
  View::Instance().notify();
  refresh();
  getch();
}

void
View::start() 
{ 
  signal(SIGINT, finish);      /* arrange interrupts to terminate */
  initscr();      /* initialize the curses library */
  keypad(stdscr, TRUE);  /* enable keyboard mapping */
  /*  nonl();          tell curses not to do NL->CR/NL on output */
  cbreak();       /* take input chars one at a time, no wait for \n */
  noecho();       /* don't echo input */
  curs_set(0);

  /* Declare the labels. */
  cdkscreen = initCDKScreen(stdscr);
  initCDKColor();

  start_color();

  init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);

  y_top_offset_ = 0;
  signal(SIGWINCH, resize);

}

View::View()
{
}

View::~View()
{
  finish(0);
}

void 
View::page(const std::string &text)
{
  char tmpfile_name[] = "/tmp/shovel-XXXXXX";
  int fd = mkstemp(tmpfile_name);
  assert (fd != -1);
  close(fd);
  unlink(tmpfile_name);
  ofstream of(tmpfile_name);
  assert(of.good());
  of << text;
  of.close();
  
  def_prog_mode();
  endwin();
  std::string cmd = "/usr/bin/less -x2 -C ";
  cmd += tmpfile_name;
  system(cmd.c_str());
  unlink(tmpfile_name);
  refresh();
}

void
View::drawtape(unsigned line, unsigned col, I3Frame::Stream stream, unsigned frameno, int attr)
{
  settext(dim_magenta);
  mvaddch(line-3, col, ' '|attr);
  settext(dim_cyan);
  mvaddch(line-2, col, ' '|attr);
  settext(dim_yellow);
  mvaddch(line-1, col, ' '|attr);
  settext(dim_green);
  mvaddch(line, col, ' '|attr);
  
  if (attr)
    settext(hi_white);
  else
    settext(dim_white);

  if ((frameno+1)%5 == 0)
    mvaddch(line-4, col, '.');

  if ((frameno+1)%10 == 0)
    mvaddch(line-4, col, '|');

  if ((frameno+1) % 20 == 0)
    {
      ostringstream oss;
      oss << frameno+1;
      mvaddstr(line-5, col - oss.str().length()/2, oss.str().c_str());
    }

  int lineoffset=0;
  if (stream == I3Frame::None)
    settext(red);
  if (stream == I3Frame::Geometry)
    settext(magenta), lineoffset = -3;
  if (stream == I3Frame::Calibration)
    settext(cyan), lineoffset = -2;
  if (stream == I3Frame::DetectorStatus)
    settext(yellow), lineoffset = -1;
  if (stream == I3Frame::Physics)
    settext(green);

  mvaddch(line + lineoffset, col, stream.id() | attr);

  if (stream == I3Frame::TrayInfo)
    {
      settext(hi_red), lineoffset = -3;
      mvaddch(line + lineoffset, col, ACS_VLINE | attr);
      settext(hi_red), lineoffset = -2;
      mvaddch(line + lineoffset, col, ACS_VLINE | attr);
      settext(hi_red), lineoffset = -1;
      mvaddch(line + lineoffset, col, ACS_VLINE | attr);
      settext(hi_red), lineoffset = 0;
      mvaddch(line + lineoffset, col, ACS_VLINE | attr);
    }
}

void 
View::display_frame(I3FramePtr frame, unsigned index, unsigned y_selected)
{
  //  log_trace("display frame index=%u y_selected=%u", index, y_selected);
  erase();
  draw_border();
  standend();

  settext(hi_red);
  mvaddstr(2, 2, "Frame:");
  standend();
  settext(white);
  {
    ostringstream oss;
    oss << index+1 << "/" << model_->totalframes();
    mvaddstr(2, 9, oss.str().c_str());
  }
  settext(hi_red);
  mvaddstr(3, 2, "Key:");

  settext(white);
  {
    ostringstream oss;
    oss << y_selected+1 << "/" << frame->size();
    mvaddstr(3, 9, oss.str().c_str());
  }

  settext(hi_red);
  mvaddstr(3, COLS/3, "Type");
  mvaddstr(3, COLS-14, "Size (bytes)");

  unsigned frame_window_height = LINES - 12;

  if (y_selected >= (frame_window_height + y_top_offset_))
    y_top_offset_ = y_selected - (frame_window_height-1);
  if (y_selected < y_top_offset_)
    y_top_offset_ = y_selected;

  // log_trace("y_selected = %u, frame_window_height = %u, y_top_offset_ = %u",
  // y_selected, frame_window_height, y_top_offset_);

  // generate the list of keys we're actually going to display
  vector<string> keys, the_keys = frame->keys();
  string selected_key;
  unsigned skip = 0, count = 0;

  for (vector<string>::iterator iter = the_keys.begin();
       iter != the_keys.end() && count < frame_window_height;
       iter++, skip++)
    {
      if (skip >= y_top_offset_)
	{
	  keys.push_back(*iter);
	  count++;
	}
      if (skip == y_selected)
	selected_key = *iter;
    }

  count = 0;

  for (vector<string>::iterator iter = keys.begin();
       iter != keys.end(); iter++, count++)
    {
      I3Frame::typename_iterator frameiter = frame->typename_find(*iter);
      assert(frameiter != frame->typename_end());
      const string &key = frameiter->first;
      const string &type_name = frameiter->second;

      if (key == selected_key)
	settext(rev_white);
      else
	settext(white);
      
      unsigned maxkeylen = COLS/3 - 2 - 1;
      string short_key = key;
      // if it is too long, shorten it
      if (key.length() > maxkeylen)
	{
	  short_key = key.substr(0, maxkeylen - 3);
	  short_key += "...";
	}

      unsigned maxtypelen = COLS-14 - COLS/3 - 1;
      string short_typename = stlfilt(type_name);
      // if it is too long, shorten it
      if (short_typename.length() > maxtypelen)
	{
	  short_typename = short_typename.substr(0, maxtypelen - 3);
	  short_typename += "...";
	}

      mvaddstr(count + 4, 2, short_key.c_str());
      mvaddstr(count + 4, COLS/3, short_typename.c_str());
      ostringstream oss;
      oss << frame->size(*iter);
      mvaddstr(count + 4, COLS-14, oss.str().c_str());
    }

  vector<I3Frame::Stream> streams;

  int tape_head_column = COLS/2;
  int tape_head_index = index;
  //  log_trace("tape_head_column=%u tape_head_index=%u", tape_head_column, tape_head_index);

  // draw to the right
  int tape_r_column = COLS-2;
  int length_r = tape_r_column - tape_head_column;
  if (tape_head_index + length_r >= (int)model_->totalframes())
    length_r = model_->totalframes() - tape_head_index;

  streams = model_->streams(tape_head_index, length_r);
  for (int i=0; i<length_r; i++)
    {
      drawtape(LINES-2, tape_head_column+i, streams[i], tape_head_index+i);
    }

  // draw to the left
  int tape_l_column = 2;
  int length_l = tape_head_column - tape_l_column - 1;
  if (tape_head_index - length_l <= 0)
    length_l = tape_head_index;
  //  log_trace("length_l = %d", length_l);

  streams = model_->streams(tape_head_index-length_l, length_l);
  for (int i=0; i < length_l; i++)
    {
      drawtape(LINES-2, tape_head_column-(length_l-i), streams[i], tape_head_index-length_l+i);
    }

  // draw the head
  streams = model_->streams(tape_head_index, 1);
  drawtape(LINES-2, tape_head_column, streams[0], tape_head_index, A_REVERSE);
  standend();
}

void
View::do_help()
{
  std::string the_help ="\
Key              Action\n\
============================================================\n\
?                This help.\n\
left/right       Previous/next frame\n\
pgup/pgdn        Back/forward a screenful of frames\n\
[/]              \n\
up/down          Select previous/next frame item\n\
k/j              \n\
<                First frame\n\
>                Last frame\n\
x                Serialize selected frame to XML\n\
p                Pretty-print selected item (currently I3TrayInfo only)\n\
w                Write frame (binary) to file\n\
q                Quit\n\
a                About\n\
g                Go to frame\n\
t                Toggle display of TrayInfo frames\n\
\n\
The 'tape' display at the bottom shows activity on each of Icecube's data\n\
'streams':\n\
  G:  Geometry\n\
  C:  Calibration\n\
  D:  Detector Status\n\
  P:  Physics (event data)\n\
\n\
And vertical lines represent splices (tray configurations).\n\
\n\
If you don't like these keybindings you can customize them in $HOME/shovelrc.\n\
This message does/will not reflect any customizations.\n\
";
  page(the_help);
}

template <typename T>
struct form_traits
{ };

template <>
struct form_traits<unsigned>
{
  static void configure_field(FIELD* field)
  {
    set_field_type(field, TYPE_INTEGER, 0, 1, std::numeric_limits<int>::max());
  }

  static unsigned convert(const char* buf)
  {
    return atoi(buf);
  }
};

template <>
struct form_traits<std::string>
{
  static void configure_field(FIELD* field)
  {
    set_field_type(field, TYPE_REGEXP, "^.*$");
  }
  static std::string convert(const char* buf)
  {
    string s = buf;
    boost::trim(s);
    return s;
  }
};

template <typename T>
optional<T>
View::dialog(const std::string& prompt)
{
#if 0
  /* Declare variables. */
  CDKLABEL *stopSign	= 0;
  CDKLABEL *title	= 0;
  WINDOW *cursesWin	= 0;
  int currentLight	= 0;
  char *mesg[5], *sign[4];
  chtype key;
  boolean functionKey;

  /* Set up CDK. */
  //   cursesWin = initscr();
  //   cdkscreen = initCDKScreen (cursesWin);

  /* Start CDK Colors. */
  //initCDKColor();

  /* Set the labels up. */
  mesg[0] = "<C><#HL(40)>";
  mesg[1] = "<C>Press </B/16>r<!B!16> for the </B/16>red light";
  mesg[2] = "<C>Press </B/32>y<!B!32> for the </B/32>yellow light";
  mesg[3] = "<C>Press </B/24>g<!B!24> for the </B/24>green light";
  mesg[4] = "<C><#HL(40)>";
  sign[0] = " <#DI> ";
  sign[1] = " <#DI> ";
  sign[2] = " <#DI> ";

  assert(cdkscreen);
  title = newCDKLabel (cdkscreen, CENTER, TOP, mesg, 5, FALSE, FALSE);
  stopSign = newCDKLabel (cdkscreen, CENTER, CENTER, sign, 3, TRUE, TRUE);

  /* Do this until they hit q or escape. */
  for (;;)
    {
      drawCDKLabel (title, FALSE);
      drawCDKLabel (stopSign, TRUE);

      key = getchCDKObject (ObjOf(stopSign), &functionKey);
      if (key == KEY_ESC || key == 'q' || key == 'Q')
	{
	  break;
	}
      else if (key == 'r' || key == 'R')
	{
	  sign[0] = " </B/16><#DI> ";
	  sign[1] = " o ";
	  sign[2] = " o ";
	  currentLight = 0;
	}
      else if (key == 'y' || key == 'Y')
	{
	  sign[0] = " o ";
	  sign[1] = " </B/32><#DI> ";
	  sign[2] = " o ";
	  currentLight = 1;
	}
      else if (key == 'g' || key == 'G')
	{
	  sign[0] = " o ";
	  sign[1] = " o ";
	  sign[2] = " </B/24><#DI> ";
	  currentLight = 2;
	}

      /* Set the contents of the label and re-draw it. */
      setCDKLabel (stopSign, sign, 3, TRUE);
    }

  /* Clean up. */
  destroyCDKLabel (title);
  destroyCDKLabel (stopSign);
  //   destroyCDKScreen (cdkscreen);

  //   endCDK();
  //   ExitProgram (EXIT_SUCCESS);
#endif

  //  log_trace(__PRETTY_FUNCTION__);
  FIELD* field[2];

  char value[20];
  memset(value, 0, 20);
  
  field[0] = new_field(1,10,0,2,0,1);
  set_field_back(field[0], A_UNDERLINE);
  form_traits<T>::configure_field(field[0]);
  set_field_buffer(field[0], 0, value);
  field[1] = NULL;
  //  field_opts_off(field[0], O_AUTOSKIP);
  
  FORM*  form;
  form = new_form(field);
  //  log_trace("form=%p", form);

  WINDOW* win = newwin(3, 32, 5, 4);
  //  log_trace("win=%p", win);

  set_form_win(form, win);
  set_form_sub(form, derwin(win, 1, 30, 1, 1));

  settext(win, hi_blue);
  box(win, ACS_VLINE, ACS_HLINE);
  settext(win, hi_yellow);
  mvwaddstr(win, 0, 2, prompt.c_str());

  post_form(form);
  wrefresh(win);

  while(true)
    {
      int ch = wgetch(win);
      //      log_info("<%c>", ch);
      if (ch == '\n')
	break;

      switch(ch)
	{
	case '\b':
	  form_driver(form, REQ_DEL_PREV);
	  //	  form_driver(form, REQ_PREV_CHAR);
	  break;
	default:
	  form_driver(form, ch);
	}
      wrefresh(win);
    }
  form_driver(form, REQ_VALIDATION);

  //  log_trace("value is %s", value);
  //  log_trace("field_buffer is %s", field_buffer(field[0], 0));

  T rv = form_traits<T>::convert(field_buffer(field[0], 0));

  unpost_form(form);
  free_form(form);
  free_field(field[0]);

  delwin(win);
  //  log_trace("END %s", __PRETTY_FUNCTION__);
  return rv;
}

template boost::optional<unsigned> View::dialog<unsigned>(const std::string&);
template boost::optional<std::string> View::dialog<std::string>(const std::string&);

void
View::do_about()
{
  erase();
  draw_border();
  
  vector<string> about_txt;

  about_txt += "written in 2006 for the icecube collaboration",
    "by troy d. straszheim <troy@icecube.umd.edu>";

  settext(white);
  for (unsigned i=0; i<about_txt.size(); i++)
    {
      mvaddstr(LINES/3 + i,
	       (COLS/2) - (about_txt[i].length() /2),
	       about_txt[i].c_str());
    }

  mvaddstr(LINES-2,3,"Hit any key to return...");
  getch();
}

void
View::usage()
{
  cerr << "Usage:  dataio-shovel <file.i3>\n";
  exit(0);
}

void 
View::draw_border()
{
  settext(hi_blue);
  box(stdscr, ACS_VLINE, ACS_HLINE);

  settext(dim_blue);
  mvhline(LINES-8, 1, ACS_HLINE, COLS-2);

  settext(hi_yellow);
  mvaddstr(0, 2, " I3 Data Shovel ");

  string pressforhelp = " Press '?' for help ";
  settext(dim_white);
  mvaddstr(0, COLS-3-pressforhelp.length(), pressforhelp.c_str());
}

void
View::notify()
{
  if (!scanning_)
    model_->notify();
}

View&
View::Instance()
{
  static View the_view;
  return the_view;
}

boost::optional<string>
View::get_file(const std::string& msg)
{
  std::string file = selectFile(cdkscreen, const_cast<char*>(msg.c_str()));
  return file;
}

void
View::start_scan_progress(const std::string& filename)
{
  log_trace("%s", __PRETTY_FUNCTION__);
  scanning_ = true;
  string msg = "Scanning " + filename;

  progresshist_ = newCDKHistogram(cdkscreen, 
				  CENTER,  // xpos
				  CENTER,  // ypos
				  1, // height
				  -6, // width (full minus 4)
				  HORIZONTAL, // orientation
				  const_cast<char*>(msg.c_str()), // label
				  true, // box
				  false); // shadow

  setCDKHistogram(progresshist_,
		  vPERCENT,//type
		  CENTER,// stats position
		  A_BOLD, // stats attr
		  0, 100, 0, '=', // filler
		  false); // box
		  
  drawCDKHistogram(progresshist_, true);

}

void 
View::scan_progress(double d)
{
  log_trace("scan_progress(%f)", d);
  setCDKHistogramValue(progresshist_, 0, 100, (int)d);
  drawCDKHistogram(progresshist_, true);
}

void
View::end_scan_progress()
{
  destroyCDKHistogram(progresshist_);
  scanning_ = false;
}
