#ifndef I3_SERIALIZATION_HASH_COLLECTIONS_SAVE_IMP_HPP
#define I3_SERIALIZATION_HASH_COLLECTIONS_SAVE_IMP_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// hash_collections_save_imp.hpp: serialization for stl collections

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
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
inline void save_hash_collection(Archive & ar, const Container &s)
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
    if(icecube::archive::library_version_type(6) != library_version){
        ar << I3_SERIALIZATION_NVP(count);
        ar << I3_SERIALIZATION_NVP(bucket_count);
    }
    else{
        // note: fixup for error in version 6.  collection size was
        // changed to size_t BUT for hashed collections it was implemented
        // as an unsigned int.  This should be a problem only on win64 machines
        // but I'll leave it for everyone just in case.
        const unsigned int c = count;
        const unsigned int bc = bucket_count;
        ar << I3_SERIALIZATION_NVP(c);
        ar << I3_SERIALIZATION_NVP(bc);
    }
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

#endif //I3_SERIALIZATION_HASH_COLLECTIONS_SAVE_IMP_HPP
