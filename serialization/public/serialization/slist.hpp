#ifndef I3_SERIALIZATION_SLIST_HPP
#define I3_SERIALIZATION_SLIST_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// slist.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // size_t
#include <boost/config.hpp> // msvc 6.0 needs this for warning suppression
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::size_t; 
} // namespace std
#endif

#ifdef BOOST_HAS_SLIST
#include BOOST_SLIST_HEADER

#include <serialization/collections_save_imp.hpp>
#include <serialization/collections_load_imp.hpp>
#include <serialization/split_free.hpp>
#include <serialization/nvp.hpp>
#include <archive/basic_archive.hpp>

namespace icecube { 
namespace serialization {

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const BOOST_STD_EXTENSION_NAMESPACE::slist<U, Allocator> &t,
    const unsigned int file_version
){
    icecube::serialization::stl::save_collection<
        Archive,
        BOOST_STD_EXTENSION_NAMESPACE::slist<U, Allocator> 
    >(ar, t);
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    BOOST_STD_EXTENSION_NAMESPACE::slist<U, Allocator> &t,
    const unsigned int file_version
){
    // retrieve number of elements
    t.clear();
    // retrieve number of elements
    collection_size_type count;
    ar >> I3_SERIALIZATION_NVP(count);
    if(collection_size_type(0) == count)
        return;
    item_version_type item_version(0);
    //const icecube::archive::library_version_type library_version(
    //    ar.get_library_version()
    //);
    //if(icecube::archive::library_version_type(3) < library_version){
    //    ar >> I3_SERIALIZATION_NVP(item_version);
    //}
    icecube::serialization::detail::stack_construct<Archive, U> u(ar, item_version);
    ar >> icecube::serialization::make_nvp("item", u.reference());
    t.push_front(u.reference());
    typename BOOST_STD_EXTENSION_NAMESPACE::slist<U, Allocator>::iterator last;
    last = t.begin();
    while(--count > 0){
        icecube::serialization::detail::stack_construct<Archive, U>
            u(ar, file_version);
        ar >> icecube::serialization::make_nvp("item", u.reference());
        last = t.insert_after(last, u.reference());
        ar.reset_object_address(& (*last), & u.reference());
    }
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class U, class Allocator>
inline void serialize(
    Archive & ar,
    BOOST_STD_EXTENSION_NAMESPACE::slist<U, Allocator> &t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

} // serialization
} // namespace icecube

#include <serialization/collection_traits.hpp>

I3_SERIALIZATION_COLLECTION_TRAITS(BOOST_STD_EXTENSION_NAMESPACE::slist)

#endif  // BOOST_HAS_SLIST
#endif  // I3_SERIALIZATION_SLIST_HPP
