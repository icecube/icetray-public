/**
 *  $Id$
 *  
 *  Copyright (C) 2011
 *  Jakob van Santen <vansanten@wisc.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 */

#ifndef DATACLASSES_I3MAPOMKEYMASK_H_INCLUDED
#define DATACLASSES_I3MAPOMKEYMASK_H_INCLUDED

#include <functional>
#include <string>
#include <list>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/dynamic_bitset.hpp> 

#include "icetray/I3FrameObject.h"
#include "icetray/OMKey.h"
#include "icetray/I3Frame.h"
#include "icetray/serialization.h"
#include "dataclasses/physics/I3RecoPulse.h"

static const unsigned i3recopulseseriesmapmask_version_ = 2;

class I3RecoPulseSeriesMapMask : public I3FrameObject {
public:
	/* 
	 * Construct a mask for the map stored at "key." All bits are set.
	 */
	I3RecoPulseSeriesMapMask(const I3Frame&, const std::string &key);
	/*
	 * Construct a mask for the map stored at "key," corresponding to
	 * the map `subset.' The subset must be an ordered subset of the map
	 * stored at `key'.
	 */
	I3RecoPulseSeriesMapMask(const I3Frame&, const std::string &key,
	    const I3RecoPulseSeriesMap &subset);
	/*
	 * Construct a mask by predicate. This may be either a free function
	 * or an instance of a class that implements
	 * bool operator()(const OMKey&, size_t, const I3RecoPulse&).
	 */
 	I3RecoPulseSeriesMapMask(const I3Frame&, const std::string &key,
 	    boost::function<bool (const OMKey&, size_t, const I3RecoPulse&)> predicate);
	
	I3RecoPulseSeriesMapMask();
	 
	std::ostream& Print(std::ostream&) const override;
  
	/*
	 * Set/unset all elements for an OMKey.
	 */
	void Set(const OMKey&, bool);
	/*
	 * Set/unset an element for OMKey by index.
	 */
	void Set(const OMKey&, const unsigned idx, bool);
	/*
	 * Set/unset an element for OMKey by value.
	 */
	void Set(const OMKey&, const I3RecoPulse&, bool);
	/*
	 * Clear all elements of the mask.
	 */
	void SetNone();
	
	/*
	 * Apply the mask to the target map in the frame.
	 */
	boost::shared_ptr<const I3RecoPulseSeriesMap> Apply(const I3Frame &frame) const;
	
	/**
	 * Return true if this mask is derived from key
	 */
	bool HasAncestor(const I3Frame &frame, const std::string &key) const;
	
	/**
	 * Convert this mask into a form that can be applied directly to key
	 * without resort to intermediate masks.
	 */
	boost::shared_ptr<I3RecoPulseSeriesMapMask> Repoint(const I3Frame &frame, const std::string &key) const;
	
	/*
	 * Get the name of the frame object the mask was made from.
	 */
	std::string GetSource() const { return key_; }
	 
	/*
	 * Get the number of set bits in the mask.
	 */
	unsigned GetSum() const;
	
	/*
	 * Are any bits set?
	 */
	bool GetAnySet() const;
	
	/*
	 * Are all bits set?
	 */
	bool GetAllSet() const;
	 
	 
	std::vector<boost::dynamic_bitset<uint8_t> > GetBits() const;
	
	/*
	 * Logical operators, applied elementwise.
	 */
	I3RecoPulseSeriesMapMask operator&(const I3RecoPulseSeriesMapMask&) const;
	I3RecoPulseSeriesMapMask operator|(const I3RecoPulseSeriesMapMask&) const;
	I3RecoPulseSeriesMapMask operator^(const I3RecoPulseSeriesMapMask&) const;
	/** Equivalent to this & ~other */
	I3RecoPulseSeriesMapMask Remove(const I3RecoPulseSeriesMapMask&) const;
	
	I3RecoPulseSeriesMapMask& operator&=(const I3RecoPulseSeriesMapMask&);
	I3RecoPulseSeriesMapMask& operator|=(const I3RecoPulseSeriesMapMask&);
	I3RecoPulseSeriesMapMask& operator^=(const I3RecoPulseSeriesMapMask&);
		
	bool operator==(const I3RecoPulseSeriesMapMask&) const;
	bool operator!=(const I3RecoPulseSeriesMapMask&) const;
	
private:
	typedef uint8_t mask_t;
	
	struct bitmask {
		uint16_t size_;
		uint8_t padding_;
		mask_t *mask_;
		
		bitmask() : size_(0), padding_(0), mask_(NULL) {};
		bitmask(unsigned length, bool set=true);
		bitmask(const bitmask& other);
		bitmask& operator=(const bitmask& other);
		~bitmask();
		void set_all();
		void unset_all();
		inline bool any() const;
		inline bool all() const;
		inline void set(const unsigned, bool);
		
		inline bool get(const unsigned) const;
		unsigned sum() const;
		size_t size() const;
		
		bool operator==(const bitmask&) const;
		
		friend class icecube::serialization::access;
		
		template <class Archive> void load(Archive & ar, unsigned version);
		template <class Archive> void save(Archive & ar, unsigned version) const;
		
		I3_SERIALIZATION_SPLIT_MEMBER();
	};
	
	friend std::ostream& operator<<(std::ostream&, const bitmask&);
		
	std::string key_;
	bitmask omkey_mask_;
	std::list<bitmask> element_masks_;
	I3RecoPulseSeriesMapConstPtr source_;
	mutable I3RecoPulseSeriesMapPtr masked_;
	
	inline void ResetCache() { masked_.reset(); }

	int FindKey(const OMKey &key, std::list<bitmask>::iterator &list_it,
	    const I3RecoPulseSeriesMap::mapped_type **vec);
	
	static bool IsOrderedSubset(const I3RecoPulseSeriesMap&, const I3RecoPulseSeriesMap&);
	static void FillSubsetMask(bitmask&, const I3RecoPulseSeriesMap::mapped_type&,
	    const I3RecoPulseSeriesMap::mapped_type&);
	
	/**
	 * Collapse this mask with its source, making it depend only on its grandparent.
	 */
	boost::shared_ptr<I3RecoPulseSeriesMapMask> CollapseLevel(const I3Frame &frame) const;
	
	template <typename BinaryOperator>
	I3RecoPulseSeriesMapMask ApplyBinaryOperator(const I3RecoPulseSeriesMapMask&) const;
	
	struct operator_and : public std::binary_function<mask_t, mask_t, mask_t> {
		inline mask_t operator()(mask_t lhs, mask_t rhs) { return lhs & rhs; }
	};
	
	struct operator_andnot : public std::binary_function<mask_t, mask_t, mask_t> {
		inline mask_t operator()(mask_t lhs, mask_t rhs) { return lhs & ~rhs; }
	};
	
	struct operator_or : public std::binary_function<mask_t, mask_t, mask_t> {
		inline mask_t operator()(mask_t lhs, mask_t rhs) { return lhs | rhs; }
	};
	
	struct operator_xor : public std::binary_function<mask_t, mask_t, mask_t> {
		inline mask_t operator()(mask_t lhs, mask_t rhs) { return lhs ^ rhs; }
	};
	
	friend class icecube::serialization::access;
	template <class Archive> void load(Archive & ar, unsigned version);
	template <class Archive> void save(Archive & ar, unsigned version) const;
	
	I3_SERIALIZATION_SPLIT_MEMBER();
	
	SET_LOGGER("I3RecoPulseSeriesMapMask");
};

std::ostream& operator<<(std::ostream&, const I3RecoPulseSeriesMapMask&);

template<> void I3RecoPulseSeriesMapMask::bitmask::load(icecube::archive::xml_iarchive& ar, unsigned version);
template<> void I3RecoPulseSeriesMapMask::bitmask::save(icecube::archive::xml_oarchive& ar, unsigned version) const;

I3_CLASS_VERSION(I3RecoPulseSeriesMapMask, i3recopulseseriesmapmask_version_);
I3_POINTER_TYPEDEFS(I3RecoPulseSeriesMapMask);

#endif /* DATACLASSES_I3MAPOMKEYMASK_H_INCLUDED */

