// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// binary_archive
#include <archive/binary_oarchive.hpp>

#include <archive/binary_iarchive.hpp>

struct binary_archive_test_settings{
    typedef icecube::archive::binary_oarchive test_oarchive;
    typedef std::ofstream test_ostream;
    
    typedef icecube::archive::binary_iarchive test_iarchive;
    typedef std::ifstream test_istream;
    
    static constexpr std::ios_base::openmode TEST_STREAM_FLAGS = (std::ios::binary);
    static constexpr std::ios_base::openmode TEST_ARCHIVE_FLAGS = (std::ios_base::openmode)0;
};

#define test_settings binary_archive_test_settings
