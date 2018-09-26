#include <I3Test.h>


#include "dataclasses/I3MapOMKeyMask.h"
#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/I3Double.h"
#include "boost/make_shared.hpp"
#include "boost/foreach.hpp"
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/interprocess/streams/bufferstream.hpp"
#include "boost/interprocess/streams/vectorstream.hpp"

TEST_GROUP(I3MapMask);

static I3RecoPulseSeriesMapPtr
manufacture_pulsemap()
{
	I3RecoPulseSeriesMapPtr pulsemap = boost::make_shared<I3RecoPulseSeriesMap>();
	I3RecoPulseSeries pulses;
	I3RecoPulse p;
	p.SetTime(1.0);
	p.SetWidth(2.0);
	p.SetCharge(3.141);
	pulses.push_back(p);
	
	p.SetTime(2.0); pulses.push_back(p);
	p.SetTime(12); pulses.push_back(p);
	p.SetTime(13); pulses.push_back(p);
	p.SetTime(14); pulses.push_back(p);
	p.SetTime(15); pulses.push_back(p);
	p.SetTime(16); pulses.push_back(p);
	p.SetTime(17); pulses.push_back(p);
	p.SetTime(18); pulses.push_back(p);
	
	
	
	pulsemap->operator[](OMKey(42, 42)) = pulses;
	
	pulsemap->operator[](OMKey(67, 12)) = pulses;
	
	return pulsemap;
}

static I3FramePtr
manufacture_frame(const std::string &name)
{
	I3FramePtr frame(new I3Frame(I3Frame::Physics));
	I3RecoPulseSeriesMapConstPtr pulses = manufacture_pulsemap();
	frame->Put(name, pulses);
	
	return frame;
}

static I3FramePtr
resurrect(I3FramePtr frame)
{
	namespace ip = boost::interprocess;
	ip::basic_vectorstream<std::vector<char> > archive_stream;
	frame->save(archive_stream);

	archive_stream.reserve(0);
	
	I3FramePtr newframe(new I3Frame);
	newframe->load(archive_stream);

	return newframe;
}

TEST(Compactify)
{
	I3FramePtr frame = manufacture_frame("foo");
	I3RecoPulseSeriesMapConstPtr pulses =
	    frame->Get<I3RecoPulseSeriesMapConstPtr>("foo");
	I3RecoPulseSeriesMapMaskPtr mask(new I3RecoPulseSeriesMapMask(*frame, "foo"));
	
	BOOST_FOREACH(const I3RecoPulseSeriesMap::value_type &pair, *pulses)
		mask->Set(pair.first, false);	
	frame->Put("mask", mask);
	
	I3FramePtr zombieframe = resurrect(frame);
	pulses = zombieframe->Get<I3RecoPulseSeriesMapConstPtr>("mask");
	ENSURE_EQUAL(pulses->size(), 0u);
}

TEST(Apply)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask(frame, "foo");
	
	ENSURE_EQUAL(mask.GetSum(), 18u);
	
	masked = mask.Apply(frame);
	
	ENSURE_EQUAL(pulses->size(), masked->size());
	ENSURE_EQUAL(masked->begin()->second.size(), 9u);
	
	/* Ensure that original and masked pulse series maps are identical. */
	I3RecoPulseSeriesMap::const_iterator mit1, mit2;
	mit1 = pulses->begin();
	mit2 = masked->begin();
	for ( ; mit1 != pulses->end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		I3RecoPulseSeries::const_iterator vit1, vit2;
		vit1 = mit1->second.begin();
		vit2 = mit2->second.begin();
		for ( ; vit1 != mit1->second.end(); vit1++, vit2++)
			ENSURE(*vit1 == *vit2);
	}
}

TEST(Repoint)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMaskPtr mask =
	    boost::make_shared<I3RecoPulseSeriesMapMask>(frame, "foo");
	mask->Set(pulses->begin()->first, false);
	frame.Put("foomask", mask);
	
	// Make a derived mask
	I3RecoPulseSeriesMapMaskPtr submask = boost::make_shared<I3RecoPulseSeriesMapMask>(frame, "foomask");
	submask->Set(boost::next(pulses->begin())->first, 3, false);
	frame.Put("submask", submask);
	ENSURE(submask->HasAncestor(frame, "foomask"), "Intermediate mask is in the chain");
	ENSURE(submask->HasAncestor(frame, "foo"), "Original pulsemap is in the chain");
	
	// Collapse the derived mask back on to the original source
	I3RecoPulseSeriesMapMaskPtr collapsed = submask->Repoint(frame, mask->GetSource());
	frame.Put("collapsed", collapsed);
	ENSURE(!collapsed->HasAncestor(frame, "foomask"), "Intermediate mask is no longer in the chain");
	ENSURE(collapsed->HasAncestor(frame, "foo"), "Original pulsemap is still in the chain");
	
	ENSURE_EQUAL(mask->GetSource(), collapsed->GetSource());
	ENSURE_EQUAL(submask->GetSum(), collapsed->GetSum());
	ENSURE(submask->GetSum() < mask->GetSum());
	
	// Now, use the collapsed mask to remove an element from the source mask
	I3RecoPulseSeriesMapMask stripped = mask->Remove(*collapsed);
	ENSURE_EQUAL(stripped.GetSum(), 1u);
}

TEST(ApplyAutomagically)
{
	I3RecoPulseSeriesMapPtr pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMaskPtr mask = 
	    boost::make_shared<I3RecoPulseSeriesMapMask>(frame, "foo");
	
	frame.Put("foomask", mask);
	
	/* Apply the mask behind the scenes. */
	I3RecoPulseSeriesMapConstPtr masked =
	    frame.Get<I3RecoPulseSeriesMapConstPtr>("foomask");
	
	ENSURE((bool)masked, "Mask was applied by I3Frame");
	ENSURE_EQUAL(pulses->size(), masked->size());
	ENSURE_EQUAL(masked->begin()->second.size(), 9u);
	
	/* Ensure that original and masked pulse series maps are identical. */
	I3RecoPulseSeriesMap::const_iterator mit1, mit2;
	mit1 = pulses->begin();
	mit2 = masked->begin();
	for ( ; mit1 != pulses->end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		I3RecoPulseSeries::const_iterator vit1, vit2;
		vit1 = mit1->second.begin();
		vit2 = mit2->second.begin();
		for ( ; vit1 != mit1->second.end(); vit1++, vit2++)
			ENSURE(*vit1 == *vit2);
	}
	
	/*
	 * Now, ask for a reference. This will blow up unless the mask
	 * manages the lifetime of the pulse series returned by Apply().
	 */
	const I3RecoPulseSeriesMap &masked_ref =
	    frame.Get<I3RecoPulseSeriesMap>("foomask");
	
	ENSURE_EQUAL(pulses->size(), masked_ref.size());
	ENSURE_EQUAL(masked_ref.begin()->second.size(), 9u);
	
	/* Ensure that original and masked pulse series maps are identical. */
	mit1 = pulses->begin();
	mit2 = masked_ref.begin();
	for ( ; mit1 != pulses->end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		I3RecoPulseSeries::const_iterator vit1, vit2;
		vit1 = mit1->second.begin();
		vit2 = mit2->second.begin();
		for ( ; vit1 != mit1->second.end(); vit1++, vit2++)
			ENSURE(*vit1 == *vit2);
	}
	
	/* Now, make sure it fails cleanly */
	I3DoublePtr dub = boost::make_shared<I3Double>();
	frame.Put("dub", dub);
	
	masked = frame.Get<I3RecoPulseSeriesMapConstPtr>("dub");
	ENSURE(!masked, "I3Frame::Get() returns a null pointer");
	
	masked = frame.Get<I3RecoPulseSeriesMapConstPtr>("nonexistant");
	ENSURE(!masked, "I3Frame::Get() returns a null pointer");

	/* Now, ensure that nothing funny happens when we get it as a FrameObject */
	I3FrameObjectConstPtr fop = frame.Get<I3FrameObjectConstPtr>("foomask");
}

static bool
Is42(const OMKey &key, unsigned idx, const I3RecoPulse &p)
{
	return key.GetString() == 42;
}

TEST(Constructors)
{
	I3RecoPulseSeriesMapPtr pulses = manufacture_pulsemap();
	I3Frame frame;
	frame.Put("foo", pulses);
	
	// Default constructor
	{
		I3RecoPulseSeriesMapMask mask;
		ENSURE_EQUAL(mask.GetSource(), "");
	}
	
	// Simple constructor
	{
		I3RecoPulseSeriesMapMask mask(frame, "foo");
		ENSURE_EQUAL(mask.GetSource(), "foo");
	}
	
	// Subset constructor
	{
		I3RecoPulseSeriesMap subset(*pulses);
		I3RecoPulseSeriesMapMask mask(frame, "foo", subset);
		ENSURE_EQUAL(mask.GetSource(), "foo");
	}
	
	// Functional constructor
	{
		I3RecoPulseSeriesMapMask mask(frame, "foo", Is42);
		ENSURE_EQUAL(mask.GetSource(), "foo");
	}
}

TEST(UnsetAllForOM)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMap::const_iterator mit1, mit2;
	I3RecoPulseSeriesMapConstPtr masked;
	I3RecoPulseSeriesMapMask mask;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);

	I3RecoPulseSeriesMap subpulses(*pulses);
	subpulses.erase(pulses->begin()->first);

	/* First, try unsetting every pulse in the DOM in one go*/
	mask = I3RecoPulseSeriesMapMask(frame, "foo");
	mask.Set(pulses->begin()->first, false);
	masked = mask.Apply(frame);

	/* Ensure that original and masked pulse series maps are identical. */
	mit1 = subpulses.begin();
	mit2 = masked->begin();
	for ( ; mit1 != subpulses.end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		I3RecoPulseSeries::const_iterator vit1, vit2;
		vit1 = mit1->second.begin();
		vit2 = mit2->second.begin();
		for ( ; vit1 != mit1->second.end(); vit1++, vit2++)
			ENSURE(*vit1 == *vit2);
	}

	/* Now, try unsetting every pulse one by one. This must be equivalent! */
	mask = I3RecoPulseSeriesMapMask(frame, "foo");
	for (unsigned i = 0; i < pulses->begin()->second.size(); i++)
		mask.Set(pulses->begin()->first, i, false);
	masked = mask.Apply(frame);

	/* Ensure that original and masked pulse series maps are identical. */
	mit1 = subpulses.begin();
	mit2 = masked->begin();
	for ( ; mit1 != subpulses.end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		I3RecoPulseSeries::const_iterator vit1, vit2;
		vit1 = mit1->second.begin();
		vit2 = mit2->second.begin();
		for ( ; vit1 != mit1->second.end(); vit1++, vit2++)
			ENSURE(*vit1 == *vit2);
	}
}
	
TEST(SetSinglePulse)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask(frame, "foo");
	
	/* Unset a pulse. */
	mask.Set(pulses->begin()->first, pulses->begin()->second[3], false);
	
	masked = mask.Apply(frame);
	ENSURE_EQUAL(mask.GetSum(), 17u);
	ENSURE_EQUAL(masked->begin()->second.size(), 8u);
	
	/* Re-set it, and make sure it comes back. */
	mask.Set(pulses->begin()->first, pulses->begin()->second[3], true);
	
	masked = mask.Apply(frame);
	ENSURE_EQUAL(mask.GetSum(), 18u);
	ENSURE_EQUAL(masked->begin()->second.size(), 9u);
	
	/* Unset a pulse, this time by index. */
	mask.Set(pulses->begin()->first, 3, false);
	
	masked = mask.Apply(frame);
	ENSURE_EQUAL(mask.GetSum(), 17u);
	ENSURE_EQUAL(masked->begin()->second.size(), 8u);
	
	/* Re-set it, and make sure it comes back. */
	mask.Set(pulses->begin()->first, 3, true);
	
	masked = mask.Apply(frame);
	ENSURE_EQUAL(mask.GetSum(), 18u);
	ENSURE_EQUAL(masked->begin()->second.size(), 9u);
}

TEST(NullSubset)
{
	/*
	 * Test a few stupid edge cases.
	 */
	I3FramePtr frame = manufacture_frame("foo");
	I3RecoPulseSeriesMapPtr nully(new I3RecoPulseSeriesMap);
	I3RecoPulseSeriesMapMaskPtr mask(new I3RecoPulseSeriesMapMask(*frame, "foo", *nully));
	
	I3RecoPulseSeriesMapConstPtr masked = mask->Apply(*frame);
	
	ENSURE_EQUAL(masked->size(), 0u, "Null subset of non-empty superset == null set");
	
	frame->Delete("foo");
	frame->Put("foo", nully);
	
	mask = boost::make_shared<I3RecoPulseSeriesMapMask>(*frame, "foo", *nully);
	masked = mask->Apply(*frame);
	
	ENSURE_EQUAL(masked->size(), 0u, "Null subset of null set == null set");
}

TEST(SetSinglePulseFromBlankMask)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask(frame, "foo");
	
	mask.SetNone();
	
	/* Re-set every other pulse, and make sure it comes back. */
	unsigned total = 0;
	BOOST_FOREACH(const I3RecoPulseSeriesMap::value_type &pair, *pulses) {
		unsigned domcount = 0;
		bool skip = false;
		BOOST_FOREACH(const I3RecoPulse &pulse, pair.second) {
			if (skip) {
				skip = false;
			} else {
				mask.Set(pair.first, pulse, true);
				masked = mask.Apply(frame);
				total++;
				domcount++;
				skip = true;
			}
			ENSURE_EQUAL(mask.GetSum(), total);
			ENSURE_EQUAL(masked->find(pair.first)->second.size(), domcount);
		}
	}
}


TEST(SetOM)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask(frame, "foo");
	
	/* Unset an entire OM. */
	mask.Set(pulses->begin()->first, false);
	
	masked = mask.Apply(frame);
	ENSURE_EQUAL(mask.GetSum(), 9u);
	ENSURE_EQUAL(masked->size(), 1u);
	
	/* We dropped the right OM. */
	ENSURE(pulses->begin()->first != masked->begin()->first);
	
	/* Bring it back. */
	mask.Set(pulses->begin()->first, true);
	
	masked = mask.Apply(frame);
	ENSURE_EQUAL(mask.GetSum(), 18u);
	ENSURE_EQUAL(masked->size(), 2u);
	
	/* The masked version is identical again. */
	ENSURE(pulses->begin()->first == masked->begin()->first);
}

TEST(ConstructAsDiff)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	I3RecoPulseSeriesMap::const_iterator mit1, mit2;
	I3RecoPulseSeriesMapMask mask;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	
	I3RecoPulseSeriesMap subset(*pulses);
	
	
	mask = I3RecoPulseSeriesMapMask(frame, "foo", subset);
	
	
	
	/* Ensure that subset and masked pulse series maps are identical. */
	masked = mask.Apply(frame);
	mit1 = subset.begin();
	mit2 = masked->begin();
	for ( ; mit1 != subset.end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		I3RecoPulseSeries::const_iterator vit1, vit2;
		vit1 = mit1->second.begin();
		vit2 = mit2->second.begin();
		for ( ; vit1 != mit1->second.end(); vit1++, vit2++)
			ENSURE(*vit1 == *vit2);
	}
	
	/* Now, actually make a subset. */
	subset.erase(--subset.end());
	mask = I3RecoPulseSeriesMapMask(frame, "foo", subset);
	ENSURE_EQUAL(mask.GetSum(), 9u);
	
	/* Ensure that subset and masked pulse series maps are identical. */
	masked = mask.Apply(frame);
	mit1 = subset.begin();
	mit2 = masked->begin();
	for ( ; mit1 != subset.end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		I3RecoPulseSeries::const_iterator vit1, vit2;
		vit1 = mit1->second.begin();
		vit2 = mit2->second.begin();
		for ( ; vit1 != mit1->second.end(); vit1++, vit2++)
			ENSURE(*vit1 == *vit2);
	}
	
	/* Erase something in the middle. */
	subset = I3RecoPulseSeriesMap(*pulses);
	subset.begin()->second.erase(subset.begin()->second.begin()+5);
	mask = I3RecoPulseSeriesMapMask(frame, "foo", subset);
	ENSURE_EQUAL(mask.GetSum(), 17u);
	
	/* Ensure that subset and masked pulse series maps are identical. */
	masked = mask.Apply(frame);
	mit1 = subset.begin();
	mit2 = masked->begin();
	for ( ; mit1 != subset.end(); mit1++, mit2++) {
		ENSURE_EQUAL(mit1->first, mit2->first);
		ENSURE_EQUAL(mit1->second.size(), mit2->second.size());
		I3RecoPulseSeries::const_iterator vit1, vit2;
		vit1 = mit1->second.begin();
		vit2 = mit2->second.begin();
		for ( ; vit1 != mit1->second.end(); vit1++, vit2++)
			ENSURE(*vit1 == *vit2);
	}
	
}

TEST(Clear)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask(frame, "foo");
	
	mask.SetNone();
	
	ENSURE_EQUAL(mask.GetSum(), 0u);
	
	masked = mask.Apply(frame);
	
	ENSURE_EQUAL(masked->size(), 0u);
}

TEST(And)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask_1(frame, "foo");
	I3RecoPulseSeriesMapMask mask_2(frame, "foo");
	I3RecoPulseSeriesMapMask mask_3 = mask_1 & mask_2;
	
	ENSURE_EQUAL(mask_1.GetSum(), 18u);
	ENSURE_EQUAL(mask_2.GetSum(), 18u);
	ENSURE_EQUAL(mask_3.GetSum(), 18u);
	
	I3RecoPulseSeriesMap::const_iterator pit = pulses->begin();
	
	mask_1.Set(pit->first, false);
	
	mask_3 = mask_1 & mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 9u);
	
	pit++;
	mask_2.Set(pit->first, false);
	mask_3 = mask_1 & mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 0u);
}

TEST(Or)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask_1(frame, "foo");
	I3RecoPulseSeriesMapMask mask_2(frame, "foo");
	I3RecoPulseSeriesMapMask mask_3 = mask_1 | mask_2;
	
	ENSURE_EQUAL(mask_1.GetSum(), 18u);
	ENSURE_EQUAL(mask_2.GetSum(), 18u);
	ENSURE_EQUAL(mask_3.GetSum(), 18u);
	
	I3RecoPulseSeriesMap::const_iterator pit = pulses->begin();
	
	mask_1.Set(pit->first, false);
	
	mask_3 = mask_1 | mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 18u);
	
	mask_2.Set(pit->first, false);
	
	mask_3 = mask_1 | mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 9u);
	
	pit++;
	
	mask_1.Set(pit->first, false);
	
	mask_3 = mask_1 | mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 9u);
	
	mask_2.Set(pit->first, false);
	
	mask_3 = mask_1 | mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 0u);
}

TEST(XOr)
{
	I3RecoPulseSeriesMapPtr pulses;
	I3RecoPulseSeriesMapConstPtr masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask_1(frame, "foo");
	I3RecoPulseSeriesMapMask mask_2(frame, "foo");
	I3RecoPulseSeriesMapMask mask_3 = mask_1 ^ mask_2;
	
	ENSURE_EQUAL(mask_1.GetSum(), 18u);
	ENSURE_EQUAL(mask_2.GetSum(), 18u);
	ENSURE_EQUAL(mask_3.GetSum(), 0u);
	
	I3RecoPulseSeriesMap::const_iterator pit = pulses->begin();
	
	mask_1.Set(pit->first, false);
	
	mask_3 = mask_1 ^ mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 9u);
	
	mask_2.Set(pit->first, false);
	
	mask_3 = mask_1 ^ mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 0u);
	
	
	pit++;
	
	mask_1.Set(pit->first, false);
	
	mask_3 = mask_1 ^ mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 9u);
	
	mask_2.Set(pit->first, false);
	
	mask_3 = mask_1 ^ mask_2;
	ENSURE_EQUAL(mask_3.GetSum(), 0u);
}

#define ROUND_UP(num, denom) (num % denom == 0) ? num/denom : (num/denom) + 1

#if 0
//T//EST(Serialization)
{
	I3RecoPulseSeriesMapPtr pulses, masked;
	pulses = manufacture_pulsemap();
		
	I3Frame frame;
	frame.Put("foo", pulses);
	I3RecoPulseSeriesMapMask mask(frame, "foo");
	// I3RecoPulseSeriesMapMask mask;
	
	
	std::ostringstream oarchive_stream;
	/* This is the archive type instantiated by I3_SERIALIZABLE */
	icecube::archive::portable_binary_oarchive oarchive(oarchive_stream);
	oarchive << mask;
	
	ENSURE_EQUAL( oarchive_stream.str().size(),
	    unsigned(12 + 2 + 1 + 2*2 + 1*(2)),
	    "Serialized size in bytes: 12 (I3FrameObject overhead) + 2 (OMKey bitmask overhead) "
	    "+ 1*(nchannel/8) (OMKey bitmask) + 2*(nchannel_selected) (vector bitmask overhead) "
	    "+ 1*(npulses/dom/8)");

	std::istringstream iarchive_stream(oarchive_stream.str());
	icecube::archive::portable_binary_iarchive iarchive(iarchive_stream);
	I3RecoPulseSeriesMapMask maska;
	iarchive >> maska;
}
#endif
