#include <iostream>
#include <istream>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>
#include <stdexcept>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include <icetray/I3Logging.h>
#include <icetray/open.h>
#include <icetray/I3DefaultName.h>
#include <dataio/I3File.h>
#include <dataclasses/physics/I3EventHeader.h>

using boost::algorithm::starts_with;
using boost::filesystem::exists;

namespace dataio {

    /* constructors */

    I3File::I3File(const I3File& rhs) :
        path_(rhs.path_), cache_(rhs.cache_),
        curr_frame_(rhs.curr_frame_), frameno_(rhs.frameno_),
        size_(rhs.size_), mode_(rhs.mode_), type_(rhs.type_)
    {
        open_file();
    }

    I3File::I3File(const std::string& path) :
        I3File(path, Mode::read)
    { }

    I3File::I3File(const std::string& path, const std::string& mode,
                   size_t frames) :
        path_(path), cache_(true), frameno_(frames), size_(0),
        mode_(Mode::read), type_(Type::closed)
    {
        char m = ::tolower(mode[0]);
        if (strchr("rwxa", m)) {
            mode_ = static_cast<Mode>(m);
        } else {
            log_fatal("bad mode");
        }
        open_file();
    }

    I3File::I3File(const std::string& path, Mode mode, size_t frames, bool mixing) :
        path_(path), cache_(true, !mixing), frameno_(frames), size_(0),
        mode_(mode), type_(Type::closed)
    {
        open_file();
    }

    I3File::~I3File()
    {
        close();
    }

    /* public methods */

    void I3File::close()
    {
        if (type_ != Type::closed) {
            ifs_.reset();
            ofs_.reset();
            cache_.Reset();
            type_ = Type::closed;
        }
    }

    void I3File::rewind()
    {
        if (type_ != Type::multipass) {
            log_fatal("file cannot be rewound");
        }

        Type old_type = type_;
        close();
        type_ = old_type;
        frameno_ = 0;
        curr_frame_.reset();
        open_file();
    }

    bool I3File::more()
    {
        if (type_ == Type::closed || mode_ != Mode::read) {
            return false;
        }

        return (ifs_.peek() != EOF && ifs_.good());
    }

    void I3File::push(I3FramePtr fr)
    {
        if (type_ == Type::closed) {
            log_fatal("file already closed");
        }
        if (mode_ == Mode::read) {
            log_fatal("file not in write mode");
        }

        fr->save(ofs_);
    }

    I3FramePtr I3File::pop_frame(I3Frame::Stream s)
    {
        if (type_ == Type::closed) {
            log_fatal("file already closed");
        }
        if (mode_ != Mode::read) {
            log_fatal("file not in read mode");
        }

        while(more()) {
            curr_frame_.reset(new I3Frame);
            curr_frame_->load(ifs_);
            if (cache_.MixingDisabled()) {
                cache_.UpdateDependencies(*curr_frame_);
            } else {
                cache_.Mix(*curr_frame_);
            }
            frameno_++;
            if (frameno_ > size_) {
                size_ = frameno_;
            }
            if (s == I3Frame::None || curr_frame_->GetStop() == s) {
                return curr_frame_;
            }
            log_debug("skipping frame");
        }
        log_fatal("no frame to pop");
    }

    void I3File::seek(size_t index_num)
    {
        if (type_ == Type::closed) {
            log_fatal("file already closed");
        }
        if (mode_ != Mode::read) {
            log_fatal("file not in read mode");
        }

        if (index_num < frameno_) {
            rewind();
        }
        if (index_num != frameno_) {
            log_trace_stream("Skipping frames: index_num=" << index_num << " frameno_=" << frameno_);
            skip_frames(index_num-frameno_-1);
            if (!more()) {
                log_fatal("index not in file");
            }
        }
    }

    std::vector<I3FramePtr> I3File::get_mixed_frames()
    {
        if (curr_frame_) {
            return cache_.GetMixedFrames(curr_frame_->GetStop());
        } else {
            std::vector<I3FramePtr> ret;
            return ret;
        }
    }

    std::vector<I3FramePtr> I3File::get_current_frame_and_deps()
    {
        if (curr_frame_) {
            std::vector<I3FramePtr> ret(cache_.GetMixedFrames(curr_frame_->GetStop()));
            ret.push_back(curr_frame_);
            return ret;
        } else {
            std::vector<I3FramePtr> ret;
            return ret;
        }
    }

    std::string I3File::get_path() const
    {
        std::string ret(path_);
        return ret;
    }

    ssize_t I3File::get_frameno() const
    {
        ssize_t ret(frameno_);
        ret--;
        return ret;
    }

    size_t I3File::get_size() const
    {
        return size_;
    }

    I3Frame::Stream I3File::get_stream() const
    {
        if (curr_frame_) {
            return curr_frame_->GetStop();
        } else {
            return I3Frame::None;
        }
    }

    I3File::Mode I3File::get_mode() const
    {
        return mode_;
    }

    I3File::Type I3File::get_type() const
    {
        return type_;
    }

    /* private methods */

    void I3File::open_file()
    {
        log_debug("open_file %s", path_.c_str());
        if (mode_ == Mode::read) {
            I3::dataio::open(ifs_, path_);

            if(!ifs_) {
                type_ = Type::closed;
                return;
            } else if (starts_with(path_, "http://")
                       || starts_with(path_, "socket://")) {
                type_ = Type::singlepass;
            } else if (ifs_.peek() == EOF || ifs_.bad()) {
                type_ = Type::empty;
                return;
            } else {
                type_ = Type::multipass;
            }

            skip_frames(frameno_);
        } else {
            std::ios::openmode m = std::ios::binary | std::ios::out;
            if (mode_ == Mode::write) {
                m |= std::ios::trunc;
            } else if (mode_ == Mode::create) {
                // test for file existence, if a file
                if (exists(path_)) {
                    log_fatal("file already exists");
                }
            } else if (mode_ == Mode::append) {
                m |= std::ios::app;
            }
            I3::dataio::open(ofs_, path_, 0, m);
            if (!ofs_) {
                type_ = Type::closed;
            } else {
                type_ = Type::singlepass;
            }
        }
    }

    void I3File::skip_frames(size_t skip_n)
    {
        if (type_ == Type::closed) {
            log_fatal("file already closed");
        }
        if (mode_ != Mode::read) {
            log_fatal("file not in read mode");
        }

        size_t n = 0;
        while(n < skip_n && more()) {
            curr_frame_.reset(new I3Frame);
            curr_frame_->load(ifs_);
            if (cache_.MixingDisabled()) {
                cache_.UpdateDependencies(*curr_frame_);
            } else {
                cache_.Mix(*curr_frame_);
            }
            n++;
            frameno_++;
            if (frameno_ > size_) {
                size_ = frameno_;
            }
            log_debug("skipping frame");
        }
        if (n < skip_n) {
            log_fatal_stream("not enough frames in file (" << n << ") to skip " << skip_n);
        }
    }

} // end namespace dataio
