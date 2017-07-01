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
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>
#include <atomic>
#include <boost/optional.hpp>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Frame.h>
#include <dataio/I3FrameSequence.h>
#include <dataio/I3FileStager.h>

class View;

class Model
{
  View& view_;
  dataio::I3FrameSequence files_;
  std::vector<I3::dataio::shared_filehandle> file_refs_;
  
  struct FrameInfo{
    I3Frame::Stream stream;
    std::string sub_event_stream;
    FrameInfo() : stream(I3Frame::None){}
    FrameInfo(I3Frame::Stream stream) : stream(stream){}
  };
  std::vector<FrameInfo> frame_infos_;
  
  struct ProgressManager{
  private:
    View& view_;
    std::mutex mut_;
    std::condition_variable cond_;
    bool stop_;
    std::thread thread_;
    std::atomic<bool> showingProgress_;
    std::atomic<bool> actuallyShowingProgress_;
    float progress_;
    std::chrono::system_clock::time_point progressStart_;
    struct WorkItem{
      std::chrono::system_clock::time_point time_;
      std::function<void()> work_;
      WorkItem(){}
      WorkItem(std::chrono::system_clock::time_point t, std::function<void()> w);
      bool operator<(const WorkItem&) const;
    };
    std::priority_queue<WorkItem> work_;
  public:
    explicit ProgressManager(View& view);
    ~ProgressManager();
    
    void MaybeStartShowingProgress();
    ///\param value a fraction in [0,1]
    void SetProgress(float value);
    void StopShowingProgress();
  };
  ProgressManager pman_;

  std::ifstream ifs_;

  unsigned x_index_;
  unsigned y_index_;
  unsigned y_max_;
  std::string y_keystring_;

  I3FramePtr get_frame(unsigned);

  unsigned cached_frame_index_;
  I3FramePtr cached_frame_;
  
  bool prescan_frames(unsigned index);
    
public:
  
  Model(View& view, const std::vector<std::string> filenames,
        boost::optional<unsigned> nframes = boost::optional<unsigned>());

  void show_xml();
  void pretty_print();

  void move_first();
  void move_last();

  void move_x(int delta);
  void move_y(int delta);
  void do_goto_frame();
  void goto_frame(unsigned frameno);
  void write_frame();
  void write_frame_with_dependencies();
  void save_xml();

  void notify();

  unsigned totalframes();
  bool totalframes_exact();

  std::vector<I3Frame::Stream> streams(unsigned start_index, unsigned length);
  std::vector<std::string> sub_event_streams(unsigned start_index, unsigned length);

  SET_LOGGER("dataio-shovel::Model");
};



#endif


