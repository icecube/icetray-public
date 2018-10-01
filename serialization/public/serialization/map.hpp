#ifndef I3_SERIALIZATION_MAP_HPP
#define I3_SERIALIZATION_MAP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization/map.hpp:
// serialization for stl map templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <map>

#include <boost/config.hpp>

#include <serialization/utility.hpp>
#include <serialization/collections_save_imp.hpp>
#include <serialization/collections_load_imp.hpp>
#include <serialization/split_free.hpp>

namespace icecube {
namespace serialization {

template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::map<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    icecube::serialization::stl::save_collection<
        Archive,
        std::map<Key, Type, Compare, Allocator>
    >(ar, t);
}

template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::map<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    icecube::serialization::stl::load_collection<
        Archive,
        std::map<Key, Type, Compare, Allocator>,
        icecube::serialization::stl::archive_input_map<
            Archive, std::map<Key, Type, Compare, Allocator> >,
            icecube::serialization::stl::no_reserve_imp<std::map<
                Key, Type, Compare, Allocator
            >
        >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::map<Key, Type, Compare, Allocator> &t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

// multimap
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::multimap<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    icecube::serialization::stl::save_collection<
        Archive,
        std::multimap<Key, Type, Compare, Allocator>
    >(ar, t);
}

template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::multimap<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    icecube::serialization::stl::load_collection<
        Archive,
        std::multimap<Key, Type, Compare, Allocator>,
        icecube::serialization::stl::archive_input_multimap<
            Archive, std::multimap<Key, Type, Compare, Allocator>
        >,
        icecube::serialization::stl::no_reserve_imp<
            std::multimap<Key, Type, Compare, Allocator>
        >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::multimap<Key, Type, Compare, Allocator> &t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

} // serialization
} // namespace icecube

#endif // I3_SERIALIZATION_MAP_HPP
