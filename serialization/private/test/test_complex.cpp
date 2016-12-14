/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_complex.cpp

// (C) Copyright 2005 Matthias Troyer . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <fstream>

#include <cstddef> // NULL
#include <cstdlib> // rand
#include <cstdio> // remove
#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/math/special_functions/next.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
#include <boost/limits.hpp>
namespace std{
    using ::rand; 
    using ::remove;
    #if BOOST_WORKAROUND(BOOST_MSVC, >= 1400) && !defined(UNDER_CE)
        using ::numeric_limits;
    #endif
}
#endif

#include <serialization/complex.hpp>

#include <iostream>

#include <I3Test.h>

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_complex");

    // test array of objects
    std::complex<float> a(
        static_cast<float>(std::rand()) / static_cast<float>(std::rand()),
        static_cast<float>(std::rand()) / static_cast<float>(std::rand()) 
    );
    std::complex<double> b(
        static_cast<double>(std::rand()) / static_cast<double>(std::rand()),
        static_cast<double>(std::rand()) / static_cast<double>(std::rand())
    );
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("afloatcomplex", a);
        oa << icecube::serialization::make_nvp("adoublecomplex", b);
    }
    std::complex<float> a1;
    std::complex<double> b1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("afloatcomplex", a1);
        ia >> icecube::serialization::make_nvp("adoublecomplex", b1);
    }

    std::cerr << "a.real()-a1a.real() distance = " << std::abs( boost::math::float_distance(a.real(), a1.real())) << std::endl;
    ENSURE(std::abs(boost::math::float_distance(a.real(), a1.real())) < 2);
    std::cerr << "a.imag() - a1a.imag() distance = " << std::abs( boost::math::float_distance(a.imag(), a1.imag())) << std::endl;
    ENSURE(std::abs(boost::math::float_distance(a.imag(), a1.imag())) < 2);
    std::cerr << "b.real() - b1.real() distance = " << std::abs( boost::math::float_distance(b.real(), b1.real())) << std::endl;
    ENSURE(std::abs(boost::math::float_distance(b.real(), b1.real())) < 2);
    std::cerr << "b.imag() - b1.imag() distance = " << std::abs( boost::math::float_distance(b.imag(), b1.imag())) << std::endl;
    ENSURE(std::abs(boost::math::float_distance(b.imag(), b1.imag())) < 2);

    std::remove(testfile.c_str());
}

TEST_GROUP(complex)

#define TEST_SET(name) \
TEST(name ## _std_complex){ \
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
