/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8

// (C) Copyright 2002-4 Pavel Vozenilek . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Provides non-intrusive serialization for boost::optional.

#ifndef I3_SERIALIZATION_OPTIONAL_HPP_
#define I3_SERIALIZATION_OPTIONAL_HPP_

#if defined(_MSC_VER)
# pragma once
#endif

#include <boost/config.hpp>

#include <archive/detail/basic_iarchive.hpp>

#include <boost/optional.hpp>
#include <serialization/item_version_type.hpp>
#include <serialization/split_free.hpp>
#include <serialization/level.hpp>
#include <serialization/nvp.hpp>
#include <serialization/version.hpp>
#include <serialization/detail/stack_constructor.hpp>

// function specializations must be defined in the appropriate
// namespace - icecube::serialization
namespace icecube { 
namespace serialization {

template<class Archive, class T>
void save(
    Archive & ar, 
    const boost::optional< T > & t, 
    const unsigned int /*version*/
){
    const bool tflag = t.is_initialized();
    ar << icecube::serialization::make_nvp("initialized", tflag);
    if (tflag){
        const icecube::serialization::item_version_type item_version(version< T >::value);
        #if 0
        const icecube::archive::library_version_type library_version(
            ar.get_library_version()
        };
        if(icecube::archive::library_version_type(3) < library_version){
            ar << I3_SERIALIZATION_NVP(item_version);
        }
        #else
            ar << I3_SERIALIZATION_NVP(item_version);
        #endif
        ar << icecube::serialization::make_nvp("value", *t);
    }
}

template<class Archive, class T>
void load(
    Archive & ar, 
    boost::optional< T > & t, 
    const unsigned int /*version*/
){
    bool tflag;
    ar >> icecube::serialization::make_nvp("initialized", tflag);
    if (tflag){
        icecube::serialization::item_version_type item_version(0);
        icecube::archive::library_version_type library_version(
            ar.get_library_version()
        );
        if(icecube::archive::library_version_type(3) < library_version){
            // item_version is handled as an attribute so it doesnt need an NVP
           ar >> I3_SERIALIZATION_NVP(item_version);
        }
        detail::stack_construct<Archive, T> aux(ar, item_version);
        ar >> icecube::serialization::make_nvp("value", aux.reference());
        t.reset(aux.reference());
    }
    else {
        t.reset();
    }
}

template<class Archive, class T>
void serialize(
    Archive & ar, 
    boost::optional< T > & t, 
    const unsigned int version
){
    icecube::serialization::split_free(ar, t, version);
}

} // serialization
} // namespace icecube

#endif // I3_SERIALIZATION_OPTIONAL_HPP_
