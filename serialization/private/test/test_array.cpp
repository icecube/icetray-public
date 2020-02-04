/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_array.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <I3Test.h>
#include <boost/config.hpp>

#include <cstddef>
#include <fstream>
#include <algorithm> // equal
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif
#include <archive/archive_exception.hpp>
#include <boost/array.hpp>

#include "A.hpp"
#include "A.ipp"

template <class T, typename TS /*test settings*/>
void test_native_array(){
    auto testfile = I3Test::testfile("test_native_array");

    // test array of objects
    const T a_array[10]={T(),T(),T(),T(),T(),T(),T(),T(),T(),T()};
    const T b_array[2][3]={{T(),T(),T()},{T(),T(),T()}};
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("a_array", a_array);
        oa << icecube::serialization::make_nvp("b_array", b_array);
    }
    {
        T a_array1[10];
        T b_array1[2][3];
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("a_array", a_array1);
        ia >> icecube::serialization::make_nvp("b_array", b_array1);

        ENSURE(std::equal(& a_array[0], & a_array[10], & a_array1[0]));
        ENSURE(b_array[0][0] == b_array1[0][0]);
        ENSURE(b_array[1][0] == b_array1[1][0]);
    }
    {
        T a_array1[9];
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        try {
            typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
            bool exception_invoked = false;
            try {
                ia >> icecube::serialization::make_nvp("a_array", a_array1);
            }
            catch (icecube::archive::archive_exception &ae){
                ENSURE(
                    icecube::archive::archive_exception::array_size_too_short
                    == ae.code
                );
                exception_invoked = true;
            }
            ENSURE(exception_invoked);
        }
        catch (icecube::archive::archive_exception &ae){}
    }
    std::remove(testfile.c_str());
}

template <class T, typename TS /*test settings*/>
void test_boost_array(){
    auto testfile = I3Test::testfile("test_boost_array");

    // test array of objects
    const boost::array<T,10> a_array = {{T(),T(),T(),T(),T(),T(),T(),T(),T(),T()}};
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("a_array", a_array);
    }
    {
        boost::array<T,10> a_array1;
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("a_array", a_array1);

        ENSURE(std::equal(a_array.begin(), a_array.end(), a_array1.begin()));
    }
    {
        boost::array<T, 9> a_array1;
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        try {
            typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
            bool exception_invoked = false;
            try {
                ia >> icecube::serialization::make_nvp("a_array", a_array1);
            }
            catch (icecube::archive::archive_exception &ae){
                ENSURE(
                    icecube::archive::archive_exception::array_size_too_short
                    == ae.code
                );
                exception_invoked = true;
            }
            ENSURE(exception_invoked);
        }
        catch (icecube::archive::archive_exception &ae){}
    }
    std::remove(testfile.c_str());
}

#ifndef BOOST_NO_CXX11_HDR_ARRAY
#include <array>

template <class T, typename TS /*test settings*/>
void test_std_array(){
    auto testfile = I3Test::testfile("test_std_array");

    // test array of objects
    const std::array<T, 10> a_array = {{T(),T(),T(),T(),T(),T(),T(),T(),T(),T()}};
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("a_array", a_array);
    }
    {
        std::array<T, 10> a_array1;
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("a_array", a_array1);

        ENSURE(std::equal(a_array.begin(), a_array.end(), a_array1.begin()));
    }
    {
        std::array<T, 9> a_array1;
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        try {
            typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
            bool exception_invoked = false;
            try {
                ia >> icecube::serialization::make_nvp("a_array", a_array1);
            }
            catch (icecube::archive::archive_exception &ae){
                ENSURE(
                    icecube::archive::archive_exception::array_size_too_short
                    == ae.code
                );
                exception_invoked = true;
            }
            ENSURE(exception_invoked);
        }
        catch (icecube::archive::archive_exception &ae){}
    }
    std::remove(testfile.c_str());
}
#endif

TEST_GROUP(array);

#define TEST_SET(name) \
TEST(name ## _native_array_A){ \
    test_native_array<A,test_settings>(); \
} \
TEST(name ## _native_array_int){ \
    test_native_array<int,test_settings>(); \
} \
TEST(name ## _boost_array_A){ \
    test_boost_array<A,test_settings>(); \
} \
TEST(name ## _boost_array_int){ \
    test_boost_array<int,test_settings>(); \
} \
TEST(name ## _std_array_A){ \
    test_std_array<A,test_settings>(); \
} \
TEST(name ## _std_array_int){ \
    test_std_array<int,test_settings>(); \
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
