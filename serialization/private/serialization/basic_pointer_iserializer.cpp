/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_pointer_iserializer.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#define I3_ARCHIVE_SOURCE
#include <archive/detail/basic_pointer_iserializer.hpp>

namespace icecube {
namespace archive {
namespace detail {

I3_ARCHIVE_DECL(BOOST_PP_EMPTY())
basic_pointer_iserializer::basic_pointer_iserializer(
    const icecube::serialization::extended_type_info & eti
) :
    basic_serializer(eti)
{}

I3_ARCHIVE_DECL(BOOST_PP_EMPTY())
basic_pointer_iserializer::~basic_pointer_iserializer() {}

} // namespace detail
} // namespace archive
} // namespace icecube
