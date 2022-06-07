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
#ifndef DATAIO_SHOVEL_VIEW_H_INCLUDED
#define DATAIO_SHOVEL_VIEW_H_INCLUDED

#include <unordered_map>
#include <ncurses.h>
#include "color.h"
#include <boost/optional.hpp>
#include <icetray/I3Frame.h>

#include <cdk.h>

class Model;

class View
{
private:

  unsigned totalframes_;
  unsigned y_top_offset_;
  CDKSCREEN* cdkscreen;
  CDKHISTOGRAM* progresshist_;
  Model* model_;

  void drawtape(unsigned line, unsigned col,
                I3Frame::Stream stream, 
                std::string sub_event_stream,
                unsigned frameno, int attr = A_NORMAL);

  void draw_border();

  View();

  std::map<I3Frame::Stream, color_pair> colors_;
  std::map<std::string, color_pair> subeventstream_colors_;
  std::vector<color_pair> new_subeventstream_colors_;
  std::unordered_map<std::string,std::string> clean_typenames_;
  unsigned maxtypelen_;
  bool scanning_;
    
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
  void start_scan_progress(const std::string& message);
  void scan_progress(double d);
  void end_scan_progress();

  template<typename T>
  boost::optional<T> dialog(const std::string& prompt);

  void usage();

  static View& Instance();

};



#endif


