/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// binary_oarchive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <ostream>

#define I3_ARCHIVE_SOURCE
#include <archive/binary_oarchive.hpp>
#include <archive/detail/archive_serializer_map.hpp>

// explicitly instantiate for this type of binary stream
#include <archive/impl/archive_serializer_map.ipp>
#include <archive/impl/basic_binary_oprimitive.ipp>
#include <archive/impl/basic_binary_oarchive.ipp>

namespace icecube {
namespace archive {

template class detail::archive_serializer_map<binary_oarchive>;
template class basic_binary_oprimitive<
    binary_oarchive,
    std::ostream::char_type,
    std::ostream::traits_type
>;
template class basic_binary_oarchive<binary_oarchive> ;
template class binary_oarchive_impl<
    binary_oarchive,
    std::ostream::char_type,
    std::ostream::traits_type
>;

} // namespace archive
} // namespace icecube
