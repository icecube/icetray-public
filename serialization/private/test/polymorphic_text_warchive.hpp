// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <boost/config.hpp>
#ifdef BOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else
#include <archive/polymorphic_text_woarchive.hpp>
typedef icecube::archive::polymorphic_text_woarchive test_oarchive;
typedef std::wofstream test_ostream;
#include <archive/polymorphic_text_wiarchive.hpp>
typedef icecube::archive::polymorphic_text_wiarchive test_iarchive;
typedef std::wifstream test_istream;
#endif
