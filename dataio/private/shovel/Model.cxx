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
#include <regex>

#include "shovel/Model.h"

#include <sys/types.h>

#include <boost/optional.hpp>
#include <boost/python.hpp>

#include "icetray/serialization.h"
#include "icetray/Utility.h"
#include "icetray/open.h"
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include "shovel/View.h"


Model::ProgressManager::ProgressManager(View& view):
view_(view),
stop_(false),
showingProgress_(false),
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
})
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

void Model::ProgressManager::MaybeStartShowingProgress(std::string message){
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
                  [this,message]()->void{
                    std::unique_lock<std::mutex> lock(this->mut_);
                    if(this->showingProgress_){ //check for cancellation in the meantime!
                      this->view_.start_scan_progress(message);
                      this->actuallyShowingProgress_=true;
                      if(progress_>0)
                        view_.scan_progress(100*progress_);
                    }
                  });
    cond_.notify_all();
  }
  else{
    nestingLevel++;
  }
}

void Model::ProgressManager::SetProgress(float value){
  if(nestingLevel)
    return;
  //ignore redundant values which would be displayed the same
  if((int)(100*value)==(int)(100*progress_))
    return;
  if(actuallyShowingProgress_){
    progress_=value;
    std::unique_lock<std::mutex> lock(this->mut_);
    view_.scan_progress(100*progress_);
  }
}

void Model::ProgressManager::StopShowingProgress(){
  std::unique_lock<std::mutex> lock(this->mut_);
  if(nestingLevel){
    nestingLevel--;
    return;
  }
  if(showingProgress_){
    showingProgress_=false;
    actuallyShowingProgress_=false;
    view_.end_scan_progress();
    //while we're here with the lock held, remove pending start operations
    while(!work_.empty())
      work_.pop();
  }
}

Model::FrameInfo::FrameInfo(const I3Frame& f):stream(f.GetStop()){
  if(stream==I3Frame::Physics){
    boost::shared_ptr<const I3EventHeader> header =
    f.Get<boost::shared_ptr<const I3EventHeader>>(I3DefaultName<I3EventHeader>::value());
    if((header) && (f.GetStop(I3DefaultName<I3EventHeader>::value())==stream))
      sub_event_stream=header->GetSubEventStream();
  }
}

bool Model::FrameInfo::operator==(const FrameInfo& other) const{
  return(stream==other.stream && sub_event_stream==other.sub_event_stream);
}

Model::FrameInfoHash::result_type
Model::FrameInfoHash::operator()(const Model::FrameInfo& fi) const{
  std::hash<char> ch;
  auto h=ch(fi.stream.id());
  if(fi.stream==I3Frame::Physics)
    for(char c : fi.sub_event_stream)
      h^=ch(c);
  return h;
}

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
  
  prescan_frames(((bool)nframes?*nframes:COLS));
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
  pman_.MaybeStartShowingProgress("Scanning. . . ");
  
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
    frame_infos_.emplace_back(*frame);
    log_info_stream(" Found a " << frame_infos_.back().stream << " at index " << frame_infos_.size()-1);
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
    if(!prescan_frames(x_index_+1))
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
  boost::optional<std::string> result = view_.dialog<std::string>("Save xml to file: ");
  if (!result || result->empty())
    return;
  
  pman_.MaybeStartShowingProgress("Generating XML. . . ");
  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs,*result,6,std::ios::binary | std::ios::app);

  I3FramePtr frame=files_[x_index_];

  std::string xml = frame->as_xml(y_keystring_);
  pman_.SetProgress(.5);
  ofs << prettify_xml(xml);
  pman_.SetProgress(1);
  pman_.StopShowingProgress();
}

void
Model::write_frame()
{
  boost::optional<std::string> result = view_.dialog<std::string>("Write frame to file: ");
  if (!result || result->empty())
    return;
  
  pman_.MaybeStartShowingProgress("Writing. . . ");
  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs,*result,6,std::ios::binary | std::ios::app);

  I3FramePtr fp=files_[x_index_];
  fp->save(ofs, std::vector<std::string>());
  pman_.SetProgress(1.);
  pman_.StopShowingProgress();
}

void
Model::write_frame_with_dependencies()
{
  boost::optional<std::string> result = view_.dialog<std::string>("Write frame and dependencies to file: ");
  if (!result || result->empty())
    return;
  
  pman_.MaybeStartShowingProgress("Writing. . . ");
  boost::iostreams::filtering_ostream ofs;
  I3::dataio::open(ofs,*result,6,std::ios::binary | std::ios::app);

  std::vector<boost::shared_ptr<I3Frame> > frames = files_.get_current_frame_and_deps();
  for(size_t i=0; i<frames.size(); i++){
    frames[i]->save(ofs);
    pman_.SetProgress(double(i+1)/frames.size());
  }
  pman_.StopShowingProgress();
}

void
Model::do_find_event()
{
  boost::optional<std::string> result = view_.dialog<std::string>("Find event: ");
  if(!result || result->empty())
    return;
  
  bool exact_run=false;
  unsigned long run=0;
  unsigned long event=0;
  
  std::regex id_matcher(R"(([0-9]+(,|/))?([0-9]+))");
  std::smatch matches;
  if(!std::regex_match(*result,matches,id_matcher))
    return;
  
  if(!matches[1].str().empty()){
    run=std::stoul(matches[1]);
    exact_run=true;
  }
  event=std::stoul(matches[3]);
  
  log_info_stream("Will search for event " << run << "," << event);
  
  std::string message="Searching for event ";
  if(exact_run)
    message+=std::to_string(run);
  else
    message+="*";
  message+=","+std::to_string(event)+". . . ";
  pman_.MaybeStartShowingProgress(message);
  auto old_x_index_ = x_index_;
  bool found=false;
  boost::shared_ptr<I3Frame> frame;
  //serach forward form the current point
  do {
    frame=get_frame(x_index_);
    if(!frame)
      break;
    boost::shared_ptr<const I3EventHeader> header =
    frame->Get<boost::shared_ptr<const I3EventHeader>>(I3DefaultName<I3EventHeader>::value());
    if(!header){
      x_index_++;
      continue;
    }
    if((!exact_run || header->GetRunID()==run) && (header->GetEventID()==event)){
      found=true;
      break;
    }
    x_index_++;
  } while (x_index_<totalframes() || !totalframes_exact());
  
  if(!found) //wrap around
    x_index_=0;
  //search up to the original point
  while(!found && x_index_<old_x_index_){
    frame=get_frame(x_index_);
    if(!frame)
      break;
    boost::shared_ptr<const I3EventHeader> header =
    frame->Get<boost::shared_ptr<const I3EventHeader>>(I3DefaultName<I3EventHeader>::value());
    if(!header){
      x_index_++;
      continue;
    }
    if((!exact_run || header->GetRunID()==run) && (header->GetEventID()==event)){
      found=true;
      break;
    }
    x_index_++;
  }
  pman_.StopShowingProgress();
  if(!found){ //go back to where we were
    goto_frame(old_x_index_+1);
    return;
  }
}

void
Model::goto_frame(unsigned frame)
{
  move_x(frame - 1 - x_index_);
  notify();
}

void
Model::move_y(int delta)
{
  int newy = y_index_ + delta;
  y_index_ = newy < 0 ? 0 : newy;
  y_index_ = y_index_ >= y_max_ ? y_max_ - 1 : y_index_;
  y_keystring_=cached_frame_keys_[y_index_];
  recent_frame_keys_[frame_infos_[x_index_]]=y_keystring_;
  notify();
}

I3FramePtr
Model::get_frame(unsigned index)
{
  if ((!cached_frame_) || (cached_frame_index_ != index)){
    if(index>=frame_infos_.size())
      prescan_frames(index+1);
    if(frame_infos_.size()<=index)
      return I3FramePtr();
    cached_frame_index_=index;
    pman_.MaybeStartShowingProgress("Scanning. . . ");
    cached_frame_=files_[index];
    pman_.SetProgress(1.);
    pman_.StopShowingProgress();
    
    cached_frame_keys_ = cached_frame_->keys();
    y_max_ = cached_frame_->size();
    //figure out whether there is already known key we should scroll to,
    auto key_it=recent_frame_keys_.find(frame_infos_[index]);
    if(key_it!=recent_frame_keys_.end()){ //yes, so see if we can scroll to it
      auto frame_key_it=std::lower_bound(cached_frame_keys_.begin(),cached_frame_keys_.end(),key_it->second);
      if(frame_key_it!=cached_frame_keys_.end()) //it's there, scroll to it
        y_index_=std::distance(cached_frame_keys_.begin(),frame_key_it);
      else //nope
        y_index_=0;
      if(y_index_<y_max_) //update the selected key
        y_keystring_=cached_frame_keys_[y_index_];
    }
    else{ //no; add an entry
      y_index_=0; //default to selecting the first key
      if(!cached_frame_keys_.empty()){
        recent_frame_keys_[frame_infos_[index]]=cached_frame_keys_.front();
        y_keystring_=cached_frame_keys_.front();
      }
    }
  }
  
  return(cached_frame_);
}

I3FramePtr
Model::current_frame(){
  return(get_frame(x_index_));
}

void
Model::show_xml()
{
  I3FramePtr frame = get_frame(x_index_);
  if (frame->size() == 0)
    return;

  std::string xml;
  pman_.MaybeStartShowingProgress("Generating XML. . . ");
  try {
    xml = frame->as_xml(y_keystring_);
    pman_.SetProgress(0.5);
  } catch (const std::exception& e) {
    xml = std::string("***\n*** ") + e.what() + " caught while deserializing\n***\n"
      + "*** Object is unreadable in an unanticipated way, \n"
      + "*** please retain the .i3 file and report this error\n"
      + "***\n"
      ;
    pman_.StopShowingProgress();
    view_.page(xml);
    return;
  }
  try {
    std::string pretty = y_keystring_ + " [type: " + frame->type_name(y_keystring_)
      + "]\n\n" + prettify_xml(xml);
    pman_.SetProgress(1.);
    pman_.StopShowingProgress();
    view_.page(pretty);
  } catch (const std::bad_alloc& e) {
    log_trace("Pretty xml output not possible, going to plain xml");
    pman_.StopShowingProgress();
    view_.page(xml);
  }
}

void
Model::pretty_print()
{
  I3FramePtr frame = get_frame(x_index_);
  if (frame->size() == 0)
    return;

  std::ostringstream oss;
  oss << y_keystring_ << " [" << frame->type_name(y_keystring_) << "]:\n";
  try{ //All frame objects are equal, but recopulses are more equal
    auto pulses=frame->Get<boost::shared_ptr<const I3RecoPulseSeriesMap>>(y_keystring_);
    if(pulses){
      oss << "As an I3RecoPulseSeriesMap:\n";
      pulses->Print(oss);
      oss << "\nOriginal form:\n";
    }
  }catch(std::exception& ex){
    //do nothing
  }
  try{
    frame->Get<I3FrameObject>(y_keystring_).Print(oss);
  }catch(std::exception& ex){
    oss << "Exception: " << ex.what();
    if(std::string(ex.what()).find("exists, but won't dynamic cast")!=std::string::npos)
      oss << "\n (This may indicate that the project needed to read this object"
      " has not been loaded)";
  }

  view_.page(oss.str());
}

void 
Model::notify()
{
  view_.display_frame(get_frame(x_index_), x_index_, y_index_);
}

std::vector<I3Frame::Stream>
Model::streams(unsigned start_index, unsigned length)
{
  log_info_stream("Get streams for " << start_index << ", len " << length << " size " << frame_infos_.size());
  if(start_index + length > frame_infos_.size()){
    prescan_frames(start_index + length);
    log_info_stream("Now size=" << frame_infos_.size());
    if(start_index >= frame_infos_.size())
      return(std::vector<I3Frame::Stream>());
    if(start_index + length > frame_infos_.size())
      length = frame_infos_.size() - start_index;
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
  if(start_index + length > frame_infos_.size()){
    prescan_frames(start_index + length);
    log_info_stream("Now size=" << frame_infos_.size());
    if(start_index >= frame_infos_.size())
      return(std::vector<std::string>());
    if(start_index + length > frame_infos_.size())
      length = frame_infos_.size() - start_index;
  }
    assert(start_index + length <= frame_infos_.size());
    std::vector<std::string> ret;
    
    for (unsigned i=0; i<length; i++)
      ret.push_back(frame_infos_[start_index + i].sub_event_stream);
    
    return ret;
}
