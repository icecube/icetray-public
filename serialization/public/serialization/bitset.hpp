/*!
 * \file      bitset.hpp
 * \brief     Provides Boost.Serialization support for std::bitset
 * \author    David Schultz
 * \author    Chris Weaver
 * \license   Boost Software License 1.0
 */
#ifndef I3_SERIALIZATION_BITSET_HPP
#define I3_SERIALIZATION_BITSET_HPP

#include <bitset>
#include <cstddef> // size_t
#include <cstdint> // uintX_t

#include <boost/config.hpp>
#include <serialization/split_free.hpp>
#include <serialization/nvp.hpp>

namespace icecube{
namespace serialization{

template <class Archive, std::size_t size>
inline void save(
    Archive & ar,
    std::bitset<size> const & t,
    const unsigned int /* version */
){
    if (size <= 8){
        uint8_t b(t.to_ulong()&0xFF);
        ar << I3_SERIALIZATION_NVP(b);
    }
    else if (size <= 32){
        uint32_t b(t.to_ulong()&0xFFFFFFFF);
        ar << I3_SERIALIZATION_NVP(b);
    }
    else{
        const size_t chunk_size(8*sizeof(uint64_t));
        const size_t chunks(size/chunk_size+(size%chunk_size==0?0:1));
        const std::bitset<size> mask((uint64_t)(-1));
        uint64_t chunk = 0;
        
        for (size_t i=0;i<chunks;i++) {
            chunk = ((t >> i*chunk_size) & mask).to_ulong();
            ar << I3_SERIALIZATION_NVP(chunk);
        }
    }
}

template <class Archive, std::size_t size>
inline void load(
    Archive & ar,
    std::bitset<size> & t,
    const unsigned int /* version */
){
    if (size <= 8){
        uint8_t b(0);
        ar >> I3_SERIALIZATION_NVP(b);
        t |= std::bitset<size>(b);
    }
    else if (size <= 32){
        uint32_t b(0);
        ar >> I3_SERIALIZATION_NVP(b);
        t |= std::bitset<size>(b);
    }
    else{
        const size_t chunk_size(8*sizeof(uint64_t));
        const size_t chunks(size/chunk_size+(size%chunk_size==0?0:1));
        uint64_t chunk;
        
        for (size_t i=0;i<chunks;i++) {
            ar >> I3_SERIALIZATION_NVP(chunk);
            t |= std::bitset<size>(chunk) << i*chunk_size;
        }
    }
}

template <class Archive, std::size_t size>
inline void serialize(
    Archive & ar,
    std::bitset<size> & t,
    const unsigned int version
){
    icecube::serialization::split_free( ar, t, version );
}

// don't track bitsets and waste space
template <std::size_t size>
struct tracking_level<std::bitset<size> >
    : boost::mpl::int_<track_never> {} ;

} //serialization
} //icecube

#endif // I3_SERIALIZATION_BITSET_HPP
