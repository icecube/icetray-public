/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// binary_woarchive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>

#ifdef BOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#define I3_WARCHIVE_SOURCE
#include <archive/binary_woarchive.hpp>

// explicitly instantiate for this type of text stream
#include <archive/impl/archive_serializer_map.ipp>
#include <archive/impl/basic_binary_oprimitive.ipp>
#include <archive/impl/basic_binary_oarchive.ipp>

namespace icecube {
namespace archive {

template class detail::archive_serializer_map<binary_woarchive>;
template class basic_binary_oprimitive<
    binary_woarchive,
    wchar_t,
    std::char_traits<wchar_t>
>;
template class basic_binary_oarchive<binary_woarchive> ;
template class binary_oarchive_impl<
    binary_woarchive,
    wchar_t,
    std::char_traits<wchar_t>
>;

} // namespace archive
} // namespace icecube

#endif // BOOST_NO_STD_WSTREAMBUF
