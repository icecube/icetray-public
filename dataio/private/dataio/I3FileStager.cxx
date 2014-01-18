#include "dataio/I3FileStager.h"
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

namespace I3 { namespace dataio {

filehandle::filehandle(const std::string &path)
    : std::string(path)
{}

filehandle::filehandle(const std::string &path, boost::function<void()> destructor)
    : std::string(path), callback_(destructor)
{}

filehandle::~filehandle()
{
	if (callback_)
		callback_();
}
}}

I3FileStager::I3FileStager()
{

}


I3FileStager::~I3FileStager()
{

}

bool
I3FileStager::CanStageIn(const std::string &url)
{
	std::vector<std::string> schemes(ReadSchemes());
	return std::find(schemes.begin(), schemes.end(),
	    url.substr(0, url.find(":"))) != schemes.end();
}

bool
I3FileStager::CanStageOut(const std::string &url)
{
	std::vector<std::string> schemes(WriteSchemes());
	return std::find(schemes.begin(), schemes.end(),
	    url.substr(0, url.find(":"))) != schemes.end();
}

std::string
I3FileStager::GetLocalFileName(const std::string &url, bool reading)
{
	std::map<std::string, std::string>::iterator fname = url_to_filename_.find(url);
	if (fname == url_to_filename_.end())
		fname = url_to_filename_.insert(std::make_pair(url, GenerateLocalFileName(url, reading))).first;
	return fname->second;
}

void
I3FileStager::WillReadLater(const std::string &url)
{
	if (CanStageIn(url))
		WillReadLater(url, GetLocalFileName(url, true));
}

namespace {
void strunlink(const std::string &s) { unlink(s.c_str()); }
}

I3::dataio::shared_filehandle
I3FileStager::GetReadablePath(const std::string &url)
{
	if (!CanStageIn(url))
		return I3::dataio::shared_filehandle(new I3::dataio::filehandle(url));
	std::string fname = GetLocalFileName(url, true);
	CopyFileIn(url, fname);
	boost::function<void()> deleter = boost::bind(&I3FileStager::Cleanup, shared_from_this(), url);
	return I3::dataio::shared_filehandle(new I3::dataio::filehandle(fname, deleter));
}

I3::dataio::shared_filehandle
I3FileStager::GetWriteablePath(const std::string &url)
{
	if (!CanStageOut(url))
		return I3::dataio::shared_filehandle(new I3::dataio::filehandle(url));
	std::string fname = GetLocalFileName(url, false);
	// Upload, then delete file when done
	boost::function<void()> deleter = boost::bind(&I3FileStager::StageFileOut, shared_from_this(), url);
	return I3::dataio::shared_filehandle(new I3::dataio::filehandle(fname, deleter));
}

void
I3FileStager::StageFileOut(const std::string &url)
{
	namespace fs = boost::filesystem;
	
	std::string fname = GetLocalFileName(url, false);
	if (fs::exists(fname)) {
		CopyFileOut(fname, url);
		unlink(fname.c_str());
	}
}

void
I3FileStager::Cleanup(const std::string &url)
{
	namespace fs = boost::filesystem;
	
	std::string fname = GetLocalFileName(url, true);
	if (fs::exists(fname))
		unlink(fname.c_str());
}

I3TrivialFileStager::I3TrivialFileStager()
{
}

boost::shared_ptr<I3FileStager>
I3TrivialFileStager::create()
{
	return boost::shared_ptr<I3TrivialFileStager>(new I3TrivialFileStager);
}

I3TrivialFileStager::~I3TrivialFileStager()
{
}

std::vector<std::string>
I3TrivialFileStager::ReadSchemes() const
{
	return std::vector<std::string>();
}

std::vector<std::string>
I3TrivialFileStager::WriteSchemes() const
{
	return std::vector<std::string>();
}

std::string
I3TrivialFileStager::GenerateLocalFileName(const std::string &url, bool reading)
{
	return url;
}

void
I3TrivialFileStager::WillReadLater(const std::string &url, const std::string &fname)
{
}

void
I3TrivialFileStager::CopyFileIn(const std::string &url, const std::string &fname)
{
}

void
I3TrivialFileStager::CopyFileOut(const std::string &fname, const std::string &url)
{
}

I3FileStagerCollection::I3FileStagerCollection(const std::vector<I3FileStagerPtr> &stagers)
	: stagers_(stagers)
{
	BOOST_FOREACH(const I3FileStagerPtr &p, stagers_) {
		if (p == NULL)
			log_fatal("Unconfigured I3FileStagerPtr!");
		BOOST_FOREACH(const std::string &scheme, p->ReadSchemes()) {
			if (readers_.find(scheme) == readers_.end())
				readers_[scheme] = p;
		}
		BOOST_FOREACH(const std::string &scheme, p->WriteSchemes()) {
			if (writers_.find(scheme) == writers_.end())
				writers_[scheme] = p;
		}
	}
}

boost::shared_ptr<I3FileStager>
I3FileStagerCollection::create(const std::vector<I3FileStagerPtr> &stagers)
{
	return boost::shared_ptr<I3FileStagerCollection>(new I3FileStagerCollection(stagers));
}

I3FileStagerCollection::~I3FileStagerCollection()
{

}

std::vector<std::string>
I3FileStagerCollection::ReadSchemes() const
{
	std::vector<std::string> schemes;
	BOOST_FOREACH(const protocol_map_t::value_type &pair, readers_)
		schemes.push_back(pair.first);
	return schemes;
}

std::vector<std::string>
I3FileStagerCollection::WriteSchemes() const
{
	std::vector<std::string> schemes;
	BOOST_FOREACH(const protocol_map_t::value_type &pair, writers_)
		schemes.push_back(pair.first);
	return schemes;
}

const I3FileStagerPtr&
I3FileStagerCollection::GetReader(const std::string &url) const
{
	std::string scheme = url.substr(0, url.find(":"));
	protocol_map_t::const_iterator target = readers_.find(scheme);
	if (target == readers_.end())
		log_fatal("No stager found for %s:// URLs", scheme.c_str());
	return target->second;
}

const I3FileStagerPtr&
I3FileStagerCollection::GetWriter(const std::string &url) const
{
	std::string scheme = url.substr(0, url.find(":"));
	protocol_map_t::const_iterator target = writers_.find(scheme);
	if (target == writers_.end())
		log_fatal("No stager found for %s:// URLs", scheme.c_str());
	return target->second;
}

std::string
I3FileStagerCollection::GenerateLocalFileName(const std::string &url, bool reading)
{
	return (reading ? GetReader(url) : GetWriter(url))->GenerateLocalFileName(url, reading);
}

void
I3FileStagerCollection::WillReadLater(const std::string &url, const std::string &fname)
{
	GetReader(url)->WillReadLater(url, fname);
}

void
I3FileStagerCollection::CopyFileIn(const std::string &url, const std::string &fname)
{
	return GetReader(url)->CopyFileIn(url, fname);
}

void
I3FileStagerCollection::CopyFileOut(const std::string &fname, const std::string &url)
{
	return GetWriter(url)->CopyFileOut(fname, url);
}
