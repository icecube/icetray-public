#include <icetray/I3Module.h>

#include <atomic>
#include <fstream>
#include <future>
#include <queue>
#include <set>
#include <thread>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/make_shared.hpp>

#include <icetray/open.h>
#include <icetray/I3Frame.h>
#include <dataio/I3FileStager.h>

class I3AsyncReader : public I3Module{
	unsigned buffer_size_; //read only after Configure()
	unsigned nframes_; //worker thread only after Configure()
	unsigned npushed_;
	bool drop_blobs_; //read only after Configure()
	std::atomic<bool> stop_flag_; //both threads
	std::vector<std::string> filenames_; //read only after Configure()
	std::vector<std::string> skip_; //read only after Configure()
	I3FileStagerPtr file_stager_; //worker thread only after Configure()
	I3::dataio::shared_filehandle current_filename_; //worker thread only after Configure()
	boost::iostreams::filtering_istream ifs_; //worker thread only after Configure()
	std::vector<std::string>::iterator filenames_iter_; //worker thread only after Configure()
	
	std::thread worker_;
	std::chrono::steady_clock::time_point a_while_ago_; //main thread only
	std::queue<std::function<void()>> read_queue_; //both threads
	std::mutex read_lock_; //both threads, protects read_queue_
	std::queue<std::future<boost::shared_ptr<I3Frame>>> frame_queue_; //main thread
	std::condition_variable condition_; //both threads
	
	void QueueRead(); //main thread only
	bool FetchNext(); //main thread only
	boost::shared_ptr<I3Frame> PerformRead(); //worker thread only
	void OpenNextFile(); //worker thread only after Configure()
	
	SET_LOGGER("I3AsyncReader");
public:
	I3AsyncReader(const I3Context&);
	~I3AsyncReader();
	
	void Configure();
	void Process();
};

I3_MODULE(I3AsyncReader);

I3AsyncReader::I3AsyncReader(const I3Context& context):
I3Module(context),
buffer_size_(128),
nframes_(0),
npushed_(0),
drop_blobs_(false),
stop_flag_(false),
a_while_ago_(std::chrono::steady_clock::now()){
	AddParameter("Filename",
	             "Filename to read.  Use either this or Filenamelist, not both.",
	             std::string());
	
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
	
	AddParameter("BufferLength",
	             "The maximum number of frames to try to load before they may be needed",
	             buffer_size_);
	
	AddOutBox("OutBox");
}

I3AsyncReader::~I3AsyncReader(){
	log_debug_stream("Pushed " << npushed_ << " frames");
	if(worker_.joinable()){
		stop_flag_=true; //tell the worker not to try to do anything else
		condition_.notify_all(); //make sure the worker wakes up at least once more
		worker_.join(); //wind down the worker thread
	}
}

void I3AsyncReader::Configure(){
	std::string fname;
	
	GetParameter("FileNameList", filenames_);
	GetParameter("FileName", fname);
	if(!filenames_.empty() && !fname.empty())
		log_fatal("Both Filename and FileNameList were specified.");
	
	if (filenames_.empty() && fname.empty())
		log_fatal("Neither 'Filename' nor 'FilenameList' specified");
	
	if (filenames_.empty())
		filenames_.push_back(fname);
	
	GetParameter("SkipKeys", skip_);
	GetParameter("DropBuffers", drop_blobs_);
	GetParameter("BufferLength", buffer_size_);
	
	file_stager_ = context_.Get<I3FileStagerPtr>();
	if (!file_stager_)
		file_stager_ = I3TrivialFileStager::create();
	for(const std::string& filename : filenames_)
		file_stager_->WillReadLater(filename);
	
	filenames_iter_ = filenames_.begin();
	OpenNextFile();
	//fire up our worker thread
	worker_=std::thread([this]{
		while(true){
			std::function<void()> task;
			{
				//hold the lock to manipulate the queue!
				std::unique_lock<std::mutex> lock(this->read_lock_);
				this->condition_.wait(lock,
				  [this]{ return(this->stop_flag_.load() || !this->read_queue_.empty()); });
				if(this->stop_flag_.load())
					return;
				task = std::move(this->read_queue_.front());
				this->read_queue_.pop();
			} //release the lock on the queue before going off to do the task
			task();
		}
	});
}

void
I3AsyncReader::Process(){
	bool pushed=false;
	//first, try to push as many frames as possible which have already been read
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
		pushed=FetchNext();
		if(!pushed) //input ended
			return;
	}
	//Now, we may not have a full complement of outstanding read requests.
	//Queue some more if necessary.
	{
		//grab the lock corresponding to the work queue before touching it
		std::lock_guard<std::mutex> g(read_lock_);
		auto old_size=read_queue_.size();
		while(read_queue_.size()<buffer_size_)
			QueueRead();
		if(read_queue_.size()>old_size){ //if we queued anything
			condition_.notify_one();
			log_debug_stream("Queued " << buffer_size_-old_size << " frames to be read");
		}
	}
	//Make sure we emit at least one frame, if possible
	if(pushed)
		return;
	assert(!frame_queue_.empty());
	//wait can block until something finshes reading, there is determined to
	//be no more input, or an error occurs
	log_debug("Waiting to push a frame");
	frame_queue_.front().wait();
	log_debug("Got a frame to push");
	FetchNext();
//	Flush();
}

///\return true if a valid frame was fetched and pushed, false if end of input reached
bool I3AsyncReader::FetchNext(){
	try{
		auto result=std::move(frame_queue_.front());
		frame_queue_.pop();
		boost::shared_ptr<I3Frame> frame=result.get();
		if(!frame){
			RequestSuspension();
			return(false);
		}
		PushFrame(frame);
		npushed_++;
		log_trace("Pushed one frame");
	}catch(...){
		log_debug("Exception thrown, hitting emergency stop");
		//if we weren't already stopped, stop now
		stop_flag_=true;
		RequestSuspension();
		throw; //send the exception off to do its thing
	}
	return(true);
}

///\pre read_lock_ must be held
void I3AsyncReader::QueueRead(){
	auto task=std::make_shared<std::packaged_task<boost::shared_ptr<I3Frame>()>>(
		[this](){ return(this->PerformRead()); });
	frame_queue_.emplace(task->get_future());
	read_queue_.emplace([task](){ (*task)(); });
}

void I3AsyncReader::OpenNextFile(){
	if (!ifs_.empty())
		ifs_.pop();
	ifs_.reset();
	assert(ifs_.empty());
	
	if(nframes_>0)
		log_debug_stream("Read " << nframes_ << " frames from last file");
	current_filename_.reset(); //release staged file handle
	current_filename_ = file_stager_->GetReadablePath(*filenames_iter_);
	nframes_ = 0;
	filenames_iter_++;
	
	I3::dataio::open(ifs_, *current_filename_);
	log_info_stream("Opened file " << *current_filename_);
}

boost::shared_ptr<I3Frame> I3AsyncReader::PerformRead(){
	boost::shared_ptr<I3Frame> frame;
	
	while(ifs_.peek() == EOF){
		if(filenames_iter_ == filenames_.end()){
			stop_flag_=true;
			log_debug_stream("Read " << nframes_ << " frames from last file");
			current_filename_.reset(); //release staged file handle
			return(frame);
		}
		else
			OpenNextFile();
    }
	
	frame.reset(new I3Frame);
	frame->drop_blobs(drop_blobs_);
	try {
		nframes_++;
		frame->load(ifs_, skip_);
	} catch (const std::exception& e) {
		log_fatal_stream("Error reading " << *current_filename_ <<
		                 " at frame " << nframes_ << ": " << e.what());
	}
	log_debug("Read one frame");
	return(frame);
}
