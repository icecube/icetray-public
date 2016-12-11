// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
// xml_archive
#include <archive/xml_oarchive.hpp>
#include <archive/xml_iarchive.hpp>

struct xml_archive_test_settings{
    typedef icecube::archive::xml_oarchive test_oarchive;
    typedef std::ofstream test_ostream;
    
    typedef icecube::archive::xml_iarchive test_iarchive;
    typedef std::ifstream test_istream;
    
    static constexpr std::ios_base::openmode TEST_STREAM_FLAGS = (std::ios_base::openmode)0;
    static constexpr std::ios_base::openmode TEST_ARCHIVE_FLAGS = (std::ios_base::openmode)0;
};

#define test_settings xml_archive_test_settings
