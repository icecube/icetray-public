/*!
 * \file      dynamic_bitset.hpp
 * \brief     Provides Boost.Serialization support for boost::dynamic_bitset
 * \author    David Schultz
 * \author    Chris Weaver
 * \license   Boost Software License 1.0
 */
#ifndef I3_SERIALIZATION_DYNAMIC_BITSET_HPP
#define I3_SERIALIZATION_DYNAMIC_BITSET_HPP

#include <bitset>
#include <cstddef> // size_t
#include <cstdint> // uintX_t

#include <boost/config.hpp>
#include <serialization/split_free.hpp>
#include <serialization/nvp.hpp>
#include <serialization/vector.hpp>

#include <boost/dynamic_bitset.hpp>

namespace icecube{
namespace serialization{

template <class Archive>
inline void save(
    Archive & ar,
    boost::dynamic_bitset<> const & t,
    const unsigned int /* version */
){
    std::size_t size(t.size());
    std::vector<boost::dynamic_bitset<>::block_type> vec(t.num_blocks());
    to_block_range(t, vec.begin());
    ar & icecube::serialization::make_nvp("size",size);
    ar & icecube::serialization::make_nvp("data",vec);
}

template <class Archive>
inline void load(
    Archive & ar,
    boost::dynamic_bitset<> & t,
    const unsigned int /* version */
)
{
    std::size_t size(0);
    std::vector<boost::dynamic_bitset<>::block_type> vec;
    ar & icecube::serialization::make_nvp("size",size);
    ar & icecube::serialization::make_nvp("data",vec);
    t.resize(size);
    assert(t.num_blocks() >= vec.size());
    from_block_range(vec.begin(), vec.end(), t);
}

template <class Archive>
inline void serialize(
    Archive & ar,
    boost::dynamic_bitset<> & t,
    const unsigned int version
){
    icecube::serialization::split_free( ar, t, version );
}

// don't track bitsets and waste space
template<>
struct tracking_level<boost::dynamic_bitset<> >
    : boost::mpl::int_<track_never> {} ;

} //serialization
} //icecube

#endif // I3_SERIALIZATION_DYNAMIC_BITSET_HPP
