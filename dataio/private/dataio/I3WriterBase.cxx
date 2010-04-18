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
#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/Utility.h>

#include <dataio/I3WriterBase.h>

#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>

#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <ostream>
#include <set>

#include <boost/iostreams/stream.hpp>
#include <boost/ref.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using boost::algorithm::to_lower;
using boost::algorithm::iends_with;
namespace io = boost::iostreams;
using boost::archive::portable_binary_oarchive;
using namespace boost::posix_time;

template <class Derived>
I3WriterBase<Derived>::I3WriterBase(const I3Context& ctx) 
  : I3ConditionalModule(ctx),
    configWritten_(false),
    frameCounter_(0),     
    gzip_compression_level_(-2)
{
  AddOutBox("OutBox");
  AddParameter("CompressionLevel", 
	       "0 == no compression, 1 == best speed, 9 == best compression (6 by default)",
	       gzip_compression_level_);

  AddParameter("SkipKeys", 
	       "Don't write keys that match any of the regular expressions in this vector", 
	       skip_keys_);

  AddParameter("Filename","The file we'll write to.  "
	       "If it ends with .gz and no CompressionLevel is specified, it will be "
	       "gzipped at gzip's default compression level.  In the I3MultiWriter this string"
	       "must contain a %u printf formatting character.  (Try %04u if you're writing < 10000 files)", 
	       path_);

  AddParameter("Streams", 
	       "Vector of I3Frame.Stream types that we should write.  Those that do not appear "
	       "here will be skipped.   Default: all streams enabled.",
	       streams_);
}

template <class Derived>
void
I3WriterBase<Derived>::Configure()
{
  log_trace("%s", __PRETTY_FUNCTION__);

  GetParameter("Filename", path_);
  if (path_.empty())
    log_fatal("Please specify a file name.");
  else
    log_trace("Filename = %s", path_.c_str());

  GetParameter("SkipKeys", skip_keys_);
  GetParameter("CompressionLevel", gzip_compression_level_);

  if (iends_with(path_, ".gz"))  // filename ends in .gz
    {
      if (gzip_compression_level_ == -2) // compression level unset
	gzip_compression_level_ = 6;    // set to default
    } 
  else    // filename doesn't end in .gz 
    { 
      if (gzip_compression_level_ == -2) // compression level unset
	gzip_compression_level_ = 0;
    }

  if (gzip_compression_level_ > 0)
    {
      log_info("Compressing at level %d", gzip_compression_level_);
    } 
  else 
    {
      log_info("Not compressing.");
    }

  try {
    GetParameter("Streams", streams_);
  } catch (const boost::python::error_already_set& e) {
    log_trace("Trying compatibility interface for I3WriterBase's Streams parameter");
    PyErr_Clear();
    vector<string> vs;
    GetParameter("Streams", vs);

    streams_.clear();

    for (unsigned i =0; i < vs.size(); i++)
      {
	if (vs[i] == "Geometry")
	  streams_.push_back(I3Frame::Geometry);
	else if (vs[i] == "Calibration")
	  streams_.push_back(I3Frame::Calibration);

	else if (vs[i] == "DetectorStatus")
	  streams_.push_back(I3Frame::DetectorStatus);
	else if (vs[i] == "Physics")
	  streams_.push_back(I3Frame::Physics);
	else if (vs[i] == "TrayInfo")
	  streams_.push_back(I3Frame::TrayInfo);
	else
	  log_fatal("I3Writer %s is seeing unknown stream %s",
		    GetName().c_str(), vs[i].c_str());
      }
  }
}

template <class Derived>
void
I3WriterBase<Derived>::Finish()
{
  log_trace("%s", __PRETTY_FUNCTION__);
  derived()->Finish_();
  log_info("%u frames written.", frameCounter_);
}

template <class Derived>
void 
I3WriterBase<Derived>::WriteConfig(I3FramePtr frame)
{
  if (configWritten_)
    return;

  if (streams_.size() > 0)
    if (find(streams_.begin(), streams_.end(), I3Frame::TrayInfo) == streams_.end())
      {
	configWritten_ = true;
	return;
      }

  I3TrayInfoService& srv = 
    context_.template Get<I3TrayInfoService>("__tray_info_service");

  const I3TrayInfo& config = srv.GetConfig();
  
  I3TrayInfoPtr trayinfo(new I3TrayInfo(config));

  // either make new frame or merge to existing frame
  I3FramePtr oframe;
  if (frame->GetStop() == I3Frame::TrayInfo)
    oframe = frame;
  else
    oframe = I3FramePtr(new I3Frame(I3Frame::TrayInfo));

  std::string timestr = to_iso_extended_string(microsec_clock::universal_time());

  while(oframe->Has(timestr))
    timestr += "~";

  oframe->Put(timestr, trayinfo);

  // if we didn't make the frame, don't save it, whoever gave it to us will.
  // Otherwise insert this into the file data stream.
  if (oframe != frame)
    {
      oframe->save(filterstream_, skip_keys_);
      derived()->Flush();
    }
  configWritten_ = true;
}


template <class Derived>
void
I3WriterBase<Derived>::Process()
{
  I3FramePtr frame = PopFrame();

  WriteConfig(frame);

  if (streams_.size() > 0)
    if (find(streams_.begin(), streams_.end(), frame->GetStop())
	== streams_.end())
      {
	PushFrame(frame, "OutBox");
	return;
      }

  frameCounter_++;

  frame->save(filterstream_, skip_keys_);
  derived()->Flush();

  PushFrame(frame,"OutBox");
  log_debug("... done");  
}


#include <dataio/I3Writer.h>
template class I3WriterBase<I3Writer>;

#include <dataio/I3MultiWriter.h>
template class I3WriterBase<I3MultiWriter>;
