#ifndef I3_ARCHIVE_POLYMORPHIC_TEXT_IARCHIVE_HPP
#define I3_ARCHIVE_POLYMORPHIC_TEXT_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_text_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#include <archive/text_iarchive.hpp>
#include <archive/detail/polymorphic_iarchive_route.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace icecube {
namespace archive {

class polymorphic_text_iarchive :
    public detail::polymorphic_iarchive_route<text_iarchive>
{
public:
    polymorphic_text_iarchive(std::istream & is, unsigned int flags = 0) :
        detail::polymorphic_iarchive_route<text_iarchive>(is, flags)
    {}
    ~polymorphic_text_iarchive(){}
};

} // namespace archive
} // namespace icecube

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

// required by export
I3_SERIALIZATION_REGISTER_ARCHIVE(
    icecube::archive::polymorphic_text_iarchive
)

#endif // I3_ARCHIVE_POLYMORPHIC_TEXT_IARCHIVE_HPP
