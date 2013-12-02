#include "dataio/I3FileStager.h"
#include <boost/foreach.hpp>

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

void
I3FileStagerCollection::WillReadLater(const std::string &url)
{
	GetReader(url)->WillReadLater(url); 
}

std::string
I3FileStagerCollection::StageFileIn(const std::string &url)
{
	return GetReader(url)->StageFileIn(url);
}

std::string
I3FileStagerCollection::WillWrite(const std::string &url)
{
	return GetWriter(url)->WillWrite(url);
}

void
I3FileStagerCollection::StageFileOut(const std::string &url)
{
	GetWriter(url)->StageFileOut(url);
}

void
I3FileStagerCollection::Cleanup(const std::string &filename)
{
	BOOST_FOREACH(const I3FileStagerPtr &p, stagers_) {
		try {
			p->Cleanup(filename);
		} catch (...) {
			
		}
	}
}

