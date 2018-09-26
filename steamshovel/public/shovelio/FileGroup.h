#ifndef SHOVEL_RAW_FILEGROUP_H_
#define SHOVEL_RAW_FILEGROUP_H_

#include "File.h"

#include <icetray/I3Logging.h>

#include <vector>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace shovelio { namespace raw {

  /** raw::FileGroup makes a sequence of files appear like
   *  a single raw::File. For documentation of interface,
   *  see raw::File.
   */
  class FileGroup: private boost::noncopyable {
    SET_LOGGER("shovelio::raw");
  public:
    typedef boost::ptr_vector<File> Files;

  private:
    Files files_;
    unsigned current_file_;

  public:
    FileGroup();

    FileGroup(const std::vector<std::string>& filenames);

    const File& AddFile(const std::string& filename)
    { files_.push_back(new raw::File(filename));
      return files_.back(); }

    void CloseAllFiles();

    void CloseLastFile();

    bool Next(I3FramePtr, bool, const FilterList&);

    bool AtStreamPos(I3FramePtr, uint64_t, const FilterList&);

    uint64_t CurrentStreamPos();

    bool HasSize() const;

    uint64_t GetSize() const;

    unsigned GetNFiles() const { return files_.size(); }

    const std::string& GetPath(unsigned i) const
    { return files_[i].GetPath(); }

    File::Type GetType(unsigned i) const
    { return files_[i].GetType(); }
  };

}}

#endif
