/**
 *  $Id$
 *  
 *  Copyright (C) 2007, 2008, 2009
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

#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/I3Time.h>

#include <limits>
#include <iostream>
#include <iomanip>

#include <form.h>
#include <fstream>
#include <signal.h>
#if defined(__APPLE_CC__) && !defined(TIOCGWINSZ)
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
#include <boost/format.hpp>
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
  #ifndef NDEBUG
  if (curs_set(0) == -1)
     log_debug("Sorry, invisible cursor not supported on this terminal");
  #endif

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

  colors_[I3Frame::Geometry] = cyan;
  colors_[I3Frame::Calibration] = cyan;
  colors_[I3Frame::DetectorStatus] = cyan;
  colors_[I3Frame::TrayInfo] = magenta;

  colors_[I3Frame::DAQ] = white;
  colors_[I3Frame::Physics] = white; // these get colored by subeventstream

  subeventstream_colors_[""] = blue;
  subeventstream_colors_["NullSplit"] = red;
  subeventstream_colors_["IceTopSplit"] = yellow;
  subeventstream_colors_["InIceSplit"] = green;
    
  // colors for new stream names
  new_subeventstream_colors_.push_back(blue);
  new_subeventstream_colors_.push_back(red);
  new_subeventstream_colors_.push_back(yellow);
  new_subeventstream_colors_.push_back(green);
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
  if(!of.good()){
    unlink(tmpfile_name);
    log_fatal("Error writing to /tmp/shovel-XXXX file.  (Maybe xml version of too large?)");
  }
  assert(of.good());
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
View::drawtape(unsigned line, unsigned col, I3Frame::Stream stream, std::string sub_event_stream, unsigned frameno, int attr)
{
  settext(dim_green);
  mvaddch(line, col, ' '|attr);

  if (attr)
    settext(hi_white);
  else
    settext(dim_white);

  if ((frameno+1)%5 == 0)
    mvaddch(line-1, col, '.');

  if ((frameno+1)%10 == 0)
    mvaddch(line-1, col, '|');

  if ((frameno+1) % 20 == 0)
    {
      ostringstream oss;
      oss << frameno+1;
      mvaddstr(line-2, col - oss.str().length()/2, oss.str().c_str());
    }

  color_pair the_color;  
  if (colors_.find(stream) != colors_.end()) {
    the_color = colors_[stream];
  } else {
    the_color = dim_white;
  }

  // if this is a Physics frame, it will get colored
  // by SubEventStream
  if (stream == I3Frame::Physics) {
    std::map<std::string, color_pair>::const_iterator it =
      subeventstream_colors_.find(sub_event_stream);
    if (it == subeventstream_colors_.end()) {
      // make a new color
      the_color = new_subeventstream_colors_[subeventstream_colors_.size() % 4];
      subeventstream_colors_[sub_event_stream] = the_color;
    } else {
      the_color = it->second;
    }
  }
    
  settext(the_color);
  mvaddch(line, col, stream.id() | attr);
}

void 
View::display_frame(I3FramePtr frame, unsigned index, unsigned y_selected)
{
  erase();
  draw_border();
  standend();

  vector<string> keys, the_keys = frame->keys();


  settext(hi_red);
  mvaddstr(2, 2, "Name");
  mvaddstr(2, COLS/3, "Type");
  mvaddstr(2, COLS-14, "Bytes");

  unsigned frame_window_height = LINES - 11;

  if (y_selected >= (frame_window_height + y_top_offset_))
    y_top_offset_ = y_selected - (frame_window_height-1);
  if (y_selected < y_top_offset_)
    y_top_offset_ = y_selected;

  // generate the list of keys we're actually going to display
  string selected_key;
  unsigned skip = 0, count = 0;


  vector<string>::iterator iter = the_keys.begin();
  while (iter != the_keys.end() && count < frame_window_height)
    {
      if (skip >= y_top_offset_)
	{
	  keys.push_back(*iter);
	  count++;
	}
      if (skip == y_selected)
	selected_key = *iter;
      iter++; 
      skip++;
    }
  bool more = (iter != the_keys.end());

  count = 0;

  for (vector<string>::iterator iter = keys.begin();
       iter != keys.end(); iter++, count++)
    {
      I3Frame::typename_iterator frameiter = frame->typename_find(*iter);
      assert(frameiter != frame->typename_end());
      const string &key = frameiter->first;
      const string &type_name = frameiter->second;
      const bool on_foreign_stream = frame->GetStop(*iter) != frame->GetStop();

      if (key == selected_key) {
          if (on_foreign_stream)
              settext(ColorParse("rev_dim_yellow"));
          else
              settext(rev_white);
      } else {
          if (on_foreign_stream)
              settext(dim_yellow);
          else
              settext(white);
      }

      
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

      mvaddstr(count + 3, 2, short_key.c_str());
      mvaddstr(count + 3, COLS/3, short_typename.c_str());
      ostringstream oss;
      oss << frame->size(*iter);
      mvaddstr(count + 3, COLS-14, oss.str().c_str());
    }

  settext(yellow);

  if (more)
    mvaddstr(count+3, COLS/3, "[scroll down for more]");

  //
  //  Draw 'longitudinal' status
  // 
  settext(hi_red);
  mvaddstr(LINES-6, 2, "      Key:");

  settext(white);
  {
    ostringstream oss;
    oss << (the_keys.size() > 0 ? y_selected+1 : 0) << "/" << frame->size();
    settext(yellow);
    mvaddstr(LINES-6, 13, oss.str().c_str());
  }

  settext(hi_red);
  mvaddstr(LINES-5, 2, "    Frame:");
  standend();
  settext(white);
  std::size_t statuslen=16;
  {
    ostringstream oss;
    oss << index+1 << "/" << model_->totalframes() 
	<< " (" << (unsigned) (100 * (float)(index+1)/(float)model_->totalframes()) << "%)";  
    settext(yellow);
    mvaddstr(LINES-5, 13, oss.str().c_str());
    statuslen = std::max(oss.str().size(), statuslen);
  }

  settext(hi_red);
  mvaddstr(LINES-4, 2, "     Stop:");
  settext(yellow);
  mvaddstr(LINES-4, 13, frame->GetStop().str().c_str());
  statuslen = std::max(frame->GetStop().str().size(), statuslen);

  {
    I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(I3DefaultName<I3EventHeader>::value());
    // no mixed-in items here:
    if ((header) && (frame->GetStop(I3DefaultName<I3EventHeader>::value()) != frame->GetStop())) header.reset();
    
    settext(hi_red);
    mvaddstr(LINES-3, 2, "Run/Event:");
    if (!header) {
      settext(dim_white);
      mvaddstr(LINES-3, 13, "(n/a)");
    } else {
      settext(yellow);
      ostringstream oss;
      oss << header->GetRunID() << "/" << header->GetEventID();
      mvaddstr(LINES-3, 13, oss.str().c_str());
      statuslen = std::max(oss.str().size(), statuslen);
    }

    settext(hi_red);
    mvaddstr(LINES-2, 2, " SubEvent:");
    if (!header || frame->GetStop() != I3Frame::Physics || header->GetSubEventStream() == "") {
      settext(dim_white);
      mvaddstr(LINES-2, 13, "(n/a)");
    } else {
      settext(yellow);
      ostringstream oss;
      oss << header->GetSubEventStream() << "/" << header->GetSubEventID();
      mvaddstr(LINES-2, 13, oss.str().c_str());
      statuslen = std::max(oss.str().size(), statuslen);
    }
      
    I3Time startTime;
    I3Time endTime;
    bool has_start_time=false;
    bool has_end_time=false;
      
    if (header) {
      startTime = header->GetStartTime();
      endTime = header->GetEndTime();
      has_start_time=true;
      has_end_time=true;
    } else {
      I3TimeConstPtr frameStartTime = frame->Get<I3TimeConstPtr>("StartTime");
      I3TimeConstPtr frameEndTime = frame->Get<I3TimeConstPtr>("EndTime");
      // no mixed-in items here:
      if ((frameStartTime) && (frame->GetStop("StartTime") != frame->GetStop())) frameStartTime.reset();
      if ((frameEndTime) && (frame->GetStop("EndTime") != frame->GetStop())) frameEndTime.reset();
        
      if (frameStartTime) {
        startTime = *frameStartTime;
        has_start_time=true;
      }

      if (frameEndTime) {
        endTime = *frameEndTime;
        has_end_time=true;
      }
    }
      
    settext(hi_red);
    mvaddstr(LINES-6, COLS - 36, "StartTime:");
    mvaddstr(LINES-5, COLS - 36, " Duration:");

    if (!has_start_time) {
      settext(dim_white);
      mvaddstr(LINES-6, COLS - 36 + 11, "(n/a)");
    } else {
      settext(yellow);
      ostringstream oss;
      oss.precision(1);
      oss << startTime.GetUTCString();
      mvaddstr(LINES-6, COLS - 36 + 11, oss.str().c_str());
    }

    if (!has_end_time) {
      settext(dim_white);
      mvaddstr(LINES-5, COLS - 36 + 11, "(n/a)");
    } else {
      settext(yellow);
      ostringstream oss;
      oss << endTime - startTime << " ns";
      mvaddstr(LINES-5, COLS - 36 + 11, oss.str().c_str());
    }

  }    


  vector<I3Frame::Stream> streams;
  vector<std::string> sub_event_streams;

  int tape_head_column = COLS/2;
  int tape_head_index = index;
  //  log_trace("tape_head_column=%u tape_head_index=%u", tape_head_column, tape_head_index);

  // draw to the right
  int tape_r_column = COLS-2;
  int length_r = tape_r_column - tape_head_column;
  if (tape_head_index + length_r >= (int)model_->totalframes())
    length_r = model_->totalframes() - tape_head_index;

  streams = model_->streams(tape_head_index, length_r);
  sub_event_streams = model_->sub_event_streams(tape_head_index, length_r);
  for (int i=0; i<length_r; i++)
    {
      drawtape(LINES-2, tape_head_column+i, streams[i], sub_event_streams[i], tape_head_index+i);
    }

  //
  // draw tape to the left, clip it 
  //
  int tape_l_column = 2;
  int length_l = tape_head_column - tape_l_column - statuslen - 18;
  if (length_l < 0)
    length_l = 0;
  if (tape_head_index - length_l <= 0)
    length_l = tape_head_index;
  //  log_trace("length_l = %d", length_l);

  streams = model_->streams(tape_head_index-length_l, length_l);
  sub_event_streams = model_->sub_event_streams(tape_head_index-length_l, length_l);
  for (int i=0; i < length_l; i++)
    {
      drawtape(LINES-2, tape_head_column-(length_l-i), streams[i], sub_event_streams[i], tape_head_index-length_l+i);
    }

  // draw the head
  streams = model_->streams(tape_head_index, 1);
  sub_event_streams = model_->sub_event_streams(tape_head_index, 1);
  drawtape(LINES-2, tape_head_column, streams[0], sub_event_streams[0], tape_head_index, A_REVERSE);
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
x                Serialize selected frame to XML (show with less)\n\
s                Serialize selected frame to XML and save to file\n\
p                Pretty-print selected item (currently I3TrayInfo only)\n\
w                Write entire frame (binary) to file\n\
q                Quit\n\
a                About\n\
g                Go to frame\n\
t                Toggle display of TrayInfo frames\n\
\n\
The 'tape' display at the bottom shows activity on each of IceTrays's data\n\
'streams'.\n\
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

  about_txt += "Written 2006-2009 for the IceCube Collaboration",
    "By Troy D. Straszheim <troy@icecube.umd.edu>";

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
  mvhline(LINES-7, 1, ACS_HLINE, COLS-2);

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
  std::string file = getString(cdkscreen, const_cast<char*>(msg.c_str()), 0, 0);
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
				  -6, // width (full minus 6)
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
