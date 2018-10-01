#ifndef I3_ARCHIVE_POLYMORPHIC_TEXT_WOARCHIVE_HPP
#define I3_ARCHIVE_POLYMORPHIC_TEXT_WOARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_text_oarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#ifdef BOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else

#include <archive/text_woarchive.hpp>
#include <archive/detail/polymorphic_oarchive_route.hpp>

namespace icecube { 
namespace archive {

typedef detail::polymorphic_oarchive_route<
    text_woarchive_impl<text_woarchive> 
> polymorphic_text_woarchive;

} // namespace archive
} // namespace icecube

// required by export
I3_SERIALIZATION_REGISTER_ARCHIVE(
    icecube::archive::polymorphic_text_woarchive
)

#endif // BOOST_NO_STD_WSTREAMBUF 
#endif // I3_ARCHIVE_POLYMORPHIC_TEXT_WOARCHIVE_HPP
