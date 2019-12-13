#include <iostream>
#include <istream>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <stdexcept>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>

#include <icetray/I3Logging.h>
#include <icetray/open.h>
#include <icetray/I3DefaultName.h>
#include <icetray/I3FrameMixing.h>
#include <dataio/I3File.h>
#include <dataio/I3FrameSequence.h>
#include <dataclasses/physics/I3EventHeader.h>


namespace {
    // Run a function in a worker thread
    template <typename R>
    class ThreadRunner
    {
        using T = R();
    public:
        ThreadRunner() : running_(true)
        {
            thread_ = std::thread([this]{ run(); });
        }
        ~ThreadRunner()
        {
            running_ = false;
            cond_.notify_one();
            thread_.join();
        }
        std::future<R> push(std::function<T> element) {
            std::lock_guard<std::mutex> lock(mutex_);
            data_.emplace(element);
            cond_.notify_all();
            return data_.back().get_future();
        }
        void run() {
            while(running_) {
                // put lock here so it unlocks every loop
                std::unique_lock<std::mutex> lock(mutex_);
                if (!data_.empty()) {
                    auto work = std::move(data_.front());
                    data_.pop();
                    lock.unlock();
                    work();
                } else {
                    cond_.wait(lock);
                }
            }
        }
        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return data_.empty();
        }
        void clear() {
            std::lock_guard<std::mutex> lock(mutex_);
            while(!data_.empty()) {
                data_.pop();
            }
        }
        size_t size() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return data_.size();
        }
    private:
        std::queue<std::packaged_task<T>> data_;
        std::condition_variable cond_;
        std::thread thread_;
        mutable std::mutex mutex_;
        std::atomic<bool> running_;
    };

    /* A thread-safe frame cache
     * Stores frames from [first_index, last_index).
     * If first_index = last_index, cache is empty.
     */
    class FrameCache
    {
    public:
        FrameCache(const FrameCache&);
        explicit FrameCache(size_t);

        void add(size_t, std::vector<I3FramePtr>);
        std::vector<I3FramePtr> get(size_t) const;
        void clear();

    private:
        std::deque<std::vector<I3FramePtr>> frames_;
        size_t first_index_;
        size_t last_index_;
        size_t max_size_;
        mutable std::mutex mutex_;
    };

    FrameCache::FrameCache(size_t s) :
        first_index_(0), last_index_(0), max_size_(s)
    { }

    FrameCache::FrameCache(const FrameCache& rhs) :
        frames_(rhs.frames_), first_index_(rhs.first_index_),
        last_index_(rhs.last_index_), max_size_(rhs.max_size_)
    { }

    void FrameCache::add(size_t index, std::vector<I3FramePtr> fr)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (index >= last_index_) {
            // insert
            size_t tmp_index = index;
            while (tmp_index > last_index_) {
                frames_.emplace_back(); // add empty frames
                last_index_++;
            }
            frames_.push_back(fr);
            last_index_++;
            // resize
            if (last_index_ - first_index_ > max_size_) {
                tmp_index = last_index_ - max_size_;
                while(first_index_ < tmp_index) {
                    frames_.pop_front();
                    first_index_++;
                }
            }
        } else if (index < first_index_) {
            // insert
            size_t tmp_index = index+1;
            while (tmp_index < first_index_) {
                frames_.emplace_front(); // add empty frames
                first_index_--;
            }
            frames_.push_front(fr);
            first_index_--;
            // resize
            if (last_index_ - first_index_ > max_size_) {
                tmp_index = first_index_ + max_size_;
                while(last_index_ > tmp_index) {
                    frames_.pop_back();
                    last_index_--;
                }
            }
        } else {
            // index already in cache
            size_t tmp_index = index-first_index_;
            frames_[tmp_index].swap(fr);
        }
    }

    std::vector<I3FramePtr> FrameCache::get(size_t index) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (index < first_index_ || index >= last_index_) {
            std::vector<I3FramePtr> ret;
            return ret;
        } else {
            size_t tmp_index = index-first_index_;
            return frames_[tmp_index];
        }
    }

    void FrameCache::clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        frames_.clear();
        first_index_ = last_index_ = 0;
    }


    struct FileStruct
    {
        dataio::I3File file;
        std::vector<I3FramePtr> last_frames;
        bool has_size;
        FileStruct(std::string path) :
            file(path, dataio::I3File::Mode::read, 0, false),
            has_size(false) { }
        FileStruct(std::string path, bool size) :
            file(path, dataio::I3File::Mode::read, 0, false),
            has_size(size) { }
    };

    // A thread-safe file access
    class FileGroup
    {
    public:
        FileGroup();
        FileGroup(const FileGroup&);
        explicit FileGroup(const std::vector<std::string>&);

        void add(const std::string&);
        void clear();
        void remove_last();

        std::vector<I3FramePtr> get_frame(size_t);

        std::vector<std::string> get_paths() const;
        ssize_t get_size() const;
        size_t get_cur_size() const;

    private:
        std::vector<FileStruct> files_;
        mutable std::mutex mutex_;
    };

    FileGroup::FileGroup()
    { }

    FileGroup::FileGroup(const FileGroup& rhs) :
        files_(rhs.files_)
    { }

    FileGroup::FileGroup(const std::vector<std::string>& paths)
    {
        for(auto path : paths) {
            files_.emplace_back(path, false);
        }
    }

    void FileGroup::add(const std::string& path)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        files_.emplace_back(path, false);
    }

    void FileGroup::clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        files_.clear();
    }

    void FileGroup::remove_last()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        files_.pop_back();
    }

    std::vector<I3FramePtr> FileGroup::get_frame(size_t frame_index)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        I3FrameMixer mixer(true, true);
        ssize_t tmp_index(frame_index);
        log_debug("FileGroup: get_frame %lu", frame_index);

        // find correct file
        for (auto& fs : files_) {
            if (fs.has_size) {
                size_t file_size = fs.file.get_size();
                log_trace("fs.has_size: %lu", file_size);
                if (file_size == 0) {
                    continue; // empty file
                } else if (tmp_index < static_cast<ssize_t>(file_size)) {
                    // seek to frame, mix the frames, and return
                    fs.file.seek(tmp_index);
                    fs.file.pop_frame();
                    auto frames = fs.file.get_current_frame_and_deps();
                    i3_assert(!frames.empty());
                    for (const auto& f : frames) {
                        mixer.UpdateDependencies(*f);
                    }
                    auto ret = mixer.GetMixedFrames(frames.back()->GetStop());
                    ret.push_back(frames.back());
                    return ret;
                } else {
                    // mix in the last frames in this file
                    for (const auto& f : fs.last_frames) {
                        try {
                            mixer.UpdateDependencies(*f);
                        } catch(const std::bad_alloc &e) {
                            log_warn("here");
                            throw;
                        }
                    }
                }
            } else {
                log_trace("!fs.has_size, frameno: %ld", fs.file.get_frameno());
                if (tmp_index < fs.file.get_frameno()) {
                    fs.file.rewind();
                }
                if (tmp_index != fs.file.get_frameno()) {
                    while (tmp_index > fs.file.get_frameno() && fs.file.more()) {
                        fs.file.pop_frame();
                    }
                }
                if (tmp_index == fs.file.get_frameno()) {
                    // found frame
                    log_trace("found frame");
                    auto frames = fs.file.get_current_frame_and_deps();
                    i3_assert(!frames.empty());
                    for (const auto& f : frames) {
                        mixer.UpdateDependencies(*f);
                    }
                    auto ret = mixer.GetMixedFrames(frames.back()->GetStop());
                    ret.push_back(frames.back());
                    return ret;
                } else {
                    // end of file
                    log_trace("EOF");
                    fs.has_size = true;
                    if (fs.file.get_size() == 0) {
                        continue; // empty file
                    }
                    auto frames = fs.file.get_current_frame_and_deps();
                    i3_assert(!frames.empty());
                    fs.last_frames.clear();
                    for (const auto& f : frames) {
                        fs.last_frames.emplace_back(new I3Frame(*f));
                        mixer.UpdateDependencies(*f);
                    }
                }
            }
            log_trace("file size: %lu, index: %ld", fs.file.get_size(), tmp_index);
            i3_assert(static_cast<ssize_t>(fs.file.get_size()) <= tmp_index);
            tmp_index -= fs.file.get_size();
        }

        // if nothing there, return an empty vector
        std::vector<I3FramePtr> ret;
        return ret;
    }

    std::vector<std::string> FileGroup::get_paths() const
    {
        std::vector<std::string> ret;
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& fs : files_) {
            ret.push_back(fs.file.get_path());
        }
        return ret;
    }

    ssize_t FileGroup::get_size() const
    {
        ssize_t ret = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& fs : files_) {
            if (!fs.has_size) {
                return -1;
            }
            ret += fs.file.get_size();
        }
        return ret;
    }

    size_t FileGroup::get_cur_size() const
    {
        size_t ret = 0;
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& fs : files_) {
            ret += fs.file.get_size();
            if (!fs.has_size) {
                break;
            }
        }
        return ret;
    }



}


namespace dataio {

    // Actual class for I3FrameSequence implementation
    class I3FrameSequenceImpl
    {
    public:
        I3FrameSequenceImpl(const I3FrameSequenceImpl&);
        explicit I3FrameSequenceImpl(size_t);
        explicit I3FrameSequenceImpl(const std::vector<std::string>&, size_t);
        ~I3FrameSequenceImpl();

        void add_file(const std::string&);
        void close();
        void close_last_file();
        void rewind();
        bool more();
        I3FramePtr pop_frame(I3Frame::Stream = I3Frame::None); 
        void seek(size_t);
        std::vector<I3FramePtr> get_mixed_frames();
        std::vector<I3FramePtr> get_current_frame_and_deps();
        std::vector<std::string> get_paths() const;
        ssize_t get_frameno() const;
        ssize_t get_size() const;
        size_t get_cur_size() const;
        I3Frame::Stream get_stream() const;

    private:
        FileGroup files_; //!< files
        FrameCache cache_; //!< frame cache
        ThreadRunner<std::vector<I3FramePtr>> tr_; //!< thread runner
        std::unordered_map<size_t,std::future<std::vector<I3FramePtr>>> tr_cache_;
        size_t frameno_; //!< "next" frame number
    };

    I3FrameSequenceImpl::I3FrameSequenceImpl(const I3FrameSequenceImpl& rhs) :
        files_(rhs.files_),
        cache_(rhs.cache_),
        frameno_(rhs.frameno_)
    { }

    I3FrameSequenceImpl::I3FrameSequenceImpl(size_t size) :
        cache_(size), frameno_(0)
    { }

    I3FrameSequenceImpl::I3FrameSequenceImpl(const std::vector<std::string>& paths, size_t size) :
        files_(paths),
        cache_(size),
        frameno_(0)
    {
        // pre-fetch 10 frames
        for(uint8_t i=0;i<10;i++) {
            auto idx=frameno_+i;
            tr_cache_.emplace(frameno_+i,tr_.push([=]{return files_.get_frame(idx);}));                    
        }
    }

    I3FrameSequenceImpl::~I3FrameSequenceImpl()
    { }

    void I3FrameSequenceImpl::add_file(const std::string& path)
    {
        files_.add(path);
        
        // pre-fetch 10 frames
        for(uint8_t i=0;i<10;i++) {
            auto newframeno = frameno_ + i;
            if (cache_.get(newframeno).empty()) {
                const auto tr_cache_iter = tr_cache_.find(newframeno);
                if (tr_cache_iter == tr_cache_.end()) {
                    tr_cache_.emplace(newframeno,tr_.push([=]{return files_.get_frame(newframeno);}));                            
                }
            }
        }
    }

    void I3FrameSequenceImpl::close()
    {
        tr_.clear();
        tr_cache_.clear();
        cache_.clear();
        files_.clear();
        frameno_ = 0;
    }

    void I3FrameSequenceImpl::close_last_file()
    {
        tr_.clear();
        tr_cache_.clear();
        files_.remove_last();
    }

    void I3FrameSequenceImpl::rewind()
    {
        tr_.clear();
        tr_cache_.clear();
        frameno_ = 0;

        // pre-fetch 10 frames
        for(uint8_t i=0;i<10;i++) {
            auto newframeno = frameno_ + i;
            if (cache_.get(newframeno).empty()) {
                const auto tr_cache_iter = tr_cache_.find(newframeno);
                if (tr_cache_iter == tr_cache_.end()) {
                    tr_cache_.emplace(newframeno,tr_.push([=]{return files_.get_frame(newframeno);}));                            
                }
            }
        }
    }

    bool I3FrameSequenceImpl::more()
    {
        // check frame cache
        std::vector<I3FramePtr> fr = cache_.get(frameno_);
        if (fr.empty()) {
            // check future cache
            const auto tr_cache_iter = tr_cache_.find(frameno_);
            if (tr_cache_iter != tr_cache_.end()) {
		try {
                    fr = tr_cache_iter->second.get(); // get from future
                } catch(icecube::archive::archive_exception& ex) {
                    log_warn_stream("Unexpected end of input file: " << ex.what());
                }
                tr_cache_.erase(tr_cache_iter);
            } else {
                // make future, then wait on it
		auto f = tr_.push([=]{return files_.get_frame(frameno_);});
                try {
                    fr = f.get();
                } catch(icecube::archive::archive_exception& ex) {
                    log_warn_stream("Unexpected end of input file: " << ex.what());
                    return false;
                }	
            }
            if (!fr.empty()) {
                // add to frame cache
                cache_.add(frameno_, fr);
            }
        }
        return !fr.empty();
    }

    I3FramePtr I3FrameSequenceImpl::pop_frame(I3Frame::Stream s)
    {
        std::vector<I3FramePtr> fr;
        while(true) {
            // check frame cache
            fr = cache_.get(frameno_);
            if (fr.empty()) {
                // check future cache
                const auto tr_cache_iter = tr_cache_.find(frameno_);
                if (tr_cache_iter != tr_cache_.end()) {
                    //log_info("pop_frame from future cache");
                    fr = tr_cache_iter->second.get(); // get from future
                    tr_cache_.erase(tr_cache_iter);
                } else {
                    //log_info("pop_frame from new future");
                    // make future, then wait on it
                    auto f = tr_.push([=]{return files_.get_frame(frameno_);});
                    fr = f.get();
                }
                if (!fr.empty()) {
                    // add to frame cache
                    cache_.add(frameno_, fr);
                }
            } //else { //log_info("pop_frame from frame cache"); }
            if (fr.empty()) {
                log_fatal_stream("no frame at index " << frameno_);
            }
            frameno_++;
            if (s == I3Frame::None || s == fr.back()->GetStop()) {
                break; // found a valid frame
            }
            // else, this is not the stream we are looking for
        }
        if (fr.empty()) {
            log_fatal("no frame at that index");
        } else {
            // pre-fetch 10 frames after this
            for(uint8_t i=0;i<10;i++) {
                auto newframeno = frameno_ + i;
                if (cache_.get(newframeno).empty()) {
                    const auto tr_cache_iter = tr_cache_.find(newframeno);
                    if (tr_cache_iter == tr_cache_.end()) {
                        tr_cache_.emplace(newframeno,tr_.push(
                                [=]{return files_.get_frame(newframeno);}));
                    }
                }
            }
            
            // mix vector of frames to produce result
            I3FrameMixer mixer;
            for (auto& f : fr) {
                mixer.Mix(*f);
            }
            return fr.back();
        }
    }

    void I3FrameSequenceImpl::seek(size_t number)
    {
        //log_info("Seek to frame %ld", number);
        frameno_ = number;
        // pre-fetch 10 frames after this
        //log_warn("future_size: %ld", tr_cache_.size());
        for(uint8_t i=0;i<10;i++) {
            auto newframeno = frameno_ + i;
            if (newframeno > files_.get_cur_size()) {
                break; // don't do repeated exploratory seeking, wait for pop_frame
            }
            if (cache_.get(newframeno).empty()) {
                const auto tr_cache_iter = tr_cache_.find(newframeno);
                if (tr_cache_iter == tr_cache_.end()) {
                    tr_cache_.emplace(newframeno,tr_.push(
                            [=]{return files_.get_frame(newframeno);}));
                }
            }
        }
    }

    std::vector<I3FramePtr> I3FrameSequenceImpl::get_mixed_frames()
    {
        if (frameno_ == 0) {
            std::vector<I3FramePtr> ret;
            return ret;
        }
        auto fr = cache_.get(frameno_-1);
        if (fr.empty()) {
            log_fatal("previous frame not in the cache");
        }
        fr.pop_back();
        return fr;
    }

    std::vector<I3FramePtr> I3FrameSequenceImpl::get_current_frame_and_deps()
    {
        if (frameno_ == 0) {
            std::vector<I3FramePtr> ret;
            return ret;
        }
        auto fr = cache_.get(frameno_-1);
        if (fr.empty()) {
            log_fatal("previous frame not in the cache");
        }
        return fr;
    }

    std::vector<std::string> I3FrameSequenceImpl::get_paths() const
    {
        return files_.get_paths();
    }

    ssize_t I3FrameSequenceImpl::get_frameno() const
    {
        ssize_t ret(frameno_);
        ret--;
        return ret;
    }

    ssize_t I3FrameSequenceImpl::get_size() const
    {
        return files_.get_size();
    }

    size_t I3FrameSequenceImpl::get_cur_size() const
    {
        return files_.get_cur_size();        
    }

    I3Frame::Stream I3FrameSequenceImpl::get_stream() const
    {
        if (frameno_ == 0) {
            return I3Frame::None;
        }
        auto fr = cache_.get(frameno_-1);
        if (fr.empty()) {
            log_fatal("previous frame not in the cache");
        }
        return fr.back()->GetStop();
    }


    /* I3FrameSequence */
    /* constructors */
    
    I3FrameSequence::I3FrameSequence() :
        impl_(new I3FrameSequenceImpl(1000))
    { }

    I3FrameSequence::I3FrameSequence(const I3FrameSequence& rhs) :
        impl_(new I3FrameSequenceImpl(*rhs.impl_))
    { }

    I3FrameSequence::I3FrameSequence(const std::vector<std::string>& paths, size_t size) :
        impl_(new I3FrameSequenceImpl(paths, size))
    { }

    I3FrameSequence::I3FrameSequence(size_t size) :
        impl_(new I3FrameSequenceImpl(size))
    { }

    I3FrameSequence::~I3FrameSequence()
    { }

    void I3FrameSequence::add_file(const std::string& path)
    {
        impl_->add_file(path);
    }

    void I3FrameSequence::close()
    {
        impl_->close();
    }

    void I3FrameSequence::close_last_file()
    {
        impl_->close_last_file();
    }

    void I3FrameSequence::rewind()
    {
        impl_->rewind();
    }

    bool I3FrameSequence::more()
    {
        return impl_->more();
    }

    I3FramePtr I3FrameSequence::pop_frame(I3Frame::Stream s)
    {
        return impl_->pop_frame(s);
    }

    void I3FrameSequence::seek(size_t number)
    {
        impl_->seek(number);
    }

    I3FramePtr I3FrameSequence::operator[](size_t number)
    {
        //log_debug("getting frame at index %ld", number);
        impl_->seek(number);
        if (!impl_->more())
            throw std::runtime_error("no frame at that index");
        I3FramePtr ret = impl_->pop_frame();
        //log_debug("got frame at index %ld", number);
        return ret;
    }

    std::vector<I3FramePtr> I3FrameSequence::get_mixed_frames()
    {
        return impl_->get_mixed_frames();
    }

    std::vector<I3FramePtr> I3FrameSequence::get_current_frame_and_deps()
    {
        return impl_->get_current_frame_and_deps();
    }

    std::vector<std::string> I3FrameSequence::get_paths() const
    {
        return impl_->get_paths();
    }

    ssize_t I3FrameSequence::get_frameno() const
    {
        return impl_->get_frameno();
    }

    ssize_t I3FrameSequence::get_size() const
    {
        return impl_->get_size();        
    }

    size_t I3FrameSequence::get_cur_size() const
    {
        return impl_->get_cur_size();        
    }

    I3Frame::Stream I3FrameSequence::get_stream() const
    {
        return impl_->get_stream();
    }

}
