/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_vector.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <fstream>
#include <string>

#include <cstdio> // remove
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <I3Test.h>

#include <serialization/nvp.hpp>
#include <serialization/string.hpp>
#include <archive/basic_archive.hpp>


template <typename TS /*test settings*/>
void test_version_type(){
    auto testfile = I3Test::testfile("test_version_type");

    // test version size
    uint32_t size = 0x10000ff;
    icecube::archive::version_type cs(size);
    std::string foo("bar");
    {   
        std::string astring(size,'a');
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("cs", cs);
        oa << icecube::serialization::make_nvp("foo", foo);
    }
    icecube::archive::version_type cs2;
    std::string foo2;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("cs", cs2);
        ia >> icecube::serialization::make_nvp("foo", foo2);
    }
    std::remove(testfile.c_str());
    ENSURE(cs == cs2);
    ENSURE(foo == foo2);
}

TEST_GROUP(version_type)

#define TEST_SET(name) \
TEST(name ## _version_type){ \
    test_version_type<test_settings>(); \
}

#define I3_ARCHIVE_TEST binary_archive.hpp
#include "select_archive.hpp"
TEST_SET(binary_archive)

#undef I3_ARCHIVE_TEST
#define I3_ARCHIVE_TEST text_archive.hpp
#include "select_archive.hpp"
TEST_SET(text_archive)

#undef I3_ARCHIVE_TEST
#define I3_ARCHIVE_TEST xml_archive.hpp
#include "select_archive.hpp"
TEST_SET(xml_archive)

#undef I3_ARCHIVE_TEST
#define I3_ARCHIVE_TEST portable_binary_archive.hpp
#include "select_archive.hpp"
TEST_SET(portable_binary_archive)

// EOF
