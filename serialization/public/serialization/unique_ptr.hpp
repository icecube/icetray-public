#ifndef I3_SERIALIZATION_UNIQUE_PTR_HPP
#define I3_SERIALIZATION_UNIQUE_PTR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// unique_ptr.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <memory>
#include <serialization/split_free.hpp>
#include <serialization/nvp.hpp>

namespace icecube {
namespace serialization {

/////////////////////////////////////////////////////////////
// implement serialization for unique_ptr< T >
// note: this must be added to the boost namespace in order to
// be called by the library
template<class Archive, class T>
inline void save(
    Archive & ar,
    const std::unique_ptr< T > &t,
    const unsigned int file_version
){
    // only the raw pointer has to be saved
    // the ref count is rebuilt automatically on load
    const T * const tx = t.get();
    ar << I3_SERIALIZATION_NVP(tx);
}

template<class Archive, class T>
inline void load(
    Archive & ar,
    std::unique_ptr< T > &t,
    const unsigned int file_version
){
    T *tx;
    ar >> I3_SERIALIZATION_NVP(tx);
    // note that the reset automagically maintains the reference count
    t.reset(tx);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class T>
inline void serialize(
    Archive & ar,
    std::unique_ptr< T > &t,
    const unsigned int file_version
){
    icecube::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace icecube

#endif // I3_SERIALIZATION_UNIQUE_PTR_HPP
