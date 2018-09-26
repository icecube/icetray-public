#include "shovelio/File.h"

#include <icetray/open.h>
#include <icetray/I3Logging.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/foreach.hpp>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>

namespace shovelio { namespace raw {

FilterConstants::FilterConstants():
  SkipAll(1, "^.*$"),
  OnlyKeepEventHeader(1)
{
  // anti-match pattern: see stackoverflow question 406230
  OnlyKeepEventHeader[0] = std::string("^((?!")
                         + I3DefaultName<I3EventHeader>::value()
                         + std::string(").)*$");
}

File::File(const std::string& filepath, Type type):
  filepath_(filepath),
  type_(Unknown),
  pos_(0),
  has_size_(false),
  size_(-1) // largest possible value
{
  log_debug_stream("Opening a new file: " << filepath);

  if( filepath == "-" ){
    // read from pipe
    log_debug("File is SinglePass (stdin)");
    type_ = SinglePass;

    boost::iostreams::file_descriptor_source
#if BOOST_VERSION < 104400
      is( STDIN_FILENO, false );
#else
      is( STDIN_FILENO, boost::iostreams::never_close_handle );
#endif
    ifs_.push( is );
    return;
  }

  I3::dataio::open(ifs_, filepath.c_str());
  if( !ifs_ ){
    log_error_stream("Can't open " << filepath << " for reading: "
                     << std::strerror(errno) << " (" << errno << ")");
    type_ = Bad;
    size_ = 0;
    has_size_ = true;
    return;
  }
  if( ifs_.peek() == EOF ){
    log_error_stream("File " << filepath << " is empty");
    type_ = Empty;
    size_ = 0;
    has_size_ = true;
    return;
  }

  if( type == Unknown ){ // type auto-detection
    // Check for socket syntax
    if( filepath.find("socket://") == 0 ){
      log_debug("File is SinglePass (socket)");
      type_ = SinglePass;
    }else{
      // Attempt to seek to file's end;
      // if file is uncompressed this will succeed.
      ifs_.seekg(0, std::ios::end);
      std::ios::pos_type length = ifs_.tellg();
      if( ifs_.good() ){
        log_debug("File is Seekable");
        type_ = Seekable;
        size_ = length;
        has_size_ = true;
        ifs_.seekg(0, std::ios::beg);
      }else{
        log_debug("File is MultiPass");
        type_ = MultiPass;
        ifs_.reset();
        ifs_.clear();
        I3::dataio::open( ifs_, filepath.c_str() );
      }
    }
  }else{ // type defined by user
    type_ = type;
    if( type == Seekable ){
      ifs_.seekg(0, std::ios::end);
      size_ = ifs_.tellg();
      has_size_ = true;
      ifs_.seekg(0, std::ios::beg);
    }
  }
}

File::~File()
{
  log_debug("File %s closed", filepath_.c_str());
}

bool
File::Next(I3FramePtr p, bool fast,
           const FilterList& filter) {
  if( ifs_.peek() == EOF || !ifs_.good() )
    return false;

  log_trace_stream("Next frame requested at "
                   << CurrentStreamPos()
                   << " with fast=" << fast
                   << " for File::Type=" << GetType());

  try{
    if( fast ){
      if (type_ == SinglePass)
        log_fatal("Fast-loading does not make "
                  "sense for SinglePass");

      // do not verify ckecksums in fast mode
      p->load(ifs_, filter, false);
    } else {
      p->load(ifs_, filter);
    }
  }catch( icecube::archive::archive_exception& b ){
    log_error_stream( "archive exception while "
                      "loading frame at stream pos "
                      << CurrentStreamPos() << ", "
                      "no more frames will be loaded." );
    // set size to current position
    size_ = CurrentStreamPos();
    has_size_ = true;
    return false;
  }

  log_trace("Got frame from file");

  // set new position
  if( type_ == Seekable )
    pos_ = ifs_.tellg();
  else
    ++pos_;

  if( ifs_.peek() == EOF ){
    size_ = CurrentStreamPos();
    has_size_ = true;
    log_trace_stream("Found end of file at pos=" << size_);
  }

  return true;
}

bool
File::AtStreamPos(I3FramePtr p, uint64_t pos,
                       const FilterList& filter)
{
  log_trace_stream("Jump from " << CurrentStreamPos()
                   << " to " << pos << " requested "
                   "for File::Type=" << type_);

  if (GetSize() == 0)
    log_fatal("Cannot seek in bad or empty stream");

  if (type_ == SinglePass && CurrentStreamPos() > pos) {
    log_trace("Cannot seek backwards in SinglePass stream");
    return false;
  }

  if (HasSize() && pos >= GetSize()) {
    log_error("Cannot seek beyond end of file");
    return false;
  }

  if (!ifs_.good() || (type_ == MultiPass &&
                       pos < CurrentStreamPos())) {
    log_trace("%s: restarting stream", filepath_.c_str());
    ifs_.reset();
    ifs_.clear();
    I3::dataio::open(ifs_, filepath_.c_str());
    pos_ = 0;
  }

  switch( type_ ){
  case Seekable:
    ifs_.seekg(pos, std::ios::beg);
    p->load(ifs_, filter);
    pos_ = ifs_.tellg();
  break;
  case MultiPass:
  case SinglePass:
    // move (fast-)forward to position
    while( pos_ < pos && Next(p, type_ == MultiPass, kFilter.SkipAll) );
    if( HasSize() && pos >= GetSize() )
      return false;
    return Next(p, false, filter);
  break;
  default:
    log_fatal("This should never happen");
  };

  return true;
}

}}
