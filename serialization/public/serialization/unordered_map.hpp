#ifndef I3_SERIALIZATION_UNORDERED_MAP_HPP
#define I3_SERIALIZATION_UNORDERED_MAP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization/unordered_map.hpp:
// serialization for stl unordered_map templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// (C) Copyright 2014 Jim Bell
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>

#include <unordered_map>

#include <serialization/utility.hpp>
#include <serialization/unordered_collections_save_imp.hpp>
#include <serialization/unordered_collections_load_imp.hpp>
#include <serialization/split_free.hpp>

namespace icecube {
namespace serialization {

namespace stl {

// map input
template<class Archive, class Container>
struct archive_input_unordered_map
{
    inline void operator()(
        Archive &ar,
        Container &s,
        const unsigned int v
    ){
        typedef typename Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, v);
        // borland fails silently w/o full namespace
        ar >> icecube::serialization::make_nvp("item", t.reference());
        std::pair<typename Container::const_iterator, bool> result =
            s.insert(t.reference());
        // note: the following presumes that the map::value_type was NOT tracked
        // in the archive.  This is the usual case, but here there is no way
        // to determine that.
        if(result.second){
            ar.reset_object_address(
                & (result.first->second),
                & t.reference().second
            );
        }
    }
};

// multimap input
template<class Archive, class Container>
struct archive_input_unordered_multimap
{
    inline void operator()(
        Archive &ar,
        Container &s,
        const unsigned int v
    ){
        typedef typename Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, v);
        // borland fails silently w/o full namespace
        ar >> icecube::serialization::make_nvp("item", t.reference());
        typename Container::const_iterator result
            = s.insert(t.reference());
        // note: the following presumes that the map::value_type was NOT tracked
        // in the archive.  This is the usual case, but here there is no way
        // to determine that.
        ar.reset_object_address(
            & result->second,
            & t.reference()
        );
    }
};

} // stl

template<
    class Archive,
    class Key,
    class HashFcn,
    class EqualKey,
    class Allocator
>
inline void save(
    Archive & ar,
    const std::unordered_map<
        Key, HashFcn, EqualKey, Allocator
    > &t,
    const unsigned int /*file_version*/
){
    icecube::serialization::stl::save_unordered_collection<
        Archive,
        std::unordered_map<
            Key, HashFcn, EqualKey, Allocator
        >
    >(ar, t);
}

template<
    class Archive,
    class Key,
    class HashFcn,
    class EqualKey,
    class Allocator
>
inline void load(
    Archive & ar,
    std::unordered_map<
        Key, HashFcn, EqualKey, Allocator
    > &t,
    const unsigned int /*file_version*/
){
    icecube::serialization::stl::load_unordered_collection<
        Archive,
        std::unordered_map<
            Key, HashFcn, EqualKey, Allocator
        >,
        icecube::serialization::stl::archive_input_unordered_map<
            Archive,
            std::unordered_map<
                Key, HashFcn, EqualKey, Allocator
            >
        >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<
    class Archive,
    class Key,
    class HashFcn,
    class EqualKey,
    class Allocator
>
inline void serialize(
    Archive & ar,
    std::unordered_map<
        Key, HashFcn, EqualKey, Allocator
    > &t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

// unordered_multimap
template<
    class Archive,
    class Key,
    class HashFcn,
    class EqualKey,
    class Allocator
>
inline void save(
    Archive & ar,
    const std::unordered_multimap<
        Key, HashFcn, EqualKey, Allocator
    > &t,
    const unsigned int /*file_version*/
){
    icecube::serialization::stl::save_unordered_collection<
        Archive,
        std::unordered_multimap<
            Key, HashFcn, EqualKey, Allocator
        >
    >(ar, t);
}

template<
    class Archive,
    class Key,
    class HashFcn,
    class EqualKey,
    class Allocator
>
inline void load(
    Archive & ar,
    std::unordered_multimap<
        Key, HashFcn, EqualKey, Allocator
    > &t,
    const unsigned int /*file_version*/
){
    icecube::serialization::stl::load_unordered_collection<
        Archive,
        std::unordered_multimap<
            Key, HashFcn, EqualKey, Allocator
        >,
        icecube::serialization::stl::archive_input_unordered_multimap<
            Archive,
            std::unordered_multimap<
                Key, HashFcn, EqualKey, Allocator
            >
        >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<
    class Archive,
    class Key,
    class HashFcn,
    class EqualKey,
    class Allocator
>
inline void serialize(
    Archive & ar,
    std::unordered_multimap<
        Key, HashFcn, EqualKey, Allocator
    > &t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace icecube

#endif // I3_SERIALIZATION_UNORDERED_MAP_HPP
