/**
 *  $Id: View.h 165886 2018-10-01 14:37:58Z nwhitehorn $
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
#ifndef DATAIO_SHOVEL_VIEW_H_INCLUDED
#define DATAIO_SHOVEL_VIEW_H_INCLUDED

#include <ncurses.h>
#include <boost/optional.hpp>
#include <icetray/I3Frame.h>

#include <cdk/cdk.h>

class Model;

class View
{

  unsigned totalframes_;
  unsigned y_top_offset_;
  CDKSCREEN* cdkscreen;
  CDKHISTOGRAM* progresshist_;
  bool scanning_;
  Model* model_;

  void drawtape(unsigned line, unsigned col, I3Frame::Stream stream, 
		unsigned frameno, int attr = A_NORMAL);

  void draw_border();

private:

  View();

public:
  
  ~View();
  void model(Model* themodel) { model_ = themodel; }
  void notify();
  void start();
  void update();
  void page(const std::string& what);

  void totalframes(unsigned total) { totalframes_ = total; }

  void display_frame(I3FramePtr frame, unsigned index, unsigned y_selected);

  void do_help();
  void do_about();
  void start_scan_progress(const std::string& filename);
  void scan_progress(double d);
  void end_scan_progress();

  template<typename T>
  boost::optional<T> dialog(const std::string& prompt);

  boost::optional<std::string> get_file(const std::string& prompt);

  void usage();

  static View& Instance();

};



#endif


