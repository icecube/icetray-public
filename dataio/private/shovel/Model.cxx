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
#include "shovel/Model.h"

#include <sys/types.h>

#include <boost/optional.hpp>
#include <boost/python.hpp>

#include "icetray/serialization.h"
#include "icetray/Utility.h"
#include "icetray/open.h"
#include <dataclasses/physics/I3EventHeader.h>
#include "shovel/View.h"

//==============================================================================
#pragma mark Model::ProgressManager
//==============================================================================

Model::ProgressManager::ProgressManager(View& view):
view_(view),
stop_(false),
thread_([this](){
  using std::chrono::system_clock;
  using duration=system_clock::time_point::duration;
  using mduration=std::chrono::duration<long long,std::milli>;
  duration sleepLen=std::chrono::duration_cast<duration>(mduration(1000));//one second
  system_clock::time_point nextTick=system_clock::now();
  nextTick+=sleepLen;
  
  WorkItem w;
  while(true){
    bool doNext=false;
    
    { //hold lock
      std::unique_lock<std::mutex> lock(this->mut_);
      //Wait for something to happen
      this->cond_.wait_until(lock,nextTick,
                             [this]{ return(this->stop_ || !this->work_.empty()); });
      //Figure out why we woke up
      if(this->stop_)
        return;
      //See if there's any work
      if(this->work_.empty()){
        //Nope. Back to sleep.
        nextTick+=sleepLen;
        continue;
      }
      //There is work. Should it be done now?
      nextTick=this->work_.top().time_;
      if(system_clock::now()>=nextTick){ //time to do it
        w=std::move(this->work_.top());
        this->work_.pop();
        //Update the time to next sleep until
        if(!this->work_.empty())
          nextTick=this->work_.top().time_;
        else
          nextTick+=sleepLen;
        doNext=true;
      }
    } //release lock
    if(doNext){
      w.work_();
      doNext=false;
    }
  }
}),
showingProgress_(false)
{}

Model::ProgressManager::~ProgressManager(){
  {
    std::unique_lock<std::mutex> lock(mut_);
    stop_=true;
  }
  cond_.notify_all();
  thread_.join();
}

Model::ProgressManager::WorkItem::WorkItem(std::chrono::system_clock::time_point t,
                                  std::function<void()> w):
time_(t),work_(w){}

bool Model::ProgressManager::WorkItem::operator<(const Model::ProgressManager::WorkItem& other) const{
  return(time_<other.time_);
}

void Model::ProgressManager::MaybeStartShowingProgress(){
  std::unique_lock<std::mutex> lock(this->mut_);
  if(!showingProgress_){
    //note when we got the request
    progressStart_=std::chrono::system_clock::now();
    showingProgress_=true;
    progress_=0;
    //schedule actually showing the progress bar in 50 milliseconds
    using duration=std::chrono::system_clock::time_point::duration;
    using sduration=std::chrono::duration<long long,std::milli>;
    work_.emplace(progressStart_+std::chrono::duration_cast<duration>(sduration(50)),
                  [this]()->void{
                    std::unique_lock<std::mutex> lock(this->mut_);
                    if(this->showingProgress_){ //check for cancellation in the meantime!
                      this->view_.start_scan_progress(". . . ");
                      this->actuallyShowingProgress_=true;
                      if(progress_>0)
                        view_.scan_progress(100*progress_);
                    }
                  });
    cond_.notify_all();
  }
}

void Model::ProgressManager::SetProgress(float value){
  if(actuallyShowingProgress_){
    progress_=value;
    std::unique_lock<std::mutex> lock(this->mut_);
    view_.scan_progress(100*progress_);
  }
}

void Model::ProgressManager::StopShowingProgress(){
  std::unique_lock<std::mutex> lock(this->mut_);
  if(showingProgress_){
    showingProgress_=false;
    actuallyShowingProgress_=false;
    view_.end_scan_progress();
    //while we're here with the lock held, remove pending start operations
    while(!work_.empty())
      work_.pop();
  }
}

//==============================================================================
#pragma mark Model
//==============================================================================

Model::Model(View& view, const std::vector<std::string> filenames,
             boost::optional<unsigned> nframes):
view_(view),files_(std::vector<std::string>{},1000),pman_(view)
{
  x_index_=0;
  y_index_=0;
  
  boost::shared_ptr<I3FileStager> myStager;
  try {
    boost::python::object rawStager = boost::python::import("icecube.dataio").attr("get_stagers")();
    myStager = boost::python::extract<I3FileStagerPtr>(rawStager);
  } catch (boost::python::error_already_set &err) {
    PyErr_Print();
    PyErr_Clear();
    myStager = I3TrivialFileStager::create();
  }
  
  for(auto filename : filenames){
    file_refs_.push_back(myStager->GetReadablePath(filename));
    files_.add_file(*file_refs_.back());
    log_info_stream("added " << *file_refs_.back());
  }
  
  prescan_frames(((bool)nframes?*nframes:100));
}

bool Model::prescan_frames(unsigned index)
{
  log_info_stream("Attempting to scan to index " << index);
  ssize_t max_size=files_.get_size();
  if(max_size>=0 && index>=max_size){
    log_info_stream(" Max size is known to be " << max_size << "; clipping");
    index=max_size;
    if(index<=frame_infos_.size())
      return(false); //we've already scanned that far, so we can just stop
  }
  else
    log_info_stream(" Max size is unknown");
  pman_.MaybeStartShowingProgress();
  
  //jump to the last known frame
  if(!frame_infos_.empty()){
    log_info_stream(" Seeking to index " << frame_infos_.size()-1);
    files_.seek(frame_infos_.size()-1);
    files_.pop_frame();
    if(!files_.more()){
      log_info_stream(" No more frames!");
      pman_.StopShowingProgress();
      return(false);
    }
    log_info_stream(" Popped one frame");
  }
  else{
    log_info_stream(" Seeking to start");
    files_.rewind();
  }
  unsigned start=frame_infos_.size();
  while(frame_infos_.size()<index){
    auto frame=files_.pop_frame();
    frame_infos_.emplace_back(frame->GetStop());
    log_info_stream(" Found a " << frame_infos_.back().stream << " at index " << frame_infos_.size()-1);
    if(frame->GetStop()==I3Frame::Physics){
      boost::shared_ptr<const I3EventHeader> header =
      frame->Get<boost::shared_ptr<const I3EventHeader>>(I3DefaultName<I3EventHeader>::value());
      if((header) && (frame->GetStop(I3DefaultName<I3EventHeader>::value())==frame->GetStop()))
        frame_infos_.back().sub_event_stream=header->GetSubEventStream();
    }
    //If reading causes us to learn the full size we may need to update our expectations
    max_size=files_.get_size();
    if(max_size>0 && index>max_size)
      index=max_size;
    pman_.SetProgress(double(frame_infos_.size()-start)/(index-start));
    if(!files_.more()){
      log_info_stream(" No more frames!");
      break;
    }
  }
  pman_.StopShowingProgress();
  return(frame_infos_.size()>index);
}

unsigned
Model::totalframes()
{
  ssize_t size=files_.get_size();
  if(size<0){
    size=files_.get_cur_size();
    log_info_stream("I know there are at least " << size << " frames");
  }
  else
    log_info_stream("I know there are " << size << " frames");
  return(size);
}

bool
Model::totalframes_exact()
{
  return(files_.get_size()>=0);
}

void
Model::move_first()
{
  x_index_ = 0;
  notify();
}

void
Model::move_last()
{
  if(totalframes_exact())
    x_index_=totalframes()-1;
  else{
    while(!totalframes_exact() || x_index_<totalframes()-1){
      prescan_frames(2*frame_infos_.size());
      x_index_ = frame_infos_.size()-1;
    }
  }

  notify();
}

void
Model::move_x(int delta)
{
  int newx = x_index_ + delta;
  x_index_ = newx < 0 ? 0 : newx;
  if(x_index_ >= frame_infos_.size())
    if(!prescan_frames(x_index_))
      x_index_ = frame_infos_.size() -1;
  notify();
}

void
Model::do_goto_frame()
{
  boost::optional<unsigned> result = view_.dialog<unsigned>("  Goto frame number: ");
  if (result)
    goto_frame(*result);
}

void
Model::save_xml()
{
  boost::optional<std::string> result = view_.get_file("Save xml to file: ");
  if (!result)
    return;

  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs,*result,6,std::ios::binary | std::ios::app);

  I3FramePtr frame=files_[x_index_];

  std::string xml = frame->as_xml(y_keystring_);

  ofs << prettify_xml(xml);
}

void
Model::write_frame()
{
  boost::optional<std::string> result = view_.get_file("Write frame to file: ");
  if (!result)
    return;

  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs,*result,6,std::ios::binary | std::ios::app);

  I3FramePtr fp=files_[x_index_];
  fp->save(ofs, std::vector<std::string>());
}

void
Model::write_frame_with_dependencies()
{
  boost::optional<std::string> result = view_.get_file("Write frame and dependencies to file: ");
  if (!result)
    return;

  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs,*result,6,std::ios::binary | std::ios::app);

  std::vector<boost::shared_ptr<I3Frame> > frames = files_.get_current_frame_and_deps();
  for(size_t i=0; i<frames.size(); i++)
    frames[i]->save(ofs);
}

void
Model::goto_frame(unsigned frame)
{
  x_index_ = 0;
  move_x(frame - 1);
  notify();
}

void
Model::move_y(int delta)
{
  int newy = y_index_ + delta;
  y_index_ = newy < 0 ? 0 : newy;
  y_index_ = y_index_ >= y_max_ ? y_max_ - 1 : y_index_;
  notify();
}

I3FramePtr
Model::get_frame(unsigned index)
{
  if ((!cached_frame_) || (cached_frame_index_ != index)){
    if(index>=frame_infos_.size())
      prescan_frames(index);
    if(frame_infos_.size()<=index)
      return I3FramePtr();
    cached_frame_index_=index;
    cached_frame_=files_[index];
  }
  
  return(cached_frame_);
}

void
Model::show_xml()
{
  I3FramePtr frame = get_frame(x_index_);
  if (frame->size() == 0)
    return;

  std::string xml;
  try {
    xml = frame->as_xml(y_keystring_);
  } catch (const std::exception& e) {
    xml = std::string("***\n*** ") + e.what() + " caught while deserializing\n***\n"
      + "*** Object is unreadable in an unanticipated way, \n"
      + "*** please retain the .i3 file and report this error\n"
      + "***\n"
      ;
    view_.page(xml);
    return;
  }
  try {
    std::string pretty = y_keystring_ + " [type: " + frame->type_name(y_keystring_)
      + "]\n\n" + prettify_xml(xml);
    view_.page(pretty);
  } catch (const std::bad_alloc& e) {
    log_trace("Pretty xml output not possible, going to plain xml");
    view_.page(xml);
  }
}

void
Model::pretty_print()
{
  I3FramePtr frame = get_frame(x_index_);
  if (frame->size() == 0)
    return;

  I3TrayInfoConstPtr ticp;
  std::ostringstream oss;

  try{
      ticp = frame->Get<I3TrayInfoConstPtr>(y_keystring_);
      if (!ticp)
        oss << "\n\n\nCan only pretty-print I3TrayInfo currently.\n\n\n";
      else{
        oss << *ticp;
      }
   }
  catch( const icecube::archive::archive_exception& e ){
      oss << "\n\n\n Trouble printing this I3TrayInfo: " << e.what() << "\n\n\n";
  }

  view_.page(oss.str());
}

void 
Model::notify()
{
  I3FramePtr frame = get_frame(x_index_);
  y_max_ = frame->size();

  if (y_index_ >= y_max_)
    y_index_ = y_max_-1;
  if (frame->size() == 0)
    y_index_ = 0;

  // you have to get the sorted keys out here
  std::vector<std::string> keys = frame->keys();
  log_trace("Got frame with %zu keys", keys.size());
  for (unsigned i=0; i < keys.size(); i++)
    {
      log_debug("key: %s", keys[i].c_str());
    }

  y_keystring_ = frame->size() > 0 ? keys[y_index_] : std::string();

  view_.display_frame(frame, x_index_, y_index_);
}

std::vector<I3Frame::Stream>
Model::streams(unsigned start_index, unsigned length)
{
  log_info_stream("Get streams for " << start_index << ", len " << length << " size " << frame_infos_.size());
  if(start_index + length > frame_infos_.size()){
    prescan_frames(start_index + length);
    log_info_stream("Now size=" << frame_infos_.size());
  }
  assert(start_index + length <= frame_infos_.size());
  std::vector<I3Frame::Stream> ret;

  for (unsigned i=0; i<length; i++)
    ret.push_back(frame_infos_[start_index + i].stream);
  
  return ret;
}

std::vector<std::string>
Model::sub_event_streams(unsigned start_index, unsigned length)
{
    //  log_trace("Get streams for %u, len %u", start_index, length);
    if(start_index + length > frame_infos_.size())
      prescan_frames(start_index + length);
    assert(start_index + length <= frame_infos_.size());
    std::vector<std::string> ret;
    
    for (unsigned i=0; i<length; i++)
      ret.push_back(frame_infos_[start_index + i].sub_event_stream);
    
    return ret;
}
