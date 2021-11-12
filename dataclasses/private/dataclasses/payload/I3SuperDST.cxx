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

#include <cassert>

#include <serialization/binary_object.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility.hpp>
#include <boost/foreach.hpp>

#include "icetray/OMKey.h"
#include "icetray/I3Units.h"

#include "dataclasses/payload/I3SuperDST.h"
#include "dataclasses/payload/I3SuperDSTUtils.h"

/* Maximum bit widths used in discretization */
#define I3SUPERDSTCHARGESTAMP_TIME_BITS_V0   8
#define I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0 6
#define I3SUPERDSTCHARGESTAMP_BYTESIZE_V0    2

#define I3SUPERDST_DOMID_BITS_V0     13
#define I3SUPERDST_SLOP_BITS_V0       3
#define I3SUPERDST_HEADER_BYTESIZE_V0 2

/* Earliest representable time (relative to the readout window) */
const double I3SuperDST::tmin_ = -512.0;

using namespace I3SuperDSTUtils;

I3SuperDSTChargeStamp::I3SuperDSTChargeStamp(double time, double charge, double width,
    bool hlc, Discretization format) : timecode_(0), chargecode_(0), charge_overflow_(0),
    version_(i3superdst_version_), kind_(hlc ? HLC : SLC), charge_format_(format)
{
	i3_assert( time >= 0.0 );
	i3_assert( charge >= 0.0 );
	
	/* Don't truncate integer codes, since we need to relativize the times later. */
	timecode_ = I3SuperDST::EncodeTime(time, 31, version_);
	widthcode_ = I3SuperDST::EncodeWidth(width, 31, version_);
	chargecode_ = I3SuperDST::EncodeCharge(charge, 31, version_, charge_format_);
}

void
I3SuperDSTChargeStamp::SetTimeReference(const I3SuperDSTChargeStamp &other)
{
	i3_assert(timecode_ >= other.timecode_);
	
	timecode_ -= other.timecode_;
}

double
I3SuperDSTChargeStamp::GetTime() const
{
	return I3SuperDST::DecodeTime(timecode_, version_);
}

double
I3SuperDSTChargeStamp::GetWidth() const
{
	return I3SuperDST::DecodeWidth(widthcode_, version_);
}

double
I3SuperDSTChargeStamp::GetCharge() const
{
	return I3SuperDST::DecodeCharge(chargecode_ + charge_overflow_,
	    version_, charge_format_);
}

bool
I3SuperDSTChargeStamp::operator==(const I3SuperDSTChargeStamp& rhs) const {
  return (this->timecode_ == rhs.timecode_ &&
          this->widthcode_ == rhs.widthcode_ &&
          this->chargecode_ == rhs.chargecode_ &&
          this->charge_overflow_ == rhs.charge_overflow_ &&
          this->version_ == rhs.version_ &&
          this->kind_ == rhs.kind_ &&
          this->charge_format_ == rhs.charge_format_);
}

I3SuperDSTReadout::I3SuperDSTReadout(const OMKey &om, bool hlc,
    const std::list<I3RecoPulse>::const_iterator &start,
    const std::list<I3RecoPulse>::const_iterator &end, double t0)
{
	std::list<I3RecoPulse>::const_iterator pulse_it = start;

	Discretization format = (om.GetOM() > 60) ? LOG : LINEAR;
	
	for ( ; pulse_it != end; pulse_it++) {
		stamps_.push_back(I3SuperDSTChargeStamp(pulse_it->GetTime()-t0,
		    std::max(pulse_it->GetCharge(), 0.0f),
		    std::max(pulse_it->GetWidth(), 1.0f), hlc, format));
	}
	
	time_overflow_ = 0;
	om_ = om;
	kind_ = hlc ? I3SuperDSTChargeStamp::HLC : I3SuperDSTChargeStamp::SLC;
	i3_assert(start != end);
	start_time_ = start->GetTime()-t0;
}

void
I3SuperDSTReadout::SetTimeReference(const I3SuperDSTReadout &other)
{
	const I3SuperDSTChargeStamp &other_stamp = other.stamps_.front();
	
	i3_assert(start_time_ >= other.start_time_);
	i3_assert(other.stamps_.size() > 0);
	
	/* 
	 * Set the times of any secondary stamp relative to the one prior to
	 * it, then set the time of the first stamp relative to first
	 * stamp in the other readout.
	 */
	Relativize();
	stamps_.front().SetTimeReference(other_stamp);
}

void
I3SuperDSTReadout::Relativize()
{
	std::vector<I3SuperDSTChargeStamp>::reverse_iterator stamp_rit = stamps_.rbegin();
	/* 
	 * Set the time of each secondary stamp relative to the one
	 * preceding it. (Reverse iterators are fun!)
	 */
	for ( ; stamp_rit != stamps_.rend()-1; stamp_rit++) {
		i3_assert(stamp_rit->GetTimeCode() >= (stamp_rit+1)->GetTimeCode());
		stamp_rit->SetTimeReference(*(stamp_rit+1));
	}
}

double
I3SuperDSTReadout::GetTime() const
{
	i3_assert(stamps_.size() != 0);
	
	return (stamps_.front().GetTime() +
	    I3SuperDST::DecodeTime(time_overflow_, i3superdst_version_));
}

bool
I3SuperDSTReadout::operator==(const I3SuperDSTReadout& rhs) const {
  return (this->om_ == rhs.om_ &&
          this->start_time_ == rhs.start_time_ &&
          this->stamps_ == rhs.stamps_ &&
          this->kind_ == rhs.kind_ &&
          this->time_overflow_ == rhs.time_overflow_);
}

namespace I3SuperDSTRecoPulseUtils {
	static bool
	TimeOrdering(const I3RecoPulse &p1, const I3RecoPulse &p2)
	{ return p1.GetTime() < p2.GetTime(); }
	
	static bool
	IsBorked(const I3RecoPulse &p1)
	{
		return (!std::isfinite(p1.GetTime()) || !std::isfinite(p1.GetCharge()));
	}

	static bool
	HasLC(const I3RecoPulse &p1)
	{
		return (p1.GetFlags() & I3RecoPulse::LC);
	}

	static bool
	ShouldSplit(const OMKey &key, const I3RecoPulse &current,
	    const I3RecoPulse &previous)
	{
		/*
		 * Split if pulses are not from the same launch. 
		 */
		static const double tmax = 6.45e3;

		return (key.GetOM() > 60 || HasLC(current) != HasLC(previous)
		    || current.GetTime() - previous.GetTime() > tmax);
	}
};

void
I3SuperDST::AddPulseMap(const I3RecoPulseSeriesMap &pulses, double t0)
{
	I3RecoPulseSeriesMap::const_iterator map_iter;
	std::list<I3RecoPulse> pulse_list;
	std::list<I3RecoPulse>::const_iterator pulse_head, pulse_tail;
	
	unpacked_.reset();
	
	/* 
	 * Convert the pulse series map to a list of "readouts"
	 * (pulses spaced closely in time in a single DOM)
	 */ 
	for (map_iter = pulses.begin(); map_iter != pulses.end(); map_iter++) {
		/*
		 * Get a time-ordered representation of the pulse series,
		 * except those pulses with nonsensical charges or times.
		 */
		using namespace I3SuperDSTRecoPulseUtils;
		pulse_list.clear();
		std::remove_copy_if(map_iter->second.begin(),
		    map_iter->second.end(), std::back_inserter(pulse_list),
		    IsBorked);
		if (pulse_list.size() == 0) continue;
		pulse_list.sort(TimeOrdering);
		
		pulse_head = pulse_list.begin();
		pulse_tail = boost::next(pulse_head);
		
		for ( ; pulse_tail != pulse_list.end(); pulse_tail++) {
			/* 
			 * Split the preceding run off into its own readout
			 * if it is from a different kind of launch or this is
			 * and IceTop DOM (where we assume exactly one pulse
			 * per launch)
			 */
			if (ShouldSplit(map_iter->first, *pulse_tail,
			    *(pulse_head))) {
				readouts_.push_back(I3SuperDSTReadout(
				    map_iter->first, HasLC(*pulse_head),
				    pulse_head, pulse_tail, t0));
				pulse_head = pulse_tail;
			}
		}
		
		/* Push the remaining pulses into a readout. */
		readouts_.push_back(I3SuperDSTReadout(
		    map_iter->first, HasLC(*pulse_head), pulse_head, pulse_tail, t0));
	}
}

void
I3SuperDST::AddPulseMap(const I3RecoPulseSeriesMap &pulses)
{
	/* 
	 * Get the earliest leading-edge time present in the input.
	 * This must be >= -512 ns to be representable.
	 */
	const double t0 = FindStartTime(pulses);
	if (t0 < tmin_)
		log_fatal("First pulse time %g ns < %3.0f ns is unrepresentable! "
		    "Is this un-timeshifted simulation?", t0, tmin_);
	
	/* 
	 * Convert the pulse series map to a list of "readouts"
	 * (pulses spaced closely in time in a single DOM)
	 */ 
	AddPulseMap(pulses, tmin_);

	/* Sort the readouts by start time */
	readouts_.sort();
	
	/* Convert the absolute times in each readout to time deltas. */
	std::list<I3SuperDSTReadout>::reverse_iterator list_rit = readouts_.rbegin();
	if (list_rit != readouts_.rend()) {
		for ( ; boost::next(list_rit) != readouts_.rend(); list_rit++)
			list_rit->SetTimeReference(*boost::next(list_rit));
		list_rit->Relativize(); /* Relativize the first readout as well. */
		i3_assert(boost::next(list_rit).base() == readouts_.begin());
	}
}

std::list<I3SuperDSTReadout>
I3SuperDST::GetReadouts(bool hlc) const
{
	std::list<I3SuperDSTReadout> filtered;
	std::list<I3SuperDSTReadout>::const_iterator list_it;
	
	for (list_it = readouts_.begin(); list_it != readouts_.end(); list_it++)
		if (hlc == (list_it->kind_ == I3SuperDSTChargeStamp::HLC))
			filtered.push_back(*list_it);
			
	return filtered;
}

I3SuperDST::I3SuperDST(const I3RecoPulseSeriesMap &pulses)
    : version_(i3superdst_version_)
{
	AddPulseMap(pulses);
	
	InitDebug();
}

/* Expand charge stamps into fake I3RecoPulses */
I3RecoPulseSeriesMapConstPtr
I3SuperDST::Unpack() const
{
	if (unpacked_)
		return unpacked_;

	std::vector<I3SuperDSTChargeStamp>::const_iterator stamp_it;
	std::list<I3SuperDSTReadout>::const_iterator readout_it;
	double t_ref;
	
	unpacked_ = I3RecoPulseSeriesMapPtr(new I3RecoPulseSeriesMap);

	t_ref = tmin_;
	
	for (readout_it = readouts_.begin(); readout_it != readouts_.end(); readout_it++) {
		int flags = (readout_it->kind_ == I3SuperDSTChargeStamp::HLC) ?
		    I3RecoPulse::ATWD | I3RecoPulse::FADC | I3RecoPulse::LC : I3RecoPulse::FADC;
		I3RecoPulseSeries &target = unpacked_->operator[](readout_it->om_);
		
		t_ref += readout_it->GetTime();
		double t_ref_internal = t_ref;
		
		stamp_it = readout_it->stamps_.begin();
		
		if (stamp_it != readout_it->stamps_.end()) {
			I3RecoPulse pulse;
			
			pulse.SetTime(t_ref);
			pulse.SetCharge(stamp_it->GetCharge());
			pulse.SetWidth(stamp_it->GetWidth());
			pulse.SetFlags(flags);
			target.push_back(pulse);
			
			stamp_it++;
		}
	
		for ( ; stamp_it != readout_it->stamps_.end(); stamp_it++) {
			I3RecoPulse pulse;
			
			t_ref_internal += stamp_it->GetTime();
			pulse.SetTime(t_ref_internal);
			pulse.SetCharge(stamp_it->GetCharge());
			pulse.SetWidth(stamp_it->GetWidth());
			pulse.SetFlags(flags);
			target.push_back(pulse);
		}
	}

	BOOST_FOREACH(I3RecoPulseSeriesMap::value_type &target, *unpacked_) {
		std::sort(target.second.begin(), target.second.end(), 
		    I3SuperDSTRecoPulseUtils::TimeOrdering);

		I3RecoPulseSeries::iterator prev, current, next;
		prev = target.second.end();
		current = target.second.begin();
		next = current+1;
		bool merge = false;
		/* Ensure that pulses do not overlap. */
		while (next < target.second.end()) {
			current->SetWidth(std::min(current->GetWidth(),
			    float(next->GetTime()-current->GetTime())));
			if (current->GetWidth() == 0 || merge) {
				if (prev != target.second.end()) {
					/* Merge widths with previous pulse */
					if (prev->GetWidth() == 0 && current->GetWidth() > 0)
						prev->SetWidth(current->GetWidth());
					current->SetWidth(prev->GetWidth()/2.);
					prev->SetWidth(prev->GetWidth()/2.);
					current->SetTime(prev->GetTime()+prev->GetWidth());
					merge = false;
				} else {
					merge = true;
				}
			} else {
				merge = false;
			}
			prev = current;
			current++;
			next = current+1;
		}
		if (merge && prev != target.second.end()) {
			if (prev->GetWidth() == 0 && current->GetWidth() > 0)
				prev->SetWidth(current->GetWidth());
			current->SetWidth(prev->GetWidth()/2.);
			prev->SetWidth(prev->GetWidth()/2.);
			current->SetTime(prev->GetTime()+prev->GetWidth());
		}
	}
	
	return unpacked_;
}

uint32_t
I3SuperDST::EncodeOMKey(const OMKey &key, unsigned int maxbits, unsigned int version)
{
	uint32_t encoded(0);
	int string = key.GetString();
	int om = key.GetOM();

	i3_assert(string >= 0);
	i3_assert(om > 0);
	i3_assert(om <= 64);
	i3_assert(maxbits <= 31);
	
	/*
	 * NB: this is the same encoding scheme as the DAQ:
	 * upper 7 bits: string number, lower 6 bits: OM number
	 */
	encoded |= ((unsigned(string) << 6) & ~((1<<6)-1));
	encoded |= ((unsigned(om)-1) & ((1<<6)-1));
	i3_assert( encoded <= uint32_t((1<<maxbits)-1) );
	
	return (encoded);
}

OMKey
I3SuperDST::DecodeOMKey(uint32_t dom_id, unsigned int version)
{
	int string(0), om(0);
	
	string = unsigned((dom_id >> 6) & ((1<<7)-1));
	om = unsigned(dom_id & ((1<<6)-1))+1;
	
	return OMKey(string, om);
}

inline uint32_t
truncate(double val, unsigned int maxbits)
{
	uint32_t code(lround(val)); /* Round to nearest integer */
	i3_assert(maxbits <= 31);
	
	/* Truncate if greater than 2^(maxbits)-1 */
	if (code > uint32_t((1<<maxbits)-1))
		code = uint32_t((1<<maxbits)-1);
	
	return (code);
}

uint32_t
I3SuperDST::EncodeTime(double time, unsigned int maxbits,
    unsigned int version)
{
	uint32_t encoded(0);
	
	i3_assert( time >= 0.0 );
	switch (version) {
		case 0:
			encoded = truncate(time / (4.0*I3Units::ns), maxbits);
			break;
		case 1:
			encoded = truncate(time / (1.0*I3Units::ns), maxbits);
			break;
		default:
			break;
	}
	
	return (encoded);
}

double
I3SuperDST::DecodeTime(uint32_t dt, unsigned int version)
{
	double decoded(0);
	
	switch (version) {
		case 0:
			decoded = dt*4.0*I3Units::ns;
			break;
		case 1:
			decoded = dt*1.0*I3Units::ns;
			break;
		default:
			break;
	}
	
	return (decoded);
}

inline unsigned
GetExponent(uint64_t i, unsigned mbits, unsigned ebits)
{
	using namespace I3SuperDSTUtils;
	i3_assert(mbits > 0);
	i3_assert(ebits > 0);
	int e = std::max(findlastset(i)-int(mbits-1), 0);
	return std::min(unsigned(e), (1u << ebits)-1);
}

uint32_t
I3SuperDST::EncodeWidth(double width, unsigned int maxbits,
    unsigned int version)
{
	i3_assert(width > 0 && width < double(std::numeric_limits<unsigned>::max()));
	unsigned rounded = unsigned(ceil(width/1.0));
	unsigned code = std::min(unsigned(findlastset(rounded)), (1u << maxbits)-1);
	return ((code > 0) && (rounded == (1u << (code-1)))) ? code-1 : code;
}

double
I3SuperDST::DecodeWidth(uint32_t dt, unsigned int version)
{
	i3_assert(dt <= 31);
	return double(1u << dt);
}

uint32_t
I3SuperDST::EncodeCharge(double charge, unsigned int maxbits,
    unsigned int version, Discretization mode)
{
	uint32_t encoded(0);
	
	i3_assert( charge >= 0 );
	
	if (mode == LOG) {
		i3_assert(maxbits >= 14);
		double logq = std::max(0.0, log10(charge) + 2.0);
		double step = 9.0/double((1<<14)-1);
		encoded = truncate(logq/step, maxbits);
	} else {
		if (version == 0) {
			encoded = truncate(charge/0.15, maxbits);
		} else {
			encoded = uint32_t(floor(std::max(0., charge)/0.05));	
		}
	}

	
	return (encoded);
}

double
I3SuperDST::DecodeCharge(uint32_t chargecode, unsigned int version,
    Discretization mode)

{
	double decoded(0);
	
	if (mode == LOG) {
		double step = 9.0/double((1<<14)-1);
		decoded = pow(10., chargecode*step - 2.0);
	} else {
		if (version == 0)
			decoded = chargecode*0.15;
		else {
			// Decode at bin center 
			decoded = chargecode*0.05 + 0.025;
		}
	}
	
	return (decoded);
}

double
I3SuperDST::FindStartTime(const I3RecoPulseSeriesMap &pmap)
{
	double tmin = std::numeric_limits<double>::max();
	I3RecoPulseSeriesMap::const_iterator map_it = pmap.begin();
	I3RecoPulseSeries::const_iterator pulse_it;
	
	for ( ; map_it != pmap.end() ; map_it++)
		for (pulse_it = map_it->second.begin();
		    pulse_it != map_it->second.end(); pulse_it++)
			if (std::isfinite(pulse_it->GetTime()) &&
			    pulse_it->GetTime() < tmin)
				tmin = pulse_it->GetTime();
				
	return (tmin);
}

bool
I3SuperDST::operator==(const I3SuperDST& rhs) const {
  if (bool(this->unpacked_) != bool(rhs.unpacked_)) {
    return false;
  }
  if (this->unpacked_) {
    if (!(*(this->unpacked_) == *(rhs.unpacked_))) {
      return false;
    }
  }
  return (this->readouts_ == rhs.readouts_ &&
          this->tmin_ == rhs.tmin_ &&
          this->version_ == rhs.version_);
}

/* 
 * NB: While the internal layout of bitfields is implementation-defined,
 * (see ISO 14882 [http://www-d0.fnal.gov/~dladams/cxx_standard.pdf] p. 159)
 * GCC >= 3.4.6 obeys the following conventions under __attribute((packed)):
 *
 * - on little-endian platforms, fields are laid out in native byte order
 *   starting from the least significant bit of the parent struct
 * 
 * - on big-endian platforms, fields are laid out starting from the most
 *   significant bit of the parent struct
 *
 * To properly interpet the binary representation of a bitfield from a little-
 * endian platform on a big-endian one (and vice versa), it is necessary not
 * only to swap the order of the bytes in memory, but also to reverse the
 * order of the fields.
 */

namespace I3SuperDSTSerialization {

struct DOMHeader {
#if BYTE_ORDER == BIG_ENDIAN
	uint16_t slop      : I3SUPERDST_SLOP_BITS_V0; /* most significant 3 bits of first HLC time */
	uint16_t dom_id    : I3SUPERDST_DOMID_BITS_V0;
#else
	uint16_t dom_id    : I3SUPERDST_DOMID_BITS_V0;
	uint16_t slop      : I3SUPERDST_SLOP_BITS_V0; /* most significant 3 bits of first HLC time */
#endif
} __attribute((packed));

BOOST_STATIC_ASSERT( sizeof(DOMHeader) == I3SUPERDST_HEADER_BYTESIZE_V0 );

union ChargeStamp {
	struct {
#if BYTE_ORDER == BIG_ENDIAN
		uint16_t stop     : 1;
		uint16_t hlc_bit  : 1;
		uint16_t charge   : I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0;
		uint16_t rel_time : I3SUPERDSTCHARGESTAMP_TIME_BITS_V0;
#else
		uint16_t rel_time : I3SUPERDSTCHARGESTAMP_TIME_BITS_V0;
		uint16_t charge   : I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0;
		uint16_t hlc_bit  : 1;
		uint16_t stop     : 1;
#endif
	} stamp;
	struct {
#if BYTE_ORDER == BIG_ENDIAN
		uint16_t stop : 1; /* The stop bit is always valid. */
		uint16_t code : 15;
#else
		uint16_t code : 15;
		uint16_t stop : 1; /* The stop bit is always valid. */
#endif
	} overflow;
	uint16_t raw;
} __attribute((packed));

BOOST_STATIC_ASSERT( sizeof(ChargeStamp) == I3SUPERDSTCHARGESTAMP_BYTESIZE_V0 );

}

#ifndef NDEBUG
#include <sys/resource.h>
class I3SuperDSTTimer {
public:
	I3SuperDSTTimer(double *acc, unsigned *counter)
	: acc_(acc), counter_(counter)
	{
		if (acc_)
			err_ = getrusage(RUSAGE_SELF, &start_);
	}
	~I3SuperDSTTimer()
	{
		if ((acc_) && !err_ && !getrusage(RUSAGE_SELF, &end_)) {
			*acc_ += double(end_.ru_utime.tv_sec + end_.ru_stime.tv_sec
			    - start_.ru_utime.tv_sec - start_.ru_stime.tv_sec)
			    + double(end_.ru_utime.tv_usec + end_.ru_stime.tv_usec
			    - start_.ru_utime.tv_usec - start_.ru_stime.tv_usec)
			    *1e-6;
		}
		if (counter_)
			*counter_ += 1;
	}
private:
	double *acc_;
	unsigned *counter_;
	int err_;
	rusage start_, end_;
};
#endif

template <class T>
static void
swap_vector(std::vector<T> &vec)
{
#if BYTE_ORDER == BIG_ENDIAN
	BOOST_STATIC_ASSERT(sizeof(T) == 2);
	typename std::vector<T>::iterator it = vec.begin();
	for ( ; it != vec.end(); it++) {
		uint16_t *swapit = (uint16_t*)(&*it);
		*swapit = ((*swapit & 0x00FF) << 8) | ((*swapit & 0xFF00) >> 8);	
	}
#endif
};

I3MapKeyVectorInt
I3SuperDST::GetEncodedSizes() const
{
	I3MapKeyVectorInt sizes;
	std::ostringstream oarchive_stream;
	icecube::archive::portable_binary_oarchive oarchive(oarchive_stream);

	/* Piggy-back on the current implementation of save() */
	this->save(oarchive, icecube::serialization::version<I3SuperDST>::value,
	    &sizes);

	return sizes;
}

namespace I3SuperDSTUtils {

	void RunCodec::EncodeRun(vector_t &codes, uint8_t val, unsigned len)
	{
		unsigned nblocks = (findlastset(len)-1)/4 + 1;
#if BYTE_ORDER == BIG_ENDIAN
		for (unsigned i=nblocks-1; i >= 0; i--) {
#else 
		for (unsigned i=0; i < nblocks; i++) {
#endif
			uint8_t code = (val & 0xf);;
			code |= ((len >> (i*4u)) << 4) & 0xf0;
			codes.push_back(code);
		}
	}

	void RunCodec::DecodeRun(vector_t &target,
	    const vector_t::const_iterator &head,
	    const vector_t::const_iterator &tail)
	{
		uint8_t code = *head & 0xf;
		unsigned runlength = 0;
		unsigned offset = 0;
#if BYTE_ORDER == BIG_ENDIAN
		vector_t::const_iterator it = tail-1;
		for ( ; it >= head; it--, offset+=4u)
#else		
		vector_t::const_iterator it = head;
		for ( ; it != tail; it++, offset+=4u)
#endif
			runlength |= (((*it >> 4) & 0xf) << offset);
		target.insert(target.end(), runlength, code);
	}

	void RunCodec::Encode(const vector_t &runs, vector_t &codes)
	{
		vector_t::const_iterator head, tail;
		head = runs.begin();
		tail = head+1;
		for ( ; tail < runs.end(); tail++)
			if (*tail != *head) {
				/* Start a new run */
				unsigned runlength = std::distance(head, tail);
				EncodeRun(codes, *head, runlength);
				head = tail;
			}

		/* True by construction, unless the vector has length 0 */
		if (tail == runs.end()) {
			unsigned runlength = std::distance(head, tail);
			EncodeRun(codes, *head, runlength);
		}
	}

	void RunCodec::Decode(const std::vector<uint8_t> &codes, vector_t &runs)
	{
		vector_t::const_iterator head, tail;
		runs.clear();
		head = codes.begin();
		tail = head+1;
		for ( ; tail < codes.end(); tail++)
			if ((*tail & 0xf) != (*head & 0xf)) {
				/* A run has ended. */
				DecodeRun(runs, head, tail);
				head = tail;
			}

		/* True by construction, unless the vector has length 0 */
		if (tail == codes.end()) {
			DecodeRun(runs, head, tail);
		}
	}

	template <class Archive>
	void SizeCodec::save(Archive &ar, unsigned version) const
	{
		if (size_ < 0xff-sizeof(size_type)) {
			uint8_t tag = size_;
			ar & make_nvp("Tag", tag);
		} else {
			uint8_t n_bytes = (findlastset(size_)-1)/8 + 1;
			uint8_t tag = 0xff - n_bytes;
			ar & make_nvp("Tag", tag);

			uint8_t bytes[8];
			uint64_t s = size_;
#if BYTE_ORDER == BIG_ENDIAN
			for (unsigned i=n_bytes-1; i >= 0; i--, s=s>>8)
#else
			for (unsigned i=0; i < n_bytes; i++, s=s>>8)
#endif
				bytes[i] = s & 0xff;

			ar & make_nvp("Bytes", icecube::serialization::make_binary_object(
			    bytes, n_bytes));
		}
	}
	
	template <class Archive>
	void SizeCodec::load(Archive &ar, unsigned version)
	{
		uint8_t tag = 0;
		size_ = 0;
		ar & make_nvp("Tag", tag);
		if (tag < 0xff-sizeof(size_type)) {
			size_ = tag;
		} else {
			unsigned n_bytes = 0xff - tag;
			uint8_t bytes[8];
			ar & make_nvp("Bytes", icecube::serialization::make_binary_object(
			    bytes, n_bytes));

#if BYTE_ORDER == BIG_ENDIAN
			for (unsigned i=n_bytes-1; i >= 0; i--)
#else
			for (unsigned i=0; i < n_bytes; i++)
#endif
				size_ |= (size_t(bytes[i]) << (i*8));
		}
	}

}

template <class Archive>
void
I3SuperDST::save(Archive& ar, unsigned version) const
{
	save(ar, version, NULL);
}

namespace {

size_t
encode_overflow(CompactVector<I3SuperDSTSerialization::ChargeStamp> &stamp_stream,
   unsigned code, unsigned max_overflow)
{
	size_t readout_bytes = 0;
	
	I3SuperDSTSerialization::ChargeStamp stamp;
	// emit overflow in blocks of max_overflow
	do {
		stamp.raw = std::min(code, max_overflow);
		code -= std::min(code, max_overflow);
	
		stamp_stream.push_back(stamp);
		readout_bytes += sizeof(stamp);

	} while (code > 0);
	
	// the decoder interprets stamps with value max_overflow as a signal
	// to continue. emit one with value 0 to make it stop.
	if (stamp.raw == max_overflow) {
		stamp.raw = 0;
		stamp_stream.push_back(stamp);
		readout_bytes += sizeof(stamp);
	}
	
	return readout_bytes;
}

}

template <class Archive>
void
I3SuperDST::save(Archive& ar, unsigned version,
    std::map<OMKey, std::vector<int > > *sizes) const
{
#ifndef NDEBUG
	I3SuperDSTTimer timer(serialization_time_, serialization_counter_);
#endif
	CompactVector<I3SuperDSTSerialization::DOMHeader> header_stream;
	CompactVector<I3SuperDSTSerialization::ChargeStamp> stamp_stream;
	CompactVector<uint8_t> ldr_stream;
	std::vector<uint8_t> widths[4];
	
	std::list<I3SuperDSTReadout>::const_iterator readout_it;
	std::vector<I3SuperDSTChargeStamp>::const_iterator stamp_it;
	
	const unsigned max_timecode_header = (1u << (I3SUPERDSTCHARGESTAMP_TIME_BITS_V0
	    + I3SUPERDST_SLOP_BITS_V0)) - 1;
	const unsigned max_timecode = (1u << I3SUPERDSTCHARGESTAMP_TIME_BITS_V0) - 1;
	const unsigned max_chargecode = (1u << I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0) - 1;
	const unsigned max_overflow = UINT16_MAX;

	for (readout_it = readouts_.begin();
	    readout_it != readouts_.end(); readout_it++) {
		
		I3SuperDSTSerialization::DOMHeader header;
		
		header.dom_id = EncodeOMKey(readout_it->om_, 13, version);
		size_t readout_bytes = sizeof(header);

		// Widths: {InIce SLC, InIce HLC, IceTop SLC, IceTop HLC}
		std::vector<uint8_t> &width =
		    widths[2*(readout_it->om_.GetOM() > 60) + readout_it->GetLCBit()];
		
		/*
		 * Special case for the first stamp in the series:
		 * use the slop space in the header to extend the time range.
		 */
		stamp_it = readout_it->stamps_.begin();
		Discretization charge_format = stamp_it->GetChargeFormat();
		if (stamp_it != readout_it->stamps_.end()) {
			unsigned timecode = stamp_it->GetTimeCode();
			unsigned chargecode = stamp_it->GetChargeCode();
			const bool stop = (boost::next(stamp_it) == readout_it->stamps_.end());
			header.slop = (std::min(timecode, max_timecode_header)
			    >> I3SUPERDSTCHARGESTAMP_TIME_BITS_V0)
			    & ((1 << I3SUPERDST_SLOP_BITS_V0)-1);
			
			I3SuperDSTSerialization::ChargeStamp stampytown; /* Population: 5! */
			stampytown.stamp.rel_time = std::min(timecode, max_timecode_header)
			    & ((1 << I3SUPERDSTCHARGESTAMP_TIME_BITS_V0)-1);
			stampytown.stamp.hlc_bit = stamp_it->GetLCBit();
			stampytown.stamp.stop = stop;

			if (charge_format == LINEAR) {
				stampytown.stamp.charge =
				    std::min(chargecode, max_chargecode); 
			} else {
				/*
				 * Special case for floating-point scheme: pack lower 6
				 * bits in the charge stamp; upper 8 bits in an
				 * extra stream.
				 */ 
				stampytown.stamp.charge = chargecode &
				    ((1<<I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0)-1);
				uint8_t ldr = (chargecode >> I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0) &
				    ((1<<8)-1);
				ldr_stream.push_back(ldr);
				readout_bytes += sizeof(ldr);
			}

			width.push_back(stamp_it->GetWidthCode());
			stamp_stream.push_back(stampytown);
			readout_bytes += sizeof(stampytown);
			
			/*
			 * If we still have any time overflow, subtract it
			 * off in 16-bit chunks until it's gone. The saturated
			 * timecode serves as a sentinel for this case.
			 */
			if (timecode >= max_timecode_header) {
				readout_bytes += encode_overflow(stamp_stream,
				    timecode-max_timecode_header, max_overflow);
			}
			
			/*
			 * Repeat the same overflow procedure for charge in the
			 * linear scheme. The floating-point scheme has no overflow.
			 */
			if (charge_format == LINEAR && chargecode >= max_chargecode) {
				readout_bytes += encode_overflow(stamp_stream,
				    chargecode-max_chargecode, max_overflow);
			}
			
			stamp_it++;
		}
		
		header_stream.push_back(header);
		
		/* Only one stamp in the floating point scheme */
		i3_assert(charge_format != LOG || stamp_it == readout_it->stamps_.end());
		
		for ( ; stamp_it != readout_it->stamps_.end(); ) {
			I3SuperDSTSerialization::ChargeStamp stampytown;
			const bool stop = (boost::next(stamp_it)
			    == readout_it->stamps_.end());
			unsigned chargecode = stamp_it->GetChargeCode();
			unsigned timecode = stamp_it->GetTimeCode();
			
			stampytown.stamp.rel_time = std::min(timecode, max_timecode);
			stampytown.stamp.charge = std::min(chargecode, max_chargecode);
			stampytown.stamp.hlc_bit = stamp_it->GetLCBit();
			
			stampytown.stamp.stop = stop;
			width.push_back(stamp_it->GetWidthCode());
			stamp_stream.push_back(stampytown);
			readout_bytes += sizeof(stampytown);
			
			/* Encode any time overflow. */
			if (timecode >= max_timecode) {
				readout_bytes += encode_overflow(stamp_stream,
				    timecode-max_timecode, max_overflow);
			}
			
			/* Encode any charge overflow. */
			if (chargecode >= max_chargecode) {
				readout_bytes += encode_overflow(stamp_stream,
				    chargecode-max_chargecode, max_overflow);
			}
			
			stamp_it++;
		}

		if (sizes != NULL)
			(*sizes)[readout_it->om_].push_back(readout_bytes);
	}
	
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

	swap_vector(stamp_stream);
	ar & make_nvp("ChargeStamps", stamp_stream);

	swap_vector(header_stream);
	ar & make_nvp("DOMHeaders", header_stream);

	CompactVector<uint8_t> width_runs;
	for (int i = 0; i < 4; i++) {
		width_runs.clear();
		RunCodec::Encode(widths[i], width_runs);
		ar & make_nvp("Widths", width_runs);
	}

	ar & make_nvp("ExtraBytes", ldr_stream);
	
	return;
}

template <typename Archive>
void I3SuperDST::load_v0(Archive &ar)
{
	uint16_t stream_size;
	ar & make_nvp("NRecords", stream_size);
	std::vector<I3SuperDSTSerialization::ChargeStamp> stamp_stream;
	stamp_stream.resize(stream_size);
	ar & make_nvp("ChargeStamps",
	    icecube::serialization::make_binary_object(&stamp_stream.front(),
	    stream_size*sizeof(I3SuperDSTSerialization::ChargeStamp)));
	swap_vector(stamp_stream);
	
	/* Count the number of distinct units in the stamp stream */
	unsigned n_headers = 0;
	BOOST_FOREACH(const I3SuperDSTSerialization::ChargeStamp &stamp, stamp_stream)
		if (stamp.stamp.stop)
			n_headers++;
		
	/* Now that we know the length of the header stream, resurrect it. */
	std::vector<I3SuperDSTSerialization::DOMHeader> header_stream;
	header_stream.resize(n_headers);
	ar & make_nvp("DOMHeaders",
	    icecube::serialization::make_binary_object(&header_stream.front(),
	    n_headers*sizeof(I3SuperDSTSerialization::DOMHeader)));
	swap_vector(header_stream);

	/* Saturation values */
	const unsigned max_timecode_header = (1 << (I3SUPERDSTCHARGESTAMP_TIME_BITS_V0
	    + I3SUPERDST_SLOP_BITS_V0)) - 1;
	const unsigned max_chargecode = (1u << I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0) - 1;
	
	std::vector<I3SuperDSTSerialization::ChargeStamp>::const_iterator stamp_it
	    = stamp_stream.begin();
	std::vector<I3SuperDSTSerialization::DOMHeader>::const_iterator header_it
	    = header_stream.begin();

	const uint32_t widthcode =
	    I3SuperDST::EncodeWidth(I3SuperDST::DecodeTime(1, 0), 0);
	
	for ( ; header_it != header_stream.end(); header_it++) {
		I3SuperDSTReadout readout;
		
		readout.om_ = I3SuperDST::DecodeOMKey(header_it->dom_id, 0);
		bool hlc = stamp_it->stamp.hlc_bit;
		
		/* 
		 * Use the slop bits in the header as the most significant
		 * bits for the time of the first header.
		 */
		unsigned timecode = stamp_it->stamp.rel_time;
		unsigned chargecode = stamp_it->stamp.charge;
		timecode |= (unsigned(header_it->slop) <<
		    I3SUPERDSTCHARGESTAMP_TIME_BITS_V0);
		
		/* If the time is saturated, add the next 15 bits. */
		if (timecode == max_timecode_header) {
			stamp_it++;
			timecode += stamp_it->overflow.code;
			//readout.time_overflow_ = stamp_it->overflow.code;
		}
	
		/* If the time is saturated, add the next 15 bits. */
		if (chargecode == max_chargecode) {
			stamp_it++;
			chargecode += stamp_it->overflow.code;
		}
		
		I3SuperDSTChargeStamp stamp(timecode, chargecode, widthcode,
		    hlc, LINEAR, 0);
		
		readout.stamps_.push_back(stamp);
		
		/* Read in the remaining stamps until we hit a stop. */
		while (!stamp_it->stamp.stop) {
			stamp_it++;
			
			/* No funny schtuff, ja Lebovski? */
			i3_assert( stamp_it->stamp.hlc_bit == hlc );
			
			timecode = stamp_it->stamp.rel_time;
			chargecode = stamp_it->stamp.charge;
			
			/* Add charge overflow if we have it */
			if (chargecode == max_chargecode) {
				stamp_it++;
				chargecode += stamp_it->overflow.code;
			}
			
			I3SuperDSTChargeStamp stamp(timecode, chargecode, widthcode,
			    hlc, LINEAR, 0);
			
			readout.stamps_.push_back(stamp);
		}
		i3_assert( stamp_it != stamp_stream.end() );
		stamp_it++; /* Advance for the next readout */
		
		readout.kind_ = hlc ? I3SuperDSTChargeStamp::HLC : I3SuperDSTChargeStamp::SLC;
		readouts_.push_back(readout);
	}
	
	/* Populate the start_time_ fields of the readouts_ for consistency. */
	double t_ref = 0.0;
	BOOST_FOREACH(I3SuperDSTReadout &readout, readouts_) {
		i3_assert(readout.stamps_.size() > 0);
		t_ref += readout.GetTime();
		readout.start_time_ = t_ref;
	}
}

template <typename Archive>
void I3SuperDST::load_v1(Archive &ar)
{
	CompactVector<I3SuperDSTSerialization::ChargeStamp> stamp_stream;
	ar & make_nvp("ChargeStamps", stamp_stream);
	swap_vector(stamp_stream);

	CompactVector<I3SuperDSTSerialization::DOMHeader> header_stream;
	ar & make_nvp("DOMHeaders", header_stream);
	swap_vector(header_stream);

	std::vector<uint8_t> widths[4];
	std::vector<uint8_t>::const_iterator width_its[4];
	CompactVector<uint8_t> width_runs;
	for (int i = 0; i < 4; i++) {
		width_runs.clear();
		ar & make_nvp("Widths", width_runs);
		RunCodec::Decode(width_runs, widths[i]);
		width_its[i] = widths[i].begin();
	}

	CompactVector<uint8_t> byte_stream;
	ar & make_nvp("ExtraBytes", byte_stream);
	
	/* Saturation values */
	const unsigned max_timecode_header = (1 << (I3SUPERDSTCHARGESTAMP_TIME_BITS_V0
	    + I3SUPERDST_SLOP_BITS_V0)) - 1;
	const unsigned max_timecode = (1u << I3SUPERDSTCHARGESTAMP_TIME_BITS_V0) - 1;
	const unsigned max_chargecode = (1u << I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0) - 1;
	
	std::vector<I3SuperDSTSerialization::ChargeStamp>::const_iterator stamp_it
	    = stamp_stream.begin();
	std::vector<I3SuperDSTSerialization::DOMHeader>::const_iterator header_it
	    = header_stream.begin();
	std::vector<uint8_t>::const_iterator ldr_it = byte_stream.begin();
	
	for ( ; header_it != header_stream.end(); header_it++) {
		I3SuperDSTReadout readout;
		
		readout.om_ = I3SuperDST::DecodeOMKey(header_it->dom_id, 1);
		i3_assert(stamp_it < stamp_stream.end());
		bool hlc = stamp_it->stamp.hlc_bit;
		bool stop = stamp_it->stamp.stop;

		/* Widths: {InIce SLC, InIce HLC, IceTop SLC, IceTop HLC} */
		std::vector<uint8_t>::const_iterator &width_it =
		    width_its[2*(readout.om_.GetOM() > 60) + hlc];
		std::vector<uint8_t>::const_iterator width_end =
		    widths[2*(readout.om_.GetOM() > 60) + hlc].end();
		
		/* 
		 * Use the slop bits in the header as the most significant
		 * bits for the time of the first stamp.
		 */
		unsigned timecode = stamp_it->stamp.rel_time;
		unsigned chargecode = stamp_it->stamp.charge;
		timecode |= (unsigned(header_it->slop) <<
		    I3SUPERDSTCHARGESTAMP_TIME_BITS_V0);

		Discretization charge_format = (readout.om_.GetOM() > 60)
		    ? LOG : LINEAR;
		
		if (timecode == max_timecode_header) {
			i3_assert(stamp_it < stamp_stream.end());
			do {
				timecode += (++stamp_it)->raw;
			} while (stamp_it->raw == UINT16_MAX);
		}
		if (charge_format == LINEAR && chargecode == max_chargecode) {
			i3_assert(stamp_it < stamp_stream.end());
			do {
				chargecode += (++stamp_it)->raw;
			} while (stamp_it->raw == UINT16_MAX);
		} else if (charge_format == LOG) {
			i3_assert(ldr_it < byte_stream.end());
			chargecode |= unsigned(*ldr_it) <<
			    I3SUPERDSTCHARGESTAMP_CHARGE_BITS_V0;
			ldr_it++;
		}
		
		i3_assert(width_it < width_end);
		I3SuperDSTChargeStamp stamp(timecode, chargecode, *width_it++,
		    hlc, charge_format, 1);
		
		readout.stamps_.push_back(stamp);
		
		/* Read in the remaining stamps until we hit a stop. */
		while (!stop) {
			stamp_it++;
			
			/* No funny schtuff, ja Lebovski? */
			i3_assert(charge_format == LINEAR);
			i3_assert( stamp_it->stamp.hlc_bit == hlc );
			
			timecode = stamp_it->stamp.rel_time;
			chargecode = stamp_it->stamp.charge;
			stop = stamp_it->stamp.stop;
			
			if (timecode == max_timecode) {
				i3_assert(stamp_it < stamp_stream.end());
				do {
					timecode += (++stamp_it)->raw;
				} while (stamp_it->raw == UINT16_MAX);
			}
			if (chargecode == max_chargecode) {
				i3_assert(stamp_it < stamp_stream.end());
				do {
					chargecode += (++stamp_it)->raw;
				} while (stamp_it->raw == UINT16_MAX);
			}
			
			i3_assert(width_it < width_end);
			I3SuperDSTChargeStamp stamp(timecode, chargecode, *width_it++,
			    hlc, LINEAR, 1);
			
			readout.stamps_.push_back(stamp);
		}
		i3_assert( stamp_it < stamp_stream.end() );
		stamp_it++; /* Advance for the next readout */
		
		readout.kind_ = hlc ? I3SuperDSTChargeStamp::HLC : I3SuperDSTChargeStamp::SLC;
		readouts_.push_back(readout);
	}

	i3_assert(stamp_it == stamp_stream.end());


	/* Populate the start_time_ fields of the readouts_ for consistency. */
	double t_ref = 0.0;
	BOOST_FOREACH(I3SuperDSTReadout &readout, readouts_) {
		i3_assert(readout.stamps_.size() > 0);
		t_ref += readout.GetTime();
		readout.start_time_ = t_ref;
	}
}

template <class Archive>
void
I3SuperDST::load(Archive& ar, unsigned version)
{
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

	switch (version) {
		case 0:
			load_v0(ar);
			break;
		case 1:
			load_v1(ar);
			break;
		default:
			log_fatal("Foo!");
	}
	
	version_ = version;
	
	return;
}

I3_SPLIT_SERIALIZABLE(I3SuperDST);

// explicitly instantiate serialization functions to avoid linker errors
// when compiling superdst-test
template void I3SuperDSTUtils::SizeCodec::save(icecube::archive::portable_binary_oarchive&, unsigned) const;
template void I3SuperDSTUtils::SizeCodec::load(icecube::archive::portable_binary_iarchive&, unsigned);
template void I3SuperDSTUtils::SizeCodec::load(icecube::archive::xml_iarchive&, unsigned);
template void I3SuperDSTUtils::SizeCodec::save(icecube::archive::xml_oarchive&, unsigned) const;


