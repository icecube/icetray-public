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
#ifndef DATAIO_SHOVEL_MODEL_H_INCLUDED
#define DATAIO_SHOVEL_MODEL_H_INCLUDED

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <boost/optional.hpp>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Frame.h>
#include <dataio/I3File.h>

class View;

class Model
{
  I3File i3file_;

  std::vector<std::pair<I3File::FrameInfo, unsigned> > frame_infos_;
  std::vector<std::pair<I3File::FrameInfo, unsigned> > frame_infos_other_;

  View& view_;

  std::ifstream ifs_;

  unsigned x_index_;
  unsigned y_index_;
  unsigned y_max_;
  std::string y_keystring_;

  I3FramePtr get_frame(unsigned);

public:
  
  Model(View& view);

  int open_file(const std::string& filename, 
		boost::optional<vector<I3Frame::Stream> > skipstreams 
		= boost::optional<vector<I3Frame::Stream> >(),
		boost::optional<unsigned> nframes = boost::optional<unsigned>()
		);

  void show_xml();
  void pretty_print();

  void move_first();
  void move_last();
  void toggle_infoframes();

  void move_x(int delta);
  void move_y(int delta);
  void do_goto_frame();
  void goto_frame(unsigned frameno);
  void write_frame();

  void notify();

  unsigned totalframes();

  std::vector<I3Frame::Stream> streams(unsigned start_index, unsigned length);
 
};



#endif


