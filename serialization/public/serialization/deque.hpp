#ifndef I3_SERIALIZATION_DEQUE_HPP
#define I3_SERIALIZATION_DEQUE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// deque.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <deque>

#include <boost/config.hpp>

#include <serialization/collections_save_imp.hpp>
#include <serialization/collections_load_imp.hpp>
#include <serialization/split_free.hpp>

namespace icecube { 
namespace serialization {

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::deque<U, Allocator> &t,
    const unsigned int /* file_version */
){
    icecube::serialization::stl::save_collection<
        Archive, std::deque<U, Allocator> 
    >(ar, t);
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::deque<U, Allocator> &t,
    const unsigned int /*file_version*/
){
    icecube::serialization::stl::load_collection<
        Archive,
        std::deque<U, Allocator>,
        icecube::serialization::stl::archive_input_seq<
        Archive, std::deque<U, Allocator> 
        >,
        icecube::serialization::stl::no_reserve_imp<std::deque<U, Allocator> >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class U, class Allocator>
inline void serialize(
    Archive & ar,
    std::deque<U, Allocator> &t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace icecube

#include <serialization/collection_traits.hpp>

I3_SERIALIZATION_COLLECTION_TRAITS(std::deque)

#endif // I3_SERIALIZATION_DEQUE_HPP
