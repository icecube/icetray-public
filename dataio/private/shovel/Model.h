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
  ///A reference to the view object assigned to draw this Model
  View& view_;
  ///The sequence of I3 files the Model represents
  dataio::I3FrameSequence files_;
  ///The collection of handles optained from staging the file
  std::vector<I3::dataio::shared_filehandle> file_refs_;
  
  ///A wrapper for information the Mdoel likes to cache about the frames it sees
  struct FrameInfo{
    I3Frame::Stream stream;
    std::string sub_event_stream;
    FrameInfo() : stream(I3Frame::None){}
    FrameInfo(const I3Frame&);
    bool operator==(const FrameInfo&) const;
  };
  ///An implementation of hashing for FrameInfos
  struct FrameInfoHash{
    using result_type=std::size_t;
    using argument_type=FrameInfo;
    result_type operator()(const FrameInfo&) const;
  };
  
  ///Information on all frames that have been seen so far. Defined to begin at
  ///the start of files_ but at any given time may not cover its full extent.
  std::vector<FrameInfo> frame_infos_;
  
  ///An object which handles deciding when to display the progress bar for slow
  ///operations. Contains its own worker thread in order to do this only when
  ///enough time has elasped for the user to probably prefer seeing a progress
  ///bar than an otherwise apparently frozen interface.
  struct ProgressManager{
  private:
    View& view_;
    bool stop_;
    std::atomic<bool> showingProgress_;
    std::atomic<bool> actuallyShowingProgress_;
    unsigned int nestingLevel;
    float progress_;
    std::mutex mut_;
    std::condition_variable cond_;
    std::thread thread_;
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
    
    void MaybeStartShowingProgress(std::string message);
    ///\param value a fraction in [0,1]
    void SetProgress(float value);
    void StopShowingProgress();
  };
  ///The progress bar manager
  ProgressManager pman_;

  ///The index of the currently selcted framewithin the frame sequence
  unsigned x_index_;
  ///The index of the currently selected key within the currently selected frame
  ///(considering its keys in alphabetical order)
  unsigned y_index_;
  ///The number of keys in the current frame
  unsigned y_max_;

  ///The index of the last frame fetched by get_frame (the current frame index)
  unsigned cached_frame_index_;
  ///The last frame fetched by get_frame (the current frame)
  I3FramePtr cached_frame_;
  ///The keys in the current frame, in sorted order
  std::vector<std::string> cached_frame_keys_;

  ///The currently selected key
  std::string y_keystring_;
  ///The most recently selected key for each distinct FrameInfo which has been seen
  std::unordered_map<FrameInfo,std::string,FrameInfoHash> recent_frame_keys_;

  
  ///Fetch the frame at the given index
  I3FramePtr get_frame(unsigned);
  
  ///Ensure that all froms up to index (exclusive) have been examined to
  ///determine their basic properties. If index is larger than the extent of
  ///files_, all frames up to the end of the sequence will be examined.
  ///\return whether the scan could read up to the requested index (the frame
  ///        sequence is long enough)
  bool prescan_frames(unsigned index);
    
public:
  
  Model(View& view, const std::vector<std::string> filenames,
        boost::optional<unsigned> nframes = boost::optional<unsigned>());

  ///Display the XML representation of the currently selected frame object
  void show_xml();
  ///Display a nice representation of the currently selected frame object.
  ///Not implemented for most frame object types.
  void pretty_print();

  ///Make the first frame in the sequence the current selection.
  void move_first();
  ///Make the last frame in the sequence the current selection.
  void move_last();

  ///Select a different frame relative to the current selection position
  ///\param delta the change to apply to the selected frame index. If this would
  ///       carry the selection outside the valid range of indices it will be clipped.
  void move_x(int delta);
  ///Select a different frame object relative to the current selection position
  ///\param delta the change to apply to the selected object index. If this would
  ///       carry the selection outside the valid range of indices it will be clipped.
  void move_y(int delta);
  ///Prompt the user for a frame index, and the make that the selected frame
  void do_goto_frame();
  ///Make the frame at the given index the current selection
  void goto_frame(unsigned frameno);
  ///Write the currently selected frame to a file, prompting the user for a file name.
  void write_frame();
  ///Write the currently selected frame, preceded by all frames from which keys
  ///have been mixed into it, to a file, prompting the user for a file name.
  void write_frame_with_dependencies();
  ///Write the XML representation of the currently selected frame object to a file.
  void save_xml();
  ///Prompt the user for an event ID, and then search forward through the frame
  ///sequence for a Q or P frame which matches.
  void do_find_event();

  ///Notify the associated View that it should redraw, assumedly because
  ///something has changed.
  void notify();

  ///\return the total number of frames in the frame sequence, or the number of
  ///        frames which have been read so far if the end of the sequence has
  ///        not yet been reached.
  unsigned totalframes();
  ///\return true if the end of the frame sequence has been reached and so the
  ///        result of totalframes is accurate.
  bool totalframes_exact();
  
  ///Retrieve the currently selected frame
  I3FramePtr current_frame();

  ///Get the streams of the frames within a range of indices. If the range
  ///extends beyond the frames which have currently been read, the Model will
  ///attempt to read as far as necessary. If the range extends beyond the end
  ///of the frames which exist it will be clipped.
  ///\param start_index the first index in the query range
  ///\param length the length of the query range
  ///\return the streams of the frames within the specified index range
  std::vector<I3Frame::Stream> streams(unsigned start_index, unsigned length);
  ///Get the subevent streams of the frames within a range of indices. If the
  ///range extends beyond the frames which have currently been read, the Model
  ///will attempt to read as far as necessary. If the range extends beyond the
  ///end of the frames which exist it will be clipped.
  ///\param start_index the first index in the query range
  ///\param length the length of the query range
  ///\return the subevent streams of the frames within the specified index range
  std::vector<std::string> sub_event_streams(unsigned start_index, unsigned length);

  SET_LOGGER("dataio-shovel::Model");
};



#endif


