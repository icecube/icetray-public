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
#ifndef DATAIO_I3FILE_H_INCLUDED
#define DATAIO_I3FILE_H_INCLUDED

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <icetray/I3Frame.h>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>

class I3FileImpl;

using namespace std;

class I3File
{
  static void noop(double) { }
  boost::scoped_ptr<I3FileImpl> impl_;

public:

  struct empty_file_exception : virtual std::exception
  {
    virtual const char* what() const throw() {
      return ".i3 file has zero size";
    }
  };

  struct FrameInfo
  {
    I3Frame::Stream stream;
    typedef std::map<I3Frame::Stream, unsigned> stream_map_t;
    stream_map_t other_streams;

    std::ios::pos_type pos;
    FrameInfo() : stream(I3Frame::None) { }
  };

  I3File();
  ~I3File();

  int open_file(const std::string& filename,
                boost::function<void(double)> cb = noop,
                boost::optional<vector<I3Frame::Stream> > skipstreams = boost::optional<vector<I3Frame::Stream> >(),
                boost::optional<unsigned> nframes = boost::optional<unsigned>(),
                bool verbose = true);

  void async_open_file(const std::string& filename,
                       boost::function<void(double)> cb = noop,
                       boost::optional<vector<I3Frame::Stream> > skipstreams = boost::optional<vector<I3Frame::Stream> >(),
                       boost::optional<unsigned> nframes = boost::optional<unsigned>());

  void close();

  I3FramePtr get_frame(unsigned);
  I3FramePtr get_raw_frame(unsigned);

  void show_frame_history();

  void move_first();
  void move_last();

  void move_x(int delta);
  void move_y(int delta);
  void do_goto_frame();
  void goto_frame(unsigned frameno);

  I3Frame::Stream stream(unsigned index);
  std::vector<I3Frame::Stream> streams(unsigned start_index, unsigned length) const;
  std::vector<I3Frame::Stream> streams() const;

  std::vector<FrameInfo> frames();
  size_t size();
  void set_skipkeys(const vector<string>&);

};

#endif


