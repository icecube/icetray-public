//
// I3TimeWindow.h
//
// A class to represent disjoint time spans within an event
// with rudimentary methods for logically combining such spans.
//
// Copyright (c) 2012 Jakob van Santen <vansanten@wisc.edu>
// and the IceCube Collaboration <http://www.icecube.wisc.edu>
//

#ifndef I3TIMEWINDOW_H_INCLUDED
#define I3TIMEWINDOW_H_INCLUDED

#include <dataclasses/I3Vector.h>
#include <dataclasses/I3Map.h>

class I3TimeWindow : public I3FrameObject {
public:
	I3TimeWindow() : start_(-std::numeric_limits<double>::infinity()),
	    stop_(std::numeric_limits<double>::infinity()) {};
	I3TimeWindow(double start, double stop) : start_(start), stop_(stop) {};
	std::ostream& Print(std::ostream&) const override;
	
	double GetStart() const { return start_; };
	double GetStop() const { return stop_; };
	double GetLength() const { return stop_ - start_; };
	/// Return true if the given time is inside this time window
	/// @param t a time
	bool Contains(double t) const { return (t >= start_) && (t < stop_); }
	bool operator<(const I3TimeWindow &other) const;
	bool operator==(const I3TimeWindow &other) const;
	enum OverlapType {
		BEFORE, // A and B intersect, B < A
		WITHIN, // A and B intersect, A is a subset of B
		AFTER,  // A and B intersect, B > A
		NONE,   // no intersection
		WITHOUT // A and B intersect, A is a superset of B
	};
	
	OverlapType GetOverlapType(const I3TimeWindow &) const;
private:
	double start_, stop_;
	
	friend class I3TimeWindowSeries;
	friend class icecube::serialization::access;
	template <typename Archive> void serialize(Archive &ar, unsigned);
};

class I3TimeWindowSeries : public std::list<I3TimeWindow>, public I3FrameObject {
public:
	
	I3TimeWindowSeries operator|(const I3TimeWindowSeries &other) const;
	I3TimeWindowSeries operator&(const I3TimeWindowSeries &other) const;
	I3TimeWindowSeries operator~() const;
	
	I3TimeWindowSeries() {}
	template <typename Iterator>
	I3TimeWindowSeries(Iterator first, Iterator last) : std::list<I3TimeWindow>(first, last) {}
	
	std::ostream& Print(std::ostream&) const override;
	
	void push_back(const I3TimeWindow &);
private:
	void coalesce();
	
	friend class icecube::serialization::access;
	template <typename Archive> void serialize(Archive &ar, unsigned);
};

typedef I3Map<OMKey, I3TimeWindowSeries> I3TimeWindowSeriesMap;

I3TimeWindowSeriesMap operator|(const I3TimeWindowSeriesMap &, const I3TimeWindowSeriesMap &);

std::ostream& operator<<(std::ostream& oss, const I3TimeWindow& w);
std::ostream& operator<<(std::ostream&, const I3TimeWindowSeries&);

I3_POINTER_TYPEDEFS(I3TimeWindow);
I3_POINTER_TYPEDEFS(I3TimeWindowSeries);
I3_POINTER_TYPEDEFS(I3TimeWindowSeriesMap);

static const unsigned i3timewindow_version_ = 1;
static const unsigned i3timewindowseries_version_ = 1;
static const unsigned i3timewindowseriesmap_version_ = 0;
I3_CLASS_VERSION(I3TimeWindow, i3timewindow_version_);
I3_CLASS_VERSION(I3TimeWindowSeries, i3timewindowseries_version_);
I3_CLASS_VERSION(I3TimeWindowSeriesMap, i3timewindowseriesmap_version_);

#endif // I3TIMEWINDOW_H_INCLUDED
