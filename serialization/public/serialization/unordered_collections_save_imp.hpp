#ifndef I3_SERIALIZATION_UNORDERED_COLLECTIONS_SAVE_IMP_HPP
#define I3_SERIALIZATION_UNORDERED_COLLECTIONS_SAVE_IMP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// hash_collections_save_imp.hpp: serialization for stl collections

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// (C) Copyright 2014 Jim Bell
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// helper function templates for serialization of collections

#include <boost/config.hpp>
#include <serialization/nvp.hpp>
#include <serialization/serialization.hpp>
#include <serialization/version.hpp>
#include <serialization/collection_size_type.hpp>
#include <serialization/item_version_type.hpp>

namespace icecube{
namespace serialization {
namespace stl {

//////////////////////////////////////////////////////////////////////
// implementation of serialization for STL containers
//

template<class Archive, class Container>
inline void save_unordered_collection(Archive & ar, const Container &s)
{
    collection_size_type count(s.size());
    const collection_size_type bucket_count(s.bucket_count());
    const item_version_type item_version(
        version<typename Container::value_type>::value
    );

    #if 0
    /* should only be necessary to create archives of previous versions
     * which is not currently supported.  So for now comment this out
     */
    icecube::archive::library_version_type library_version(
        ar.get_library_version()
    );
    // retrieve number of elements
	ar << I3_SERIALIZATION_NVP(count);
	ar << I3_SERIALIZATION_NVP(bucket_count);
    if(icecube::archive::library_version_type(3) < library_version){
        // record number of elements
        // make sure the target type is registered so we can retrieve
        // the version when we load
        ar << I3_SERIALIZATION_NVP(item_version);
    }
    #else
        ar << I3_SERIALIZATION_NVP(count);
        ar << I3_SERIALIZATION_NVP(bucket_count);
        ar << I3_SERIALIZATION_NVP(item_version);
    #endif

    typename Container::const_iterator it = s.begin();
    while(count-- > 0){
        // note borland emits a no-op without the explicit namespace
        icecube::serialization::save_construct_data_adl(
            ar,
            &(*it),
            icecube::serialization::version<
                typename Container::value_type
            >::value
        );
        ar << icecube::serialization::make_nvp("item", *it++);
    }
}

} // namespace stl
} // namespace serialization
} // namespace icecube

#endif //I3_SERIALIZATION_UNORDERED_COLLECTIONS_SAVE_IMP_HPP
