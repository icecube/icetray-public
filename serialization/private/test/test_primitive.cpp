/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_primitive.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test implementation level "primitive_type"

#include <cstddef> // NULL
#include <fstream>

#include <I3Test.h>

#include <serialization/level.hpp>
#include <serialization/nvp.hpp>

namespace{
struct A
{
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        // note: should never fail here
        static_assert(0 == sizeof(Archive),"This assert should be unreachable");
    }
};

std::ostream & operator<<(std::ostream &os, const A & /* a */){ return os;}
std::istream & operator>>(std::istream &is, A & /* a */){return is;}

//#ifndef BOOST_NO_STD_WSTREAMBUF
//std::wostream & operator<<(std::wostream &os, const A & /* a */){ return os;}
//std::wistream & operator>>(std::wistream &is, A & /* a */){return is;}
//#endif
}

I3_CLASS_IMPLEMENTATION(A, icecube::serialization::primitive_type)

template <typename TS /*test settings*/>
void out(const char* testfile, A& a)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(a);
}

template <typename TS /*test settings*/>
void in(const char* testfile, A& a)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(a);
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_primitive");

    A a;
    out<TS>(testfile.c_str(), a);
    in<TS>(testfile.c_str(), a);
    std::remove(testfile.c_str());
}

TEST_GROUP(primitive)

#define TEST_SET(name) \
TEST(name ## _primitive){ \
    do_test<test_settings>(); \
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
