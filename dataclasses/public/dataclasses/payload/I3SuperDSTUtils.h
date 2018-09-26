/**
 * @file 
 * @brief 
 *
 * (c) 2011 Jakob van Santen 
 *     and the IceCube Collaboration
 *
 * $Id$
 * @version $Revision$
 * @date $Date$
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 */

#ifndef I3SUPERDST_I3SUPERDSTUTILS_H_INCLUDED
#define I3SUPERDST_I3SUPERDSTUTILS_H_INCLUDED

#include <serialization/binary_object.hpp>

namespace I3SuperDSTUtils {
	/*
	 * Variable-width integer encoding.
	 *
	 * Values up to 246 are encoded in 1 byte; larger values in the
	 * minimum required to represent the value, plus 1.
	 */
	struct SizeCodec {
		typedef uint64_t size_type;
		size_type size_;
		
		SizeCodec() : size_(0) {};
		SizeCodec(size_type size) : size_(size) {};
		
		size_type value() { return size_; };
		
		friend class icecube::serialization::access;
		template <class Archive>
		void load(Archive &ar, unsigned version);
		template <class Archive>
		void save(Archive &ar, unsigned version) const;
		I3_SERIALIZATION_SPLIT_MEMBER();
	};

	/* Optimized serialization for short vectors */
	template <typename T>
	class CompactVector : public std::vector<T> {
	private:
		friend class icecube::serialization::access;
		template <class Archive>
		void load(Archive &ar, unsigned version) {
			SizeCodec c;
			ar >> make_nvp("Size", c);
			this->resize(c.size_);
			ar >> make_nvp("Contents",
			    icecube::serialization::make_binary_object(&this->front(),
			    this->size()*sizeof(T)));
		}
		template <class Archive>
		void save(Archive &ar, unsigned version) const
		{
			SizeCodec c(this->size());
			ar << make_nvp("Size", c);
			ar << make_nvp("Contents",
			    icecube::serialization::make_binary_object((void*)(&this->front()),
			    this->size()*sizeof(T)));
		}
		I3_SERIALIZATION_SPLIT_MEMBER();
	};

	/*
	 * Hobo run-length encoding for 4-bit numbers.
	 *
	 * Runs of 4-bit codes are encoded in single-byte instructions.
	 * The lower 4 bits hold the code, and the upper 4 bits hold
	 * a run length. If successive bytes contain the same code, then
	 * the run lengths are concatenated to form a wider number. This
	 * way, a run of 16 fits in 1 byte, a run of 256 in 2 bytes,
	 * 4096 in 3, etc.
	 *
	 * As with any run-length encoding, this does worst when there
	 * are no runs to encode. In the worst case where the code
	 * switches at every element, this encoding exactly doubles
	 * the size of the data.
	 */ 
	struct RunCodec {
		typedef std::vector<uint8_t> vector_t;

		static void EncodeRun(vector_t &codes, uint8_t val, unsigned len);
		static void DecodeRun(vector_t &target,
		    const vector_t::const_iterator &head,
		    const vector_t::const_iterator &tail);

		static void Encode(const vector_t &runs, vector_t &codes);
		static void Decode(const vector_t &codes, vector_t &runs);
	};

	inline int findlastset(uint32_t i)
	{
		return i == 0 ? 0 : (8*sizeof(i)-__builtin_clz(i));
	}
	
	inline int findlastset(uint64_t i)
	{
		return (i & 0xffffffff00000000LL) ? findlastset(uint32_t(i >> 32))+32
		    : findlastset(uint32_t(i & 0xffffffff));
	}
}

#endif

