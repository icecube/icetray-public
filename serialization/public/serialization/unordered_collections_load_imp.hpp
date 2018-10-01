#ifndef I3_SERIALIZATION_UNORDERED_COLLECTIONS_LOAD_IMP_HPP
#define I3_SERIALIZATION_UNORDERED_COLLECTIONS_LOAD_IMP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
# pragma warning (disable : 4786) // too long name, harmless warning
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// unordered_collections_load_imp.hpp: serialization for loading stl collections

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// (C) Copyright 2014 Jim Bell
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// helper function templates for serialization of collections

#include <boost/assert.hpp>
#include <cstddef> // size_t
#include <boost/config.hpp> // msvc 6.0 needs this for warning suppression
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::size_t; 
} // namespace std
#endif
#include <boost/detail/workaround.hpp>

#include <archive/detail/basic_iarchive.hpp>
#include <serialization/access.hpp>
#include <serialization/nvp.hpp>
#include <serialization/detail/stack_constructor.hpp>
#include <serialization/collection_size_type.hpp>
#include <serialization/item_version_type.hpp>

namespace icecube{
namespace serialization {
namespace stl {

//////////////////////////////////////////////////////////////////////
// implementation of serialization for STL containers
//
template<class Archive, class Container, class InputFunction>
inline void load_unordered_collection(Archive & ar, Container &s)
{
    s.clear();
    collection_size_type count;
    collection_size_type bucket_count;
    icecube::serialization::item_version_type item_version(0);
    icecube::archive::library_version_type library_version(
        ar.get_library_version()
    );
    // retrieve number of elements
    ar >> I3_SERIALIZATION_NVP(count);
    ar >> I3_SERIALIZATION_NVP(bucket_count);
    if(icecube::archive::library_version_type(3) < library_version){
        ar >> I3_SERIALIZATION_NVP(item_version);
    }
    s.rehash(bucket_count);
    InputFunction ifunc;
    while(count-- > 0){
        ifunc(ar, s, item_version);
    }
}

} // namespace stl 
} // namespace serialization
} // namespace icecube

#endif //I3_SERIALIZATION_UNORDERED_COLLECTIONS_LOAD_IMP_HPP
