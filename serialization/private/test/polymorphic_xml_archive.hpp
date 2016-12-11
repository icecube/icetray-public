// (C) Copyright 2002-4 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <archive/polymorphic_xml_oarchive.hpp>
#include <archive/polymorphic_xml_iarchive.hpp>

struct polymorphic_xml_archive_test_settings{
    typedef icecube::archive::polymorphic_xml_oarchive test_oarchive;
    typedef std::ofstream test_ostream;
    
    typedef icecube::archive::polymorphic_xml_iarchive test_iarchive;
    typedef std::ifstream test_istream;
    
    static constexpr int TEST_STREAM_FLAGS = 0;
    static constexpr int TEST_ARCHIVE_FLAGS = 0;
};

#define test_settings polymorphic_xml_archive_test_settings
