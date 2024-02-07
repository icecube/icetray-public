// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
// text_archive
#include <archive/text_oarchive.hpp>
#include <archive/text_iarchive.hpp>

struct text_archive_test_settings{
    typedef icecube::archive::text_oarchive test_oarchive;
    typedef std::ofstream test_ostream;

    typedef icecube::archive::text_iarchive test_iarchive;
    typedef std::ifstream test_istream;

    static constexpr std::ios_base::openmode TEST_STREAM_FLAGS = (std::ios_base::openmode)0;
    static constexpr std::ios_base::openmode TEST_ARCHIVE_FLAGS = (std::ios_base::openmode)0;
};

#define test_settings text_archive_test_settings
