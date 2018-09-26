#include "shovelio/FileGroup.h"

#include <boost/foreach.hpp>

namespace shovelio { namespace raw {

FileGroup::FileGroup():
  current_file_(0)
{}

FileGroup::FileGroup(const std::vector<std::string>& filenames):
  current_file_(0)
{
  BOOST_FOREACH(const std::string& x, filenames)
    AddFile(x);
}

bool
FileGroup::Next(I3FramePtr p, bool fast,
                const FilterList& filter)
{
  if (current_file_ == files_.size()){
    log_debug("End of file group");
    return false;
  }

  File& cur = files_[current_file_];
  if (cur.GetType() == File::SinglePass && fast == true){
    log_debug("Fast option does not work with SinglePass");
    return false;
  }

  if (!cur.Next(p, fast, filter)) {
    ++current_file_;
    return FileGroup::Next(p, fast, filter);
  }

  return true;
}

bool
FileGroup::AtStreamPos(I3FramePtr p, uint64_t pos,
                       const FilterList& filter)
{
  log_trace_stream("Jump from " << CurrentStreamPos() << " to " << pos);
  Files::iterator iter = files_.begin();
  current_file_ = 0;
  for( ; iter != files_.end() &&
         iter->HasSize() && pos >= iter->GetSize(); ++iter ){
    pos -= iter->GetSize();
    ++current_file_;
  }

  if( iter == files_.end() )
    return false;

  return iter->AtStreamPos(p, pos, filter);
}

uint64_t
FileGroup::CurrentStreamPos()
{
  uint64_t idx = 0;
  for( unsigned i = 0; i < current_file_; ++i )
    idx += files_[i].GetSize();
  if( current_file_ < files_.size() )
    idx += files_[current_file_].CurrentStreamPos();
  return idx;
}

bool
FileGroup::HasSize() const
{
  for( Files::const_iterator iter = files_.begin();
       iter != files_.end(); ++iter )
    if( !iter->HasSize() )
      return false;
  return true;
}

uint64_t
FileGroup::GetSize() const
{
  uint64_t size = 0;
  for( Files::const_iterator iter = files_.begin();
       iter != files_.end(); ++iter ){
    if( !iter->HasSize() )
      return uint64_t(-1); // largest possible value
    size += iter->GetSize();
  }
  return size;
}

void
FileGroup::CloseAllFiles()
{
  files_.clear();
  current_file_ = 0;
}

void
FileGroup::CloseLastFile()
{
  files_.pop_back();
  if( current_file_ > files_.size() )
    current_file_ = files_.size();
}

}}
