/**
 * @file 
 * @brief 
 *
 * (c) 2011 Jakob van Santen <vansanten@wisc.edu>
 *          and the IceCube Collaboration <icecube.wisc.edu>
 *
 * $Id$
 * @version $Revision$
 * @date $Date$
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 */

#include "dataclasses/payload/I3SuperDST.h"
#include "dataclasses/payload/I3SuperDSTUtils.h"
#include "dataclasses/payload/I3SuperDSTTrigger.h"
#include "dataclasses/status/I3DetectorStatus.h"
#include "dataclasses/physics/I3Trigger.h"

#include <boost/foreach.hpp>
#include <boost/utility.hpp>

inline uint32_t FindIndex(const I3TriggerStatusMap &triggers, const TriggerKey &key)
{
	I3TriggerStatusMap::const_iterator it = triggers.find(key);
	return (std::distance(triggers.begin(), it));
}

inline const TriggerKey& FindKey(const I3TriggerStatusMap &triggers, uint32_t idx)
{
	I3TriggerStatusMap::const_iterator it = triggers.begin();
	std::advance(it, idx);
	return it->first;
}

I3SuperDSTTrigger::I3SuperDSTTrigger(const I3Trigger &trigger,
    const I3DetectorStatus &status)
{
	key_idx_ = FindIndex(status.triggerStatus, trigger.GetTriggerKey());
	if (key_idx_ >= status.triggerStatus.size())
		throw std::range_error("Can't find trigger config in DetectorStatus!");
	
	startcode_  = I3SuperDST::EncodeTime(trigger.GetTriggerTime(), 31, i3superdst_version_);
	lengthcode_ = I3SuperDST::EncodeTime(trigger.GetTriggerLength(), 31, i3superdst_version_);
}

I3SuperDSTTrigger::I3SuperDSTTrigger(uint32_t key_idx, uint32_t startcode,
    uint32_t lengthcode) : key_idx_(key_idx), startcode_(startcode),
    lengthcode_(lengthcode)
{}

bool
I3SuperDSTTrigger::operator<(const I3SuperDSTTrigger &other) const
{
	return (startcode_ < other.startcode_);
}

bool
I3SuperDSTTrigger::operator==(const I3SuperDSTTrigger &rhs) const
{
	return (key_idx_ == rhs.key_idx_) &&
	       (startcode_ == rhs.startcode_) &&
	       (lengthcode_ == rhs.lengthcode_);
}

void
I3SuperDSTTrigger::SetTimeReference(const I3SuperDSTTrigger &other)
{
	assert(startcode_ >= other.startcode_);
	startcode_ -= other.startcode_;
}

double
I3SuperDSTTrigger::GetTime() const
{
	return I3SuperDST::DecodeTime(startcode_);
}

double
I3SuperDSTTrigger::GetLength() const
{
	return I3SuperDST::DecodeTime(lengthcode_);
}

const TriggerKey&
I3SuperDSTTrigger::GetTriggerKey(const I3DetectorStatus &status) const
{
	if (key_idx_ >= status.triggerStatus.size())
		throw std::range_error("Can't find trigger config in DetectorStatus!");
	return FindKey(status.triggerStatus, key_idx_);
}

inline bool ExpectNoConfig(const TriggerKey &key)
{
	// The types listed here are window dressing that can
	// be reproduced offline, given the "real" triggers
	switch (key.GetType()) {
		case TriggerKey::THROUGHPUT:
		case TriggerKey::MERGED:
		case TriggerKey::TWO_COINCIDENCE:
		case TriggerKey::THREE_COINCIDENCE:
			return true;
		default:
			return false;
	}
}

I3SuperDSTTriggerSeries::I3SuperDSTTriggerSeries(
    const I3TriggerHierarchy &triggers, const I3DetectorStatus &status)
{
	I3TriggerHierarchy::iterator trigger = triggers.begin();
	for ( ; trigger != triggers.end(); trigger++) {
		if (trigger->GetTriggerKey().CheckConfigID())
			this->push_back(I3SuperDSTTrigger(*trigger, status));
		else if (!ExpectNoConfig(trigger->GetTriggerKey()))
			log_fatal("Got a %s without a config ID. WTF?",
			    trigger->GetTriggerKey().GetTypeString());
	}
	this->sort();
	I3SuperDSTTriggerSeries::reverse_iterator current = this->rbegin(),
	    prev = boost::next(current);
	for ( ; prev != this->rend(); current++, prev++)
		current->SetTimeReference(*prev);
}

boost::shared_ptr<I3TriggerHierarchy>
I3SuperDSTTriggerSeries::Unpack(const I3DetectorStatus &status) const
{
	if (unpacked_)
		return unpacked_;
	
	unpacked_ = boost::shared_ptr<I3TriggerHierarchy>(new I3TriggerHierarchy);
	I3TriggerHierarchy::iterator iter;
	double t_ref = 0.0;
	BOOST_FOREACH(const I3SuperDSTTrigger &strigger, *this) {
		I3Trigger trig;
		
		// Behold, cargo-cult encapsulation.
		trig.GetTriggerKey() = strigger.GetTriggerKey(status);
		trig.SetTriggerTime(strigger.GetTime() + t_ref);
		trig.SetTriggerLength(strigger.GetLength());
		trig.SetTriggerFired(true);
		
		t_ref += strigger.GetTime();

		unpacked_->insert(unpacked_->begin(), trig);
	}
	
	return unpacked_;
}

template <class Archive>
void
I3SuperDSTTriggerSeries::save(Archive & ar, unsigned version) const
{
	I3SuperDSTUtils::SizeCodec ntriggers(this->size());
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("NTriggers", ntriggers);
	BOOST_FOREACH(const I3SuperDSTTrigger &strigger, *this) {
		uint8_t tag = (strigger.key_idx_ & 0xf) |
		    ((strigger.lengthcode_ & 0xf) << 4);
		I3SuperDSTUtils::SizeCodec length(strigger.lengthcode_ >> 4);
		I3SuperDSTUtils::SizeCodec start(strigger.startcode_);
		ar & make_nvp("Tag", tag);
		ar & make_nvp("Length", length);
		ar & make_nvp("Time", start);
	}
}

template <class Archive>
void
I3SuperDSTTriggerSeries::load(Archive & ar, unsigned version)
{
	I3SuperDSTUtils::SizeCodec ntriggers;
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("NTriggers", ntriggers);
	for (unsigned i=0; i < ntriggers.value(); i++) {
		uint8_t tag(0);
		I3SuperDSTUtils::SizeCodec length, start;
		ar & make_nvp("Tag", tag);
		ar & make_nvp("Length", length);
		ar & make_nvp("Time", start);
		
		uint32_t key_idx = tag & 0xf;
		uint32_t lengthcode =
		    (length.value() << 4) | ((tag >> 4) & 0xf);
		this->push_back(I3SuperDSTTrigger(key_idx, start.value(),
		    lengthcode));
	}
}

I3_SPLIT_SERIALIZABLE(I3SuperDSTTriggerSeries);
