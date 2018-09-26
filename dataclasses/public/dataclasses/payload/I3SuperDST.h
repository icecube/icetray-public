/**
 * @file 
 * @brief 
 *
 * (c) 2010 the IceCube Collaboration
 *
 * $Id:$
 * @version $Revision$
 * @date $Date$
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 */

#ifndef I3SUPERDST_I3SUPERDST_H_INCLUDED
#define I3SUPERDST_I3SUPERDST_H_INCLUDED

/* standard library stuff */
#include <map>
#include <vector>

/* IceTray stuff */
#include "icetray/I3FrameObject.h"
#include "icetray/IcetrayFwd.h"

#include "dataclasses/physics/I3RecoPulse.h"
#include "dataclasses/physics/I3TriggerHierarchy.h"
	 
#include "dataclasses/payload/I3SuperDSTTrigger.h"

/* Forward declarations of real classes (not typedef'd templates) */
class OMKey;
class I3EventHeader;
class I3Time;

static const unsigned i3superdst_version_ = 1;

namespace I3SuperDSTUtils {
	enum Discretization { LINEAR, LOG };
}

class I3SuperDSTChargeStamp {
public:
	enum LCType {HLC, SLC};
	
	I3SuperDSTChargeStamp(double time, double charge, double width, bool hlc,
	    I3SuperDSTUtils::Discretization format=I3SuperDSTUtils::LINEAR);
	I3SuperDSTChargeStamp(uint32_t timecode, uint32_t chargecode, uint32_t widthcode,
	    bool hlc, I3SuperDSTUtils::Discretization format=I3SuperDSTUtils::LINEAR, 
	    unsigned version=i3superdst_version_) : timecode_(timecode), widthcode_(widthcode),
	    chargecode_(chargecode), charge_overflow_(0),
	    version_(version), kind_(hlc ? HLC : SLC), charge_format_(format) {};
	
	double GetTime() const;
	double GetWidth() const;
	double GetCharge() const;
	
	uint32_t GetTimeCode() const { return timecode_; };
	uint32_t GetChargeCode() const { return chargecode_; };
	uint32_t GetWidthCode() const { return widthcode_; };
	uint32_t GetChargeOverflow() const { return charge_overflow_; };
	I3SuperDSTUtils::Discretization GetChargeFormat() const
	{ return charge_format_; };
	
	LCType GetKind() const { return kind_; };
	bool GetLCBit() const { return (kind_ == HLC); };
	
	void SetTimeReference(const I3SuperDSTChargeStamp &other);

  bool operator==(const I3SuperDSTChargeStamp& rhs) const;
  bool operator!=(const I3SuperDSTChargeStamp& rhs) const {
    return !(*this == rhs);
  }

private:
	uint32_t timecode_;
	uint32_t widthcode_;
	uint32_t chargecode_;
	uint32_t charge_overflow_;
	unsigned version_;
	LCType kind_;
	I3SuperDSTUtils::Discretization charge_format_;

	SET_LOGGER("I3SuperDST");
};

struct I3SuperDSTReadout {
	
	OMKey om_;
	double start_time_;
	std::vector<I3SuperDSTChargeStamp> stamps_;
	I3SuperDSTChargeStamp::LCType kind_;
	uint32_t time_overflow_;
	
	I3SuperDSTReadout(const OMKey &om, bool hlc,
	    const std::list<I3RecoPulse>::const_iterator &start, 
	    const std::list<I3RecoPulse>::const_iterator &end, double t0 = 0.0);
	I3SuperDSTReadout() : om_(), start_time_(0.0), stamps_(), kind_(),
	    time_overflow_(0) {};
	
	void SetTimeReference(const I3SuperDSTReadout &other);
	void Relativize();
	
	double GetTime() const;
	
	bool GetLCBit() const { return (kind_ == I3SuperDSTChargeStamp::HLC); };
	
	bool operator<(const I3SuperDSTReadout &other) const
	    { return start_time_ < other.start_time_; };

  bool operator==(const I3SuperDSTReadout& rhs) const;
  bool operator!=(const I3SuperDSTReadout& rhs) const {
    return !(*this == rhs);
  }
	
	SET_LOGGER("I3SuperDST");
};


class I3SuperDST : public I3FrameObject {
public:
	I3SuperDST() : version_(i3superdst_version_) { InitDebug(); };
	
	/** 
	 * Create a compressed representation of an event from a pulse map.
	 *
	 * @param pulses	Map containing pulses extracted from all
	 *                      InIce and IceTop launches.
	 * 
	 * @see Unpack()
	 */
	I3SuperDST(const I3RecoPulseSeriesMap &pulses);
	
	/**
	 * Expand charge stamps into fake I3RecoPulses, packing everything into
	 * a single map.
	 */
	I3RecoPulseSeriesMapConstPtr Unpack() const;
	
	I3MapKeyVectorInt GetEncodedSizes() const;
	
	std::list<I3SuperDSTReadout> GetHLCReadouts() const
	    { return GetReadouts(true); };
	std::list<I3SuperDSTReadout> GetSLCReadouts() const
	    { return GetReadouts(false); };

#ifndef NDEBUG
	double *serialization_time_;
	unsigned *serialization_counter_;
	void InitDebug() { serialization_time_ = NULL; serialization_counter_ = NULL; };
#else
	void InitDebug() {};
#endif
		
	static uint32_t EncodeOMKey(const OMKey &key, unsigned int maxbits=16,
	    unsigned int version=i3superdst_version_);
	static OMKey DecodeOMKey(uint32_t number,
	    unsigned int version=i3superdst_version_);
	
	static uint32_t EncodeTime(double time, unsigned int maxbits=16,
	    unsigned int version=i3superdst_version_);
	static double DecodeTime(uint32_t dt,
	    unsigned int version=i3superdst_version_);
	static uint32_t EncodeWidth(double width, unsigned int maxbits=16,
	    unsigned int version=i3superdst_version_);
	static double DecodeWidth(uint32_t code,
	    unsigned int version=i3superdst_version_);
	
	static uint32_t EncodeCharge(double charge, unsigned int maxbits=16,
	    unsigned int version=i3superdst_version_,
	    I3SuperDSTUtils::Discretization mode=I3SuperDSTUtils::LINEAR);
	static double DecodeCharge(uint32_t logcharge,
	    unsigned int version=i3superdst_version_,
	    I3SuperDSTUtils::Discretization mode=I3SuperDSTUtils::LINEAR);
	
	static double FindStartTime(const I3RecoPulseSeriesMap &pmap);

  bool operator==(const I3SuperDST& rhs) const;
  bool operator!=(const I3SuperDST& rhs) const {
    return !(*this == rhs);
  }

private:
	std::list<I3SuperDSTReadout> readouts_;
	mutable I3RecoPulseSeriesMapPtr unpacked_;
		
	void AddPulseMap(const I3RecoPulseSeriesMap &pulses, double t0);
	void AddPulseMap(const I3RecoPulseSeriesMap &pulses);
	std::list<I3SuperDSTReadout> GetReadouts(bool hlc) const;
	
	static const double tmin_;
	
	unsigned version_;
	
	friend class icecube::serialization::access;

	template <class Archive> void save(Archive & ar, unsigned version) const;
	template <class Archive> void save(Archive & ar, unsigned version,
	    std::map<OMKey, std::vector<int > > *sizes) const;
	template <class Archive> void load(Archive & ar, unsigned version);
	template <class Archive> void load_v0(Archive & ar);
	template <class Archive> void load_v1(Archive & ar);    
	I3_SERIALIZATION_SPLIT_MEMBER();
	
	SET_LOGGER("I3SuperDST");
};


I3_CLASS_VERSION(I3SuperDST, i3superdst_version_);

I3_POINTER_TYPEDEFS(I3SuperDST);

#endif
