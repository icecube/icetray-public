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
	 
#ifndef DATACLASSES_I3SUPERDSTTRIGGER_H_INCLUDED
#define DATACLASSES_I3SUPERDSTTRIGGER_H_INCLUDED	 

#include "icetray/I3FrameObject.h"
#include "icetray/I3Logging.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"

class I3Trigger;
class TriggerKey;
class I3DetectorStatus;
class I3SuperDSTTriggerSeries;

class I3SuperDSTTrigger {
private:
	uint32_t key_idx_;
	uint32_t startcode_, lengthcode_;
	
	SET_LOGGER("I3SuperDST");
	
	friend class I3SuperDSTTriggerSeries;
	
public:
	I3SuperDSTTrigger(const I3Trigger&, const I3DetectorStatus&);
	I3SuperDSTTrigger(uint32_t key_idx, uint32_t startcode, uint32_t lengthcode);
	
	const TriggerKey& GetTriggerKey(const I3DetectorStatus&) const;
	double GetTime() const;
	double GetLength() const;
	
	bool operator<(const I3SuperDSTTrigger &) const;
	bool operator==(const I3SuperDSTTrigger &) const;
	
	void SetTimeReference(const I3SuperDSTTrigger &);
};

class I3SuperDSTTriggerSeries : public std::list<I3SuperDSTTrigger>, public I3FrameObject {
public:
	I3SuperDSTTriggerSeries() {};
	I3SuperDSTTriggerSeries(const I3TriggerHierarchy &, const I3DetectorStatus&);
	
	template <typename Iterator>
	I3SuperDSTTriggerSeries(Iterator first, Iterator last) : std::list<I3SuperDSTTrigger>(first, last) {}
	
	boost::shared_ptr<I3TriggerHierarchy> Unpack(const I3DetectorStatus&) const;

private:
	SET_LOGGER("I3SuperDST");
	
	mutable boost::shared_ptr<I3TriggerHierarchy> unpacked_;
	
	friend class icecube::serialization::access;

	template <class Archive> void save(Archive & ar, unsigned version) const;
	template <class Archive> void load(Archive & ar, unsigned version);
	I3_SERIALIZATION_SPLIT_MEMBER();
};

I3_POINTER_TYPEDEFS(I3SuperDSTTriggerSeries);

#endif
