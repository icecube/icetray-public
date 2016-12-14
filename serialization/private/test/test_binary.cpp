/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_simple_class.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <I3Test.h>
#include <cstdlib> // for rand(), NULL, size_t

#include <fstream>
#include <boost/config.hpp>

#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::rand; 
    using ::remove;
}
#endif

#include <serialization/nvp.hpp>
#include <serialization/binary_object.hpp>

namespace{
class A {
    friend class icecube::serialization::access;
    char data[150];
    // note: from an aesthetic perspective, I would much prefer to have this
    // defined out of line.  Unfortunately, this trips a bug in the VC 6.0
    // compiler. So hold our nose and put it her to permit running of tests.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & icecube::serialization::make_nvp(
            "data",
            icecube::serialization::make_binary_object(data, sizeof(data))
        );
    }

public:
    A();
    bool operator==(const A & rhs) const;
};

A::A(){
    int i = sizeof(data);
    while(i-- > 0)
        data[i] = static_cast<char>(0xff & std::rand());
}

bool A::operator==(const A & rhs) const {
    int i = sizeof(data);
    while(i-- > 0)
        if(data[i] != rhs.data[i])
            return false;
    return true;
}
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_binary");

    const A a;
    char s1[] = "a";
    char s2[] = "ab";
    char s3[] = "abc";
    char s4[] = "abcd";
    const int i = 12345;

    A a1;
    char s1_1[10];
    char s1_2[10];
    char s1_3[10];
    char s1_4[10];
    int i1 = 34790;

    std::memset(s1_1, '\0', sizeof(s1_1));
    std::memset(s1_2, '\0', sizeof(s1_2));
    std::memset(s1_3, '\0', sizeof(s1_3));
    std::memset(s1_4, '\0', sizeof(s1_4));
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp(
            "s1", 
            icecube::serialization::make_binary_object(
                s1, 
                sizeof(s1)
            )
        );
        oa << icecube::serialization::make_nvp(
            "s2", 
            icecube::serialization::make_binary_object(
                s2, 
                sizeof(s2)
            )
        );
        oa << icecube::serialization::make_nvp(
            "s3", 
            icecube::serialization::make_binary_object(
                s3, 
                sizeof(s3)
            )
        );
        oa << icecube::serialization::make_nvp(
            "s4", 
            icecube::serialization::make_binary_object(
                s4, 
                sizeof(s4)
            )
        );
        oa << I3_SERIALIZATION_NVP(a);
        // note: add a little bit on the end of the archive to detect
        // failure of text mode binary.
        oa << I3_SERIALIZATION_NVP(i);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp(
            "s1", 
            icecube::serialization::make_binary_object(
                s1_1, 
                sizeof(s1)
            )
        );
        ia >> icecube::serialization::make_nvp(
            "s2", 
            icecube::serialization::make_binary_object(
                s1_2, 
                sizeof(s2)
            )
        );
        ia >> icecube::serialization::make_nvp(
            "s3", 
            icecube::serialization::make_binary_object(
                s1_3, 
                sizeof(s3)
            )
        );
        ia >> icecube::serialization::make_nvp(
            "s4", 
            icecube::serialization::make_binary_object(
                s1_4, 
                sizeof(s4)
            )
        );
        ia >> I3_SERIALIZATION_NVP(a1);
        // note: add a little bit on the end of the archive to detect
        // failure of text mode binary.
        ia >> I3_SERIALIZATION_NVP(i1);
    }
    ENSURE(0 == std::strcmp(s1, s1_1));
    ENSURE(0 == std::strcmp(s2, s1_2));
    ENSURE(0 == std::strcmp(s3, s1_3));
    ENSURE(0 == std::strcmp(s4, s1_4));
    ENSURE(a == a1);
    ENSURE(i == i1);
    std::remove(testfile.c_str());
}

TEST_GROUP(binary)

#define TEST_SET(name) \
TEST(name ## _binary){ \
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
