/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_non_intrursive.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

// this tests:
// a) non-intrusive method of implementing serialization
// b) usage of a non-default constructor

#include <fstream>
#include <cstdlib> // for rand()
#include <cstdio>  // remove
#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/limits.hpp>
#include <boost/math/special_functions/next.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::rand; 
    using ::remove;
    #if BOOST_WORKAROUND(BOOST_MSVC, >= 1400) && !defined(UNDER_CE)
        using ::numeric_limits;
    #endif
}
#endif

#include <archive/archive_exception.hpp>
#include <serialization/nvp.hpp>
#include <I3Test.h>

namespace{
///////////////////////////////////////////////////////
// simple class test - using non-intrusive syntax
// illustrates the usage of the non-intrusve syntax
class A
{
public:
    signed char s;
    unsigned char t;
    signed int u;
    unsigned int v;
    float w;
    double x;
    A();
};

A::A() : 
    s(static_cast<signed char>(0xff & std::rand())),
    t(static_cast<signed char>(0xff & std::rand())),
    u(std::rand()),
    v(std::rand()),
    w((float)std::rand() / std::rand()),
    x((double)std::rand() / std::rand())
{
}
 
}

// note the following:

// function specializations must be defined in the appropriate
// namespace - boost::serialization
namespace icecube {
namespace serialization {

// This first set of overrides should work with all compilers.

// The last argument is int while the default versions
// defined in serialization.hpp have long as the last argument.
// This is part of the work around for compilers that don't 
// support correct function template ordering.  These functions
// are always called with 0 (i.e. an int) as the last argument.
// Our specialized versions also have int as the last argument
// while the default versions have a long as the last argument.
// This makes our specialized versions a better match than the
// default ones as no argument conversion is required to make a match
template<class Archive>
void serialize(
    Archive & ar, 
    A & a, 
    const unsigned int /* file_version */
){
    ar & icecube::serialization::make_nvp("s", a.s);
    ar & icecube::serialization::make_nvp("t", a.t);
    ar & icecube::serialization::make_nvp("u", a.u);
    ar & icecube::serialization::make_nvp("v", a.v);
    ar & icecube::serialization::make_nvp("w", a.w);
    ar & icecube::serialization::make_nvp("x", a.x);
}

} // serialization
} // namespace boost

template <typename TS /*test settings*/>
void save(const char * testfile){
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    A a;

    oa << I3_SERIALIZATION_NVP(a);
    
    // save a copy pointer to this item
    A* pa1 = &a;
    oa << I3_SERIALIZATION_NVP(pa1);

    // save pointer to a new object
    A* pa2 = new A();
    oa << I3_SERIALIZATION_NVP(pa2);

    delete pa2;
}

template <typename TS /*test settings*/>
void load(const char * testfile){
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    A a;
    ia >> I3_SERIALIZATION_NVP(a);

    A* pa1;
    ia >> I3_SERIALIZATION_NVP(pa1);
    ENSURE(pa1 == &a, "Copy of pointer not correctly restored");

    A* pa2;
    ia >> I3_SERIALIZATION_NVP(pa2);
    ENSURE(pa2 != &a, "Pointer not correctly restored");

    delete pa2;
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_complex");
    save<TS>(testfile.c_str());
    load<TS>(testfile.c_str());
    std::remove(testfile.c_str());
}

TEST_GROUP(test_non_intrusive)

#define TEST_SET(name) \
TEST(name ## _test_non_intrusive){ \
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
