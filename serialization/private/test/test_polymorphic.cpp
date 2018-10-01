/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_polymorphic.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

// the following is to ensure that when one of the libraries changes
// BJAM rebuilds and relinks the test.
/*
#include "polymorphic_text_archive.hpp"
#include "polymorphic_text_warchive.hpp"
#include "polymorphic_binary_archive.hpp"
#include "polymorphic_xml_archive.hpp"
#include "polymorphic_xml_warchive.hpp"
*/

#include <I3Test.h>

#include <archive/polymorphic_oarchive.hpp>
#include <archive/polymorphic_iarchive.hpp>

#include <serialization/nvp.hpp>
#include "test_polymorphic_A.hpp"

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_polymorphic");
    const data d;
    data d1;
    // test using using polymorphic interface
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa_implementation(os, TS::TEST_ARCHIVE_FLAGS);
        icecube::archive::polymorphic_oarchive& oa_interface = oa_implementation;
        oa_interface << I3_SERIALIZATION_NVP(d);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia_implementation(is, TS::TEST_ARCHIVE_FLAGS);
        icecube::archive::polymorphic_iarchive& ia_interface = ia_implementation;
        ia_interface >> I3_SERIALIZATION_NVP(d1);
    }
    ENSURE(d == d1);
    std::remove(testfile.c_str());

    // test using using polymorphic implementation.
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa_implementation(os, TS::TEST_ARCHIVE_FLAGS);
        oa_implementation << I3_SERIALIZATION_NVP(d);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia_implementation(is, TS::TEST_ARCHIVE_FLAGS);
        ia_implementation >> I3_SERIALIZATION_NVP(d1);
    }
    ENSURE(d == d1);
    std::remove(testfile.c_str());

    // test using using polymorphic implementation.
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        icecube::archive::polymorphic_oarchive* oa_implementation
            = new typename TS::test_oarchive(os, TS::TEST_ARCHIVE_FLAGS);
        *oa_implementation << I3_SERIALIZATION_NVP(d);
        delete oa_implementation;
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        icecube::archive::polymorphic_iarchive* ia_implementation
            = new typename TS::test_iarchive(is, TS::TEST_ARCHIVE_FLAGS);
        *ia_implementation >> I3_SERIALIZATION_NVP(d1);
        delete ia_implementation;
    }
    ENSURE(d == d1);
    std::remove(testfile.c_str());
}

TEST_GROUP(polymorphic)

#define TEST_SET(name) \
TEST(name ## _polymorphic){ \
    do_test<test_settings>(); \
}

#define I3_ARCHIVE_TEST polymorphic_binary_archive.hpp
#include "select_archive.hpp"
TEST_SET(polymorphic_binary_archive)

#undef I3_ARCHIVE_TEST
#define I3_ARCHIVE_TEST polymorphic_text_archive.hpp
#include "select_archive.hpp"
TEST_SET(polymorphic_text_archive)

#undef I3_ARCHIVE_TEST
#define I3_ARCHIVE_TEST polymorphic_xml_archive.hpp
#include "select_archive.hpp"
TEST_SET(polymorphic_xml_archive)
