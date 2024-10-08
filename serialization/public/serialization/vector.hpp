#ifndef I3_SERIALIZATION_VECTOR_HPP
#define I3_SERIALIZATION_VECTOR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector.hpp: serialization for stl vector templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// fast array serialization (C) Copyright 2005 Matthias Troyer
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <vector>

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <serialization/collections_save_imp.hpp>
#include <serialization/collections_load_imp.hpp>
#include <serialization/split_free.hpp>
#include <serialization/array.hpp>
#include <serialization/detail/get_data.hpp>
#include <boost/mpl/bool.hpp>

// default is being compatible with version 1.34.1 files, not 1.35 files
#ifndef I3_SERIALIZATION_VECTOR_VERSION
#define I3_SERIALIZATION_VECTOR_VERSION 3
#endif


namespace icecube {
namespace serialization {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector<T>

// the default versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    boost::mpl::false_
){
    icecube::serialization::stl::save_collection<Archive, std::vector<U, Allocator> >(
        ar, t
    );
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    boost::mpl::false_
){
    icecube::serialization::stl::load_collection<
        Archive,
        std::vector<U, Allocator>,
        icecube::serialization::stl::archive_input_seq<
            Archive, std::vector<U, Allocator>
        >,
        icecube::serialization::stl::reserve_imp<std::vector<U, Allocator> >
    >(ar, t);
}

// the optimized versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    boost::mpl::true_
){
    const collection_size_type count(t.size());
    ar << I3_SERIALIZATION_NVP(count);
    /* I3_HACK
    if(I3_SERIALIZATION_VECTOR_VERSION < ar.get_library_version()) {
      const unsigned int item_version = version<U>::value;
      ar << I3_SERIALIZATION_NVP(item_version);
    }
    */
   if (!t.empty())
      ar << make_array(detail::get_data(t),t.size());
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int /* file_version */,
    boost::mpl::true_
){
    collection_size_type count(t.size());
    ar >> I3_SERIALIZATION_NVP(count);
    t.resize(count);
    /* I3_HACK
    unsigned int item_version=0;
    if(I3_SERIALIZATION_VECTOR_VERSION < ar.get_library_version())
        ar >> I3_SERIALIZATION_NVP(item_version);
    */
    if (!t.empty())
      ar >> make_array(detail::get_data(t),t.size());
  }

// dispatch to either default or optimized versions

template<class Archive, class U, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<U, Allocator> &t,
    const unsigned int file_version
){
    save(ar,t,file_version, BOOST_DEDUCED_TYPENAME use_array_optimization<Archive>::template apply<U>::type());
}

template<class Archive, class U, class Allocator>
inline void load(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int file_version
){
    load(ar,t,file_version, BOOST_DEDUCED_TYPENAME use_array_optimization<Archive>::template apply<U>::type());
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class U, class Allocator>
inline void serialize(
    Archive & ar,
    std::vector<U, Allocator> & t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

#if ! BOOST_WORKAROUND(BOOST_MSVC, <= 1300)

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector<bool>
template<class Archive, class Allocator>
inline void save(
    Archive & ar,
    const std::vector<bool, Allocator> &t,
    const unsigned int /* file_version */
){
    // record number of elements
    collection_size_type count (t.size());
    ar << I3_SERIALIZATION_NVP(count);
    std::vector<bool>::const_iterator it = t.begin();
    while(count-- > 0){
        bool tb = *it++;
        ar << icecube::serialization::make_nvp("item", tb);
    }
}

template<class Archive, class Allocator>
inline void load(
    Archive & ar,
    std::vector<bool, Allocator> &t,
    const unsigned int /* file_version */
){
    // retrieve number of elements
    collection_size_type count;
    ar >> I3_SERIALIZATION_NVP(count);
    t.clear();
    while(count-- > 0){
        bool i;
        ar >> icecube::serialization::make_nvp("item", i);
        t.push_back(i);
    }
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Allocator>
inline void serialize(
    Archive & ar,
    std::vector<bool, Allocator> & t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

#endif // BOOST_WORKAROUND

} // serialization
} // namespace icecube

#include <serialization/collection_traits.hpp>

I3_SERIALIZATION_COLLECTION_TRAITS(std::vector)

#endif // I3_SERIALIZATION_VECTOR_HPP
