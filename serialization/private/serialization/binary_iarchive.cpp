/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// binary_iarchive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <istream>

#define I3_ARCHIVE_SOURCE
#include <archive/binary_iarchive.hpp>
#include <archive/detail/archive_serializer_map.hpp>

#include <archive/impl/archive_serializer_map.ipp>
#include <archive/impl/basic_binary_iprimitive.ipp>
#include <archive/impl/basic_binary_iarchive.ipp>

namespace icecube {
namespace archive {

// explicitly instantiate for this type of stream
template class detail::archive_serializer_map<binary_iarchive>;
template class basic_binary_iprimitive<
    binary_iarchive,
    std::istream::char_type,
    std::istream::traits_type
>;
template class basic_binary_iarchive<binary_iarchive> ;
template class binary_iarchive_impl<
    binary_iarchive,
    std::istream::char_type,
    std::istream::traits_type
>;

} // namespace archive
} // namespace icecube
