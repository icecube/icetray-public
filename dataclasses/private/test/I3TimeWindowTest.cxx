#include <I3Test.h>
#include <dataclasses/I3TimeWindow.h>

#include <boost/next_prior.hpp>
#include <boost/foreach.hpp>

TEST_GROUP(I3TimeWindowTest);

TEST(Coalesce)
{
	I3TimeWindowSeries windows;
	const double tstart = -std::numeric_limits<double>::infinity();
	
	// Add one window. This should act just like a vanilla std::list
	windows.push_back(I3TimeWindow(tstart, 1));
	ENSURE_EQUAL(windows.size(), 1u);
	ENSURE_EQUAL(windows.front().GetStart(), tstart);
	ENSURE_EQUAL(windows.front().GetStop(), 1);
	
	// Add a window that overlaps by equality; it gets merged.
	windows.push_back(I3TimeWindow(1, 2));
	ENSURE_EQUAL(windows.size(), 1u);
	ENSURE_EQUAL(windows.front().GetStart(), tstart);
	ENSURE_EQUAL(windows.front().GetStop(), 2);
	
	// Add a window that overlaps by quite a bit. Also merged.
	windows.push_back(I3TimeWindow(0, 3));
	ENSURE_EQUAL(windows.size(), 1u);
	ENSURE_EQUAL(windows.front().GetStart(), tstart);
	ENSURE_EQUAL(windows.front().GetStop(), 3);
	
	// Add a disjoint window. Now we have two.
	windows.push_back(I3TimeWindow(10, 12));
	ENSURE_EQUAL(windows.size(), 2u);
	ENSURE_EQUAL(boost::next(windows.begin())->GetStart(), 10);
	ENSURE_EQUAL(boost::next(windows.begin())->GetStop(), 12);
	
	// Add a bridging window; back to one.
	windows.push_back(I3TimeWindow(3, 10));
	ENSURE_EQUAL(windows.size(), 1u);
	ENSURE_EQUAL(windows.front().GetStart(), tstart);
	ENSURE_EQUAL(windows.front().GetStop(), 12);
}

TEST(Contains)
{
	I3TimeWindow window(0., 10.);
	// I3TimeWindows are half-open intervals
	ENSURE(window.Contains(0.));
	ENSURE(!window.Contains(10.));
	ENSURE(window.Contains(5.));
}

TEST(Complement)
{
	I3TimeWindowSeries windows, complement, intersection;
	const double tstart = -std::numeric_limits<double>::infinity();
	const double tstop = std::numeric_limits<double>::infinity();
	
	// Add one window. This should act just like a vanilla std::list
	windows.push_back(I3TimeWindow(tstart, 1));
	complement = ~windows;
	ENSURE_EQUAL(complement.size(), 1u);
	ENSURE_EQUAL(complement.front().GetStart(), 1);
	ENSURE_EQUAL(complement.front().GetStop(), tstop);
	
	// Add a disjoint window. The complement now has two parts.
	windows.push_back(I3TimeWindow(10, 12));
	complement = ~windows;
	ENSURE_EQUAL(complement.size(), 2u);
	ENSURE_EQUAL(complement.front().GetStart(), 1);
	ENSURE_EQUAL(complement.front().GetStop(), 10);
	ENSURE_EQUAL(boost::next(complement.begin())->GetStart(), 12);
	ENSURE_EQUAL(boost::next(complement.begin())->GetStop(), tstop);
	ENSURE((windows & complement).empty(), "intersection with complement is empty by construction");
	
	// The complement of the complete set is empty.
	windows.push_back(I3TimeWindow(tstart, tstop));
	complement = ~windows;
	ENSURE_EQUAL(complement.size(), 0u);
	
	// The complement of a single span has two parts.
	windows.clear();
	windows.push_back(I3TimeWindow(0, 10));
	complement = ~windows;
	ENSURE_EQUAL(complement.size(), 2u);
	ENSURE_EQUAL(complement.front().GetStart(), tstart);
	ENSURE_EQUAL(complement.front().GetStop(), 0);
	ENSURE_EQUAL(boost::next(complement.begin())->GetStart(), 10);
	ENSURE_EQUAL(boost::next(complement.begin())->GetStop(), tstop);
}

TEST(Difference)
{
	I3TimeWindowSeries readout_window, exclusions;
	readout_window.emplace_back(0,10);
	exclusions.emplace_back(-1,1); // partial overlap
	exclusions.emplace_back(2,3); // completely in readout
	exclusions.emplace_back(5,6); // completely in readout
	exclusions.emplace_back(10,11); // no overlap
	
	// Take the set difference between readout and exclusions
	I3TimeWindowSeries effective_readouts = (readout_window & (~exclusions));
	ENSURE_EQUAL(effective_readouts.size(), 3u);
	{
		auto window = effective_readouts.cbegin();
		ENSURE_EQUAL(*(window++), I3TimeWindow(1,2));
		ENSURE_EQUAL(*(window++), I3TimeWindow(3,5));
		ENSURE_EQUAL(*(window++), I3TimeWindow(6,10));
	}
}

TEST(OverlapType)
{
	I3TimeWindow t1, t2;
	
	// Completely disjoint
	t1 = I3TimeWindow(1, 5);
	t2 = I3TimeWindow(6, 10);
	ENSURE_EQUAL(t1.GetOverlapType(t2), I3TimeWindow::NONE);
	ENSURE_EQUAL(t2.GetOverlapType(t1), I3TimeWindow::NONE);
	
	// Completely identical
	t1 = I3TimeWindow(1, 5);
	t2 = I3TimeWindow(1, 5);
	ENSURE_EQUAL(t1.GetOverlapType(t2), I3TimeWindow::WITHOUT);
	
	// Nested
	t1 = I3TimeWindow(1, 5);
	t2 = I3TimeWindow(2, 4);
	ENSURE_EQUAL(t1.GetOverlapType(t2), I3TimeWindow::WITHOUT);
	ENSURE_EQUAL(t2.GetOverlapType(t1), I3TimeWindow::WITHIN);
	
	// Identical left edges
	t1 = I3TimeWindow(1, 5);
	t2 = I3TimeWindow(1, 10);
	ENSURE_EQUAL(t1.GetOverlapType(t2), I3TimeWindow::AFTER);
	ENSURE_EQUAL(t2.GetOverlapType(t1), I3TimeWindow::WITHOUT);
	
	// Identical right edges
	t1 = I3TimeWindow(1, 10);
	t2 = I3TimeWindow(5, 10);
	ENSURE_EQUAL(t1.GetOverlapType(t2), I3TimeWindow::WITHOUT);
	ENSURE_EQUAL(t2.GetOverlapType(t1), I3TimeWindow::BEFORE);
	
	// Partial overlap
	t1 = I3TimeWindow(1, 5);
	t2 = I3TimeWindow(4, 10);
	ENSURE_EQUAL(t1.GetOverlapType(t2), I3TimeWindow::AFTER);
	ENSURE_EQUAL(t2.GetOverlapType(t1), I3TimeWindow::BEFORE);
}
