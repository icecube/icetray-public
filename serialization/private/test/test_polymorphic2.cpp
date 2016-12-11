/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_polymorphic2.cpp

// (C) Copyright 2009 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution
#include <fstream>

#include <I3Test.h>

#include <archive/polymorphic_oarchive.hpp>
#include <archive/polymorphic_iarchive.hpp>

#include <serialization/nvp.hpp>
#include "test_polymorphic2.hpp"

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_polymorphic2");

    //using namespace polymorphic2;
    polymorphic2::A* a = new polymorphic2::B();
    a->i = 3;
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa_implementation(os, TS::TEST_ARCHIVE_FLAGS);
        icecube::archive::polymorphic_oarchive& opa = oa_implementation;
        opa << I3_SERIALIZATION_NVP(a);
    }
    polymorphic2::A* loaded = 0;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia_implementation(is, TS::TEST_ARCHIVE_FLAGS);
        icecube::archive::polymorphic_iarchive& ipa = ia_implementation;
        ipa >> I3_SERIALIZATION_NVP(loaded);
    }
    ENSURE(a->i == loaded->i);
    delete a;
    delete loaded;
}

TEST_GROUP(polymorphic2)

#define TEST_SET(name) \
TEST(name ## _polymorphic2){ \
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
