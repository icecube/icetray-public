#include <atomic>
#include <fstream>
#include <future>
#include <queue>
#include <set>
#include <thread>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/make_shared.hpp>

#include <dataio/I3FileStager.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Module.h>
#include <icetray/I3TrayInfoService.h>
#include <icetray/open.h>

class I3AsyncWriter : public I3Module{
private:
	unsigned buffer_size_; //read only after Configure()
	std::vector<std::string> skip_keys_; //read only after Configure()
	std::vector<I3Frame::Stream> streams_; //read only after Configure()
	std::vector<I3Frame::Stream> dropOrphanStreams_; //read only after Configure()
	std::vector<boost::shared_ptr<I3Frame>> orphanarium_; //worker thread only
	bool configWritten_; //main thread only
	std::atomic<std::size_t> frameCounter_; //both threads
	int compression_level_; //read only after Configure()
	boost::iostreams::filtering_ostream filterstream_; //worker thread only after Configure and WriteConfig
	boost::shared_ptr<I3FileStager> file_stager_; //main thread only
	I3::dataio::shared_filehandle current_filename_; //main thread only
	
	std::thread worker_;
	std::chrono::steady_clock::time_point a_while_ago_; //main thread only
	std::queue<std::function<void()>> write_queue_; //both threads
	std::mutex write_lock_; //both threads, protects read_queue_
	std::queue<std::future<boost::shared_ptr<I3Frame>>> frame_queue_; //main thread
	std::condition_variable condition_; //both threads
	//indicates that no more writes will be queued
	std::atomic<bool> stop_flag_; //both threads
	//indicates that a failure has occurred and all in-flight writes should be dumped
	std::atomic<bool> abort_flag_; //both threads
	std::size_t queued_;
	
	void QueueWrite(boost::shared_ptr<I3Frame> frame); //main thread only
	void FetchNext(); //main thread only
	void WriteConfig(boost::shared_ptr<I3Frame> next_frame); //main thread only
	void PerformWrite(boost::shared_ptr<I3Frame> frame); //worker thread only
	
	SET_LOGGER("I3AsyncWriter");
public:
	I3AsyncWriter(const I3Context&);
	~I3AsyncWriter();
	
	void Configure();
	void Process();
	void Finish();
};

I3_MODULE(I3AsyncWriter);

I3AsyncWriter::I3AsyncWriter(const I3Context& context):
I3Module(context),
buffer_size_(128),
configWritten_(false),
frameCounter_(0),
compression_level_(0),
a_while_ago_(std::chrono::steady_clock::now()),
stop_flag_(false),
abort_flag_(false),
queued_(0)
{
	AddOutBox("OutBox");
	AddParameter("CompressionLevel", "0 == default compression, "
	             "1 == best speed, 9 == best compression (6 by default)",
	             compression_level_);
	
	AddParameter("SkipKeys",
	             "Don't write keys that match any of the regular expressions in "
	             "this vector", skip_keys_);
	
	AddParameter("Filename","The file we'll write to. If it ends with "
	             ".gz and no CompressionLevel is specified, it will be "
	             "gzipped at gzip's default compression level.", std::string());
	
	AddParameter("Streams", "Vector of I3Frame.Stream types that we should "
	             "write.  Those that do not appear here will be skipped. "
	             "Default: all streams enabled.", streams_);
	
	AddParameter("DropOrphanStreams", "Vector of I3Frame.Stream types to "
	             "drop if they are not followed by other frames. Default: drop "
	             "nothing", dropOrphanStreams_);
	
	AddParameter("BufferLength",
	             "The maximum number of frames to try to load before they may be needed",
	             buffer_size_);
}

void I3AsyncWriter::Configure(){
	std::string path;
	GetParameter("Filename", path);
	if (path.empty())
		log_fatal("Please specify a file name.");
	else
		log_trace("Filename = %s", path.c_str());
	
	GetParameter("SkipKeys", skip_keys_);
	GetParameter("CompressionLevel", compression_level_);
	GetParameter("BufferLength", buffer_size_);
	
	//no compatibility mode with lists of strings,
	//there are no old scripts which use this module
	GetParameter("Streams", streams_);
	
	GetParameter("DropOrphanStreams", dropOrphanStreams_);
	file_stager_ = context_.Get<boost::shared_ptr<I3FileStager>>();
	if (!file_stager_)
		file_stager_ = I3TrivialFileStager::create();
	
	current_filename_ = file_stager_->GetWriteablePath(path);
	I3::dataio::open(filterstream_, *current_filename_, compression_level_);
	
	//fire up our worker thread
	worker_=std::thread([this]{
		while(true){
			std::function<void()> task;
			{
				//hold the lock to manipulate the queue!
				std::unique_lock<std::mutex> lock(this->write_lock_);
				this->condition_.wait(lock,
					[this]{ return(this->abort_flag_.load() || this->stop_flag_.load() || !this->write_queue_.empty()); });
				if(this->abort_flag_.load())
					return;
				if(this->stop_flag_.load() && this->write_queue_.empty())
					return;
				task = std::move(this->write_queue_.front());
				this->write_queue_.pop();
			} //release the lock on the queue before going off to do the task
			task();
		}
	});
}

void I3AsyncWriter::Finish(){
	//Move frames from our inbox to the write queue.
	while(PeekFrame()){
		{
			std::lock_guard<std::mutex> g(write_lock_);
			auto old_size=write_queue_.size();
			while(write_queue_.size()<buffer_size_){
				if(PeekFrame())
					QueueWrite(PopFrame());
				else
					break;
			}
			if(write_queue_.size()>old_size){ //if we queued anything
				condition_.notify_one(); //make sure the worker wakes up to process it
				log_debug_stream("Queued " << write_queue_.size()-old_size << " frames to be written");
			}
		}
		//If we did not manage to empty the inbox it is necessary to block until
		//something finishes processing.
		if(PeekFrame()){
			log_debug("Waiting to push a frame");
			FetchNext();
		}
	}
	while(!frame_queue_.empty())
		FetchNext();
	
	//attempt to shut down cleanly
	stop_flag_=true; //tell the worker no more work will be added
	condition_.notify_all(); //make sure the worker wakes up at least once more
	worker_.join(); //wind down the worker thread
	
#if BOOST_VERSION < 104400
	if(frameCounter_ == 0 &&
		(current_filename_->rfind(".bz2") == current_filename_->size()-4) &&
		!streams_.empty() &&
		find(streams_.begin(), streams_.end(), I3Frame::TrayInfo)==streams_.end()){
			log_warn("You have attempted to write an empty file using bzip2 "
					 "compression with a Boost version prior to 1.44. A bug (fixed "
					 "in Boost 1.44) would normally cause the process to exit with "
					 "a bus error, To prevent this, either upgrade to a newer Boost,"
					 "switch to gzip compression, or ensure that you always write at "
					 "least 1 frame (for example, the TrayInfo frame). As a temporary "
					 "workaround, I have written the TrayInfo frame to the output "
					 "file for you.");
			// temporarily reconfigure output
			configWritten_ = false;
			std::vector<I3Frame::Stream> tempstreams(1, I3Frame::TrayInfo);
			tempstreams.swap(streams_);
			// write TrayInfo with a dummy frame
			WriteConfig(I3FramePtr(new I3Frame));
			// restore old configuration
			tempstreams.swap(streams_);
	}
#endif
	filterstream_.flush();
	
	current_filename_.reset(); //release staged file handle
	log_debug_stream(queued_ << " frames queued for writing.");
	log_info_stream(frameCounter_ << " frames written.");
	Flush();
}

I3AsyncWriter::~I3AsyncWriter(){
	//attempt to shut down hard
	if(worker_.joinable()){ //stop the worker if it didn't stop already
		abort_flag_=true; //tell the worker to drop whatever else it was doing
		condition_.notify_all(); //make sure the worker wakes up at least once more
		worker_.join(); //wind down the worker thread
	}
}

void I3AsyncWriter::Process(){
	//first, try to push as many frames as possible which have already been written
	while(!frame_queue_.empty()){
		//Check whether the next future is ready now, by waiting until a time
		//we're sure is well in the past.
		//This saves a call to the clock over using a duration of zero, since
		//that typically just calls the clock to find out the current time, then
		//passes that on to the absolute time version of wait_until.
		std::future_status tstatus=frame_queue_.front().wait_until(a_while_ago_);
		if(tstatus!=std::future_status::ready)
			break; //no point in waiting around, we have other stuff to do
		//The next future is ready! Fetch its value.
		//Note that if it contains an exception, that will be thrown now.
		FetchNext();
	}
	
	//Move frames from our inbox to the write queue.
	while(PeekFrame()){
		{
			std::lock_guard<std::mutex> g(write_lock_);
			auto old_size=write_queue_.size();
			while(write_queue_.size()<buffer_size_){
				if(PeekFrame())
					QueueWrite(PopFrame());
				else
					break;
			}
			if(write_queue_.size()>old_size){ //if we queued anything
				condition_.notify_one(); //make sure the worker wakes up to process it
				log_debug_stream("Queued " << write_queue_.size()-old_size << " frames to be written");
			}
		}
		//If we did not manage to empty the inbox it is necessary to block until
		//something finishes processing.
		if(PeekFrame()){
			log_debug("Waiting to push a frame");
			FetchNext();
		}
	}
}

///\return true if a valid frame was fetched and pushed, false if end of input reached
void I3AsyncWriter::FetchNext(){
	try{
		auto result=std::move(frame_queue_.front());
		frame_queue_.pop();
		boost::shared_ptr<I3Frame> frame=result.get();
		assert(frame); //all frames we process should remain valid
		PushFrame(frame);
	}catch(...){
		log_debug("Exception thrown, hitting emergency stop");
		//if we weren't already stopped, stop now
		abort_flag_=true;
		RequestSuspension();
		throw; //send the exception off to do its thing
	}
}

///\pre write_lock_ must be held
void I3AsyncWriter::QueueWrite(boost::shared_ptr<I3Frame> frame){
	if(!configWritten_) //this must happen in the main thread to avoid python issues
		WriteConfig(frame);
	frame->create_blobs(false,skip_keys_);
	auto task=std::make_shared<std::packaged_task<boost::shared_ptr<I3Frame>()>>(
		[this,frame](){
			this->PerformWrite(frame);
			return(frame);
		});
	frame_queue_.emplace(task->get_future());
	write_queue_.emplace([task](){ (*task)(); });
	queued_++;
}

//Note that this function is not allowed to PushFrame() because it is executing
//on the worker thread, but it also does not need to beacuse the surrounding task
//will pass the frame back to FetchNext() to be pushed.
void I3AsyncWriter::PerformWrite(boost::shared_ptr<I3Frame> frame){
	// See if this is a frame type that actually gets written
	if(!streams_.empty() &&
	   find(streams_.begin(), streams_.end(), frame->GetStop())==streams_.end())
		return;
	
	// See if this is one of our potentially orphanable frames. If so,
	// cache it for now.
	if(find(dropOrphanStreams_.begin(), dropOrphanStreams_.end(),
			 frame->GetStop()) != dropOrphanStreams_.end()) {
		unsigned i;
		// Replace one of our cached frames if it is the same type
		for(i=0; i<orphanarium_.size() &&
			 orphanarium_[i]->GetStop()!=frame->GetStop(); i++);
		
		// Duplicate frame so it can't be modified by following modules
		auto orphan=boost::make_shared<I3Frame>(frame->GetStop());
		orphan->merge(*frame);
		if(i==orphanarium_.size())	// No match, add to end
			orphanarium_.push_back(orphan);
		else				// Replace in situ
			orphanarium_[i]=orphan;
		return;
	}
	
	// At this point, we have a frame we (a) want to write, and (b) is not
	// part of an orphanable stream, so dump the contents of the orphanarium
	// to disk before the frame and clear it.
	for(const auto& adopted : orphanarium_) {
		frameCounter_++;
		adopted->save(filterstream_, skip_keys_);
	}
	orphanarium_.clear();
	
	// Write to disk
	frameCounter_++;
	frame->save(filterstream_, skip_keys_);
	log_trace("Wrote one frame");
}

void I3AsyncWriter::WriteConfig(boost::shared_ptr<I3Frame> frame){
	if(!streams_.empty() && find(streams_.begin(),streams_.end(),I3Frame::TrayInfo)==streams_.end()){
		configWritten_=true;
		return;
	}
		
	std::string timestr=to_iso_extended_string(boost::posix_time::microsec_clock::universal_time());
	I3TrayInfoService& srv=context_.Get<I3TrayInfoService>("__tray_info_service");
	boost::shared_ptr<I3TrayInfo> config(new I3TrayInfo(srv.GetConfig()));
	
	// either make new frame or merge to existing frame
	boost::shared_ptr<I3Frame> oframe;
	if(frame->GetStop() == I3Frame::TrayInfo)
		oframe=frame;
	else
		oframe=boost::shared_ptr<I3Frame>(new I3Frame(I3Frame::TrayInfo));
	
	while(oframe->Has(timestr))
		timestr+="~";
	
	oframe->Put(timestr, config);
	
	// If we didn't make the frame, don't save it, since it is already going to
	// be written. Otherwise we have to write it.
	if(oframe!=frame){
		frameCounter_++;
		oframe->save(filterstream_, skip_keys_);
	}
	
	configWritten_ = true;
}
