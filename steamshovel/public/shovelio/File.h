#ifndef SHOVEL_RAW_FILE_H_
#define SHOVEL_RAW_FILE_H_

#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>
#include <dataclasses/physics/I3EventHeader.h>

#include <vector>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace shovelio { namespace raw {

  typedef std::vector<std::string> FilterList;

  struct FilterConstants {
    FilterList SkipAll;
    FilterList OnlyKeepEventHeader;
    FilterConstants();
  };

  static const FilterConstants kFilter;

  /** raw::File provides raw access to I3Frames in form
   *  of forward iteration via Next(...), or in form of
   *  random access via the AtStreamPos(...). raw::File
   *  does not merge in keys from parent frames, as it
   *  does not keep track of relations between frames.
   *  AtStreamPos(...) hides details of the underlying
   *  iterator as much as possible. It uses a fast way
   *  to advance a sequence iterator to a random position,
   *  based on a fast version of Next(...), which skips
   *  de-serializing all frame keys.
   */
  class File: private boost::noncopyable {
    SET_LOGGER("shovelio::raw");
  public:
    enum Type {
      Unknown = -3,
      Bad = -2,
      Empty = -1,
      SinglePass = 0,
      MultiPass,
      Seekable
    };

  private:
    boost::iostreams::filtering_istream ifs_;
    std::string filepath_;
    Type type_;
    uint64_t pos_;
    bool has_size_;
    uint64_t size_;

  public:
    File(const std::string& file, Type type = Unknown);
    ~File();

    /// Get next I3Frame in file.
    bool Next(I3FramePtr, bool fast, const FilterList&);
    /// Get I3Frame at stream position.
    bool AtStreamPos(I3FramePtr, uint64_t, const FilterList&);
    /// Current stream position.
    uint64_t CurrentStreamPos() const { return pos_; }
    /// Check if total size is already known.
    bool HasSize() const { return has_size_; }
    /// Total size of file. Returns huge value, if HasSize() == false.
    uint64_t GetSize() const { return size_; }
    /// Get info on file type, see enum File::Type.
    Type GetType() const { return type_; }
    /// Get path to opened file.
    const std::string& GetPath() const { return filepath_; }
  };

}}

#endif
