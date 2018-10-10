#include <I3Test.h>

#include <dataclasses/physics/I3RecoPulseSeriesMapCombineByModule.h>

TEST_GROUP(I3RecoPulseSeriesMapCombineByModule);

TEST(Apply) {
	I3Frame frame;

	auto split = boost::make_shared<I3RecoPulseSeriesMap>();
	I3RecoPulse p;
	p.SetTime(1);
	(*split)[OMKey(1,2,0)].push_back(p);
	p.SetTime(0);
	(*split)[OMKey(1,2,1)].push_back(p);

	frame.Put("SplitPulses", split);
	frame.Put("CombinedPulses", boost::make_shared<I3RecoPulseSeriesMapCombineByModule>("SplitPulses"));

	auto combined = frame.Get<I3RecoPulseSeriesMapConstPtr>("CombinedPulses");

	ENSURE_EQUAL(combined->size(), 1u);
	auto it = combined->find(OMKey(1,2,0));
	ENSURE(combined->find(OMKey(1,2,0)) != combined->cend());
	ENSURE(it->second.size() == 2);
	ENSURE(it->second[0].GetTime() == 0);
	ENSURE(it->second[1].GetTime() == 1);
}

