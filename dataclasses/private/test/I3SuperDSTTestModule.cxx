/**
 * @file 
 * @brief 
 *
 * (c) 2010 the IceCube Collaboration
 *
 * $Id$
 * @version $Revision$
 * @date $Date$
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 */

#include "icetray/I3Module.h"
#if 1
#include <I3Test.h>
#else
// I3Test.h requires symbols defined in I3TestMain.ixx, which we can't include here.
#define ENSURE_DISTANCE(left, right, distance, ...) \
	assert(abs(left - right) <= distance);
#define ENSURE_EQUAL(left, right, ...) \
	assert(left == right);
#endif

#include <boost/utility.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <archive/portable_binary_archive.hpp>
#include <limits>

#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/I3MapOMKeyUnion.h"
#include "dataclasses/payload/I3SuperDST.h"

class I3SuperDSTTestModule : public I3Module {
public:
	I3SuperDSTTestModule(const I3Context& ctx);
	void Configure();
	void DAQ(I3FramePtr frame);
private:
	unsigned superdst_version_;
	std::string pulses_name_;
	std::string superdst_name_;
	std::string fake_pulses_name_;
};

static const char* pulses_optionname = "Pulses";
static const char* superdst_optionname = "SuperDST";

I3_MODULE(I3SuperDSTTestModule);

I3SuperDSTTestModule::I3SuperDSTTestModule(const I3Context& ctx) : I3Module(ctx)
{
	AddOutBox("OutBox");

	pulses_name_ = "HLCPulses";
	AddParameter(pulses_optionname, "Name of the "
	    "I3RecoPulseSeriesMap in the frame",
	    pulses_name_);
	
	superdst_name_ = "";
	AddParameter(superdst_optionname, "Name of the "
	    "I3SuperDST object to get from the frame",
	     superdst_name_);

	superdst_version_ = i3superdst_version_;
	AddParameter("Version", "Version of SuperDST encoding to use", superdst_version_);
}

void
I3SuperDSTTestModule::Configure()
{
	GetParameter(pulses_optionname, pulses_name_);
	GetParameter(superdst_optionname, superdst_name_);
	GetParameter("Version", superdst_version_);
}

void
I3SuperDSTTestModule::DAQ(I3FramePtr frame)
{
	
	I3RecoPulseSeriesMapConstPtr pulses, fake_pulses;
	I3RecoPulseSeriesMapPtr clean_pulses;
	I3RecoPulseSeriesMap::const_iterator mit1, mit2;
	I3RecoPulseSeries::const_iterator pit1, pit2;
	I3SuperDSTConstPtr superdst;

	pulses = frame->Get<I3RecoPulseSeriesMapConstPtr>(pulses_name_);
	
	if (!pulses) {
		goto bail;
	}
	if (superdst_name_.size() > 0)
		superdst = frame->Get<I3SuperDSTConstPtr>(superdst_name_);
	else {
		/* Send the whole shebang on a round-trip through serialzation */
		superdst = I3SuperDSTPtr(new I3SuperDST(*pulses));
		std::ostringstream oarchive_stream;
		icecube::archive::portable_binary_oarchive oarchive(oarchive_stream);
		oarchive << (*superdst);
		superdst.reset();

		std::istringstream iarchive_stream(oarchive_stream.str());
		icecube::archive::portable_binary_iarchive iarchive(iarchive_stream);
		/* NB: I know this is dangerous. */
		I3SuperDST *sdst = new I3SuperDST;
		superdst = I3SuperDSTPtr(sdst);
		iarchive >> *sdst;
	}
	
	fake_pulses = superdst->Unpack();

	/* Remove zero-length vectors from the map, as sDST can't represent them. */
	clean_pulses = I3RecoPulseSeriesMapPtr(new I3RecoPulseSeriesMap(*pulses));
	pulses.reset();
	{
		I3RecoPulseSeriesMap::iterator mit = clean_pulses->begin();
		for ( ; mit != clean_pulses->end(); ) {
			if (mit->second.size() == 0)
				clean_pulses->erase(mit++);
			else
				mit++;
		}
	}

	pulses = clean_pulses;
	
	//ENSURE_EQUAL(pulses->size(), fake_pulses->size());
	BOOST_FOREACH(const I3RecoPulseSeriesMap::value_type &pair, *pulses) {
		const OMKey &key = pair.first;
		assert( fake_pulses->find(key) != fake_pulses->end() );
	}

	using namespace I3SuperDSTUtils;
	
	for (mit1 = pulses->begin(), mit2 = fake_pulses->begin();
	    mit1 != pulses->end() && mit2 != fake_pulses->end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		Discretization format = (mit1->first.GetOM() > 60) ? LOG : LINEAR;
		if (superdst_version_ < 1)
			format = LINEAR;
		for (pit1 = mit1->second.begin(), pit2 = mit2->second.begin();
		    pit1 != mit1->second.end() && pit2 != mit1->second.end(); pit1++, pit2++) {
			double epsilon;
			ENSURE_DISTANCE(pit1->GetTime(), pit2->GetTime(),
			    std::numeric_limits<float>::epsilon()
			    + I3SuperDST::DecodeTime(1, superdst_version_)/2.0, "Leading-edge "
			    "times agree to within discretization error");
			if (superdst_version_ > 0) {
				uint32_t w = I3SuperDST::EncodeWidth(pit1->GetWidth(),
				    31, superdst_version_);
				epsilon = I3SuperDST::DecodeWidth(w+1, superdst_version_)
				    - I3SuperDST::DecodeWidth(w, superdst_version_);
				/* 
				 * Stupid edge case: decoded pulses are not allowed to overlap,
				 * so we only check the encoding if the pulses don't touch.
				 */
				if (!(pit2+1 != mit2->second.end() &&
				    fabs(pit2->GetTime()+pit2->GetWidth() - (pit2+1)->GetTime()) < 0.1))
					ENSURE_DISTANCE(pit1->GetWidth(), pit2->GetWidth(), epsilon/2.0);
			}
			double charge = std::max(pit1->GetCharge(), 0.0f);
			uint32_t l = I3SuperDST::EncodeCharge(charge,
			    31, superdst_version_, format);
			epsilon = I3SuperDST::DecodeCharge(l+1, superdst_version_, format)
				    - I3SuperDST::DecodeCharge(l, superdst_version_, format);
			/* Account for rounding error near 0 */
			epsilon *= (1.+std::numeric_limits<float>::epsilon())/2.;
			ENSURE_DISTANCE(charge, pit2->GetCharge(),
			    epsilon, "Charges agree to within discretization");
		}
	}

bail:
	PushFrame(frame, "OutBox");
}


class UnionMan : public I3Module {
public:
	UnionMan(const I3Context &ctx) : I3Module(ctx)
	{
		AddParameter("Keys", "Keys from which to create a unified pulse map", keys_);
		AddOutBox("OutBox");
	}

	void Configure()
	{
		GetParameter("Keys", keys_);
	}

	void DAQ(I3FramePtr frame)
	{
		std::vector<std::string> keys;
		BOOST_FOREACH(const std::string &key, keys_)
			if (frame->Has(key))
				keys.push_back(key);
		I3RecoPulseSeriesMapUnionPtr uni =
		    boost::make_shared<I3RecoPulseSeriesMapUnion>(*frame, keys);
		frame->Put(GetName(), uni);
		PushFrame(frame);
	}
private:
	std::vector<std::string> keys_;
};

I3_MODULE(UnionMan);


