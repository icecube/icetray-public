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
#include <icetray/IcetrayFwd.h>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/scoped_ptr.hpp>

#include <fstream>
#include <set>

#include <icetray/open.h>
#include <icetray/I3Frame.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Module.h>

#include "dataio/I3FileStager.h"

class I3Reader : public I3Module
{

  unsigned nframes_;
  std::vector<std::pair<std::string, I3FileStagerPtr> > filenames_;
  std::vector<std::string> skip_;
  bool popmeta_done_;
  bool skip_unregistered_;
  bool drop_blobs_;
  std::map<std::string, I3FileStagerPtr> file_stager_map_;

  boost::iostreams::filtering_istream ifs_;

  I3FramePtr tmp_;

  std::vector<std::pair<std::string, I3FileStagerPtr> >::iterator filenames_iter_;
  std::string current_filename_;
  I3FileStagerPtr current_file_stager_;

  void OpenNextFile();

 public:

  I3Reader(const I3Context&);

  void Configure();
  void Process();

  ~I3Reader();

};

namespace io = boost::iostreams;
namespace dataio = I3::dataio;

I3_MODULE(I3Reader);

I3Reader::I3Reader(const I3Context& context) : I3Module(context),
					       nframes_(0),
					       drop_blobs_(false)
{
  std::string fname;

  AddParameter("Filename", 
	       "Filename to read.  Use either this or Filenamelist, not both.", 
	       fname);

  AddParameter("FilenameList", 
	       "List of files to read, *IN SORTED ORDER*", 
	       std::vector<std::string>());

  AddParameter("SkipKeys", 
	       "Don't load frame objects with these keys", 
	       skip_);

  AddParameter("DropBuffers",
	       "Tell I3Frames not to cache buffers of serialized frameobject data (this saves memory "
	       "at the expense of processing speed and the ability to passthru unknown frame objects)",
	       drop_blobs_);

  AddParameter("FileStagerList",
               "An optional list of I3FileStager objects used to retrieve files from remote locations. "
               "They will handle URLs passed in Filename/FilenameList.",
               std::vector<I3FileStagerPtr>());

  AddOutBox("OutBox");
}

void
I3Reader::Configure()
{
  std::string fname;
  std::vector<std::string> filenames;

  GetParameter("FileNameList", filenames);
  GetParameter("FileName", fname);
  if(!filenames.empty() && !fname.empty())
    log_fatal("Both Filename and FileNameList were specified.");

  if (filenames.empty() && fname.empty())
    log_fatal("Neither 'Filename' nor 'FilenameList' specified");

  if (filenames.empty())
    filenames.push_back(fname);

  GetParameter("SkipKeys", skip_);

  GetParameter("DropBuffers",
	       drop_blobs_);

  std::vector<I3FileStagerPtr> file_stager_list;
  GetParameter("FileStagerList",
               file_stager_list);
  file_stager_map_.clear();
  for (std::size_t i=0;i<file_stager_list.size();++i) {
    if (!file_stager_list[i]) log_fatal("FileStager cannot be NULL/None");
    const std::vector<std::string> schemes = 
      file_stager_list[i]->Schemes();
    for (std::size_t j=0;j<schemes.size();++j) {
      if (file_stager_map_.find(schemes[j])!=file_stager_map_.end()) 
        log_fatal("Multiple file stagers can handle URL scheme \"%s\". Cannot continue.", schemes[j].c_str());
      file_stager_map_[schemes[j]] = file_stager_list[i];
    }
  }

  // find a stager for each input filename
  filenames_.clear();
  for (std::size_t i=0;i<filenames.size();++i) {
    const std::string &filename=filenames[i];
    I3FileStagerPtr stager;

    for (std::map<std::string, I3FileStagerPtr>::const_iterator it=file_stager_map_.begin();
      it!=file_stager_map_.end();++it)
    {
      const std::string prefix = it->first + ":";
      if (filename.size() < prefix.size()) continue; 
      if (filename.find(prefix) == 0) {
        stager=it->second;
        log_debug("Found a file stager (\"%s\") responsible for URL %s",
          it->first.c_str(), filename.c_str());
        break;
      }
    }

    if (!stager) {
      log_debug("No stager available (local direct i/o) for URL %s",
        filename.c_str());
    }

    if (stager) {
      stager->WillReadLater(filename);
    }

    // if there is no stager available, we get a NULL pointer here
    filenames_.push_back(std::make_pair(filename, stager));
  }
  filenames_iter_ = filenames_.begin();

  OpenNextFile();
}

void
I3Reader::Process()
{
  while (ifs_.peek() == EOF)
    {
      if (filenames_iter_ == filenames_.end())
	{
	  RequestSuspension();
	  return;
	}
      else
	OpenNextFile();
    }

  I3FramePtr frame(new I3Frame);
  frame->drop_blobs(drop_blobs_);
  try {
	nframes_++;
	frame->load(ifs_, skip_);
  } catch (const std::exception &e) {
	log_fatal("Error reading %s at frame %d: %s!",
	    current_filename_.c_str(), nframes_, e.what());
	return;
  }
	
  PushFrame(frame, "OutBox");
}

void
I3Reader::OpenNextFile()
{
  if (!ifs_.empty())
    ifs_.pop();
  ifs_.reset();
  assert(ifs_.empty());

  if (current_file_stager_) {
    current_file_stager_->Cleanup(current_filename_);
  }

  current_filename_ = filenames_iter_->first;
  current_file_stager_ = filenames_iter_->second;
  nframes_ = 0;
  filenames_iter_++;

  if (current_file_stager_) {
    // stage from URL to temporary file and replace the current filename
    current_filename_ = current_file_stager_->StageFile(current_filename_);
  }

  I3::dataio::open(ifs_, current_filename_);
  log_trace("Constructing with filename %s, %zu regexes", 
	    current_filename_.c_str(), skip_.size());

  log_info("Opened file %s", current_filename_.c_str());
}

I3Reader::~I3Reader() 
{ 
}

