#ifndef I3_ARCHIVE_BINARY_IARCHIVE_HPP
#define I3_ARCHIVE_BINARY_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// binary_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <istream>
#include <archive/binary_iarchive_impl.hpp>
#include <archive/detail/register_archive.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace icecube {
namespace archive {

// do not derive from this class.  If you want to extend this functionality
// via inhertance, derived from binary_iarchive_impl instead.  This will
// preserve correct static polymorphism.
class binary_iarchive :
    public binary_iarchive_impl<
        icecube::archive::binary_iarchive,
        std::istream::char_type,
        std::istream::traits_type
    >{
public:
    binary_iarchive(std::istream & is, unsigned int flags = 0) :
        binary_iarchive_impl<
            binary_iarchive, std::istream::char_type, std::istream::traits_type
        >(is, flags)
    {}
    binary_iarchive(std::streambuf & bsb, unsigned int flags = 0) :
        binary_iarchive_impl<
            binary_iarchive, std::istream::char_type, std::istream::traits_type
        >(bsb, flags)
    {}
};

} // namespace archive
} // namespace icecube

// required by export
I3_SERIALIZATION_REGISTER_ARCHIVE(icecube::archive::binary_iarchive)
I3_SERIALIZATION_USE_ARRAY_OPTIMIZATION(icecube::archive::binary_iarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // I3_ARCHIVE_BINARY_IARCHIVE_HPP
