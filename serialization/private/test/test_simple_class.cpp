/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_simple_class.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

// invoke header for a custom archive test.

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>
#include <cmath>
#include <boost/math/special_functions/next.hpp>

#include <boost/config.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <I3Test.h>

#include "A.hpp"
#include "A.ipp"

bool A::check_equal(const A &rhs) const
{
    ENSURE_EQUAL(b, rhs.b);
    ENSURE_EQUAL(l, rhs.l);
    #ifndef BOOST_NO_INT64_T
    ENSURE_EQUAL(f, rhs.f);
    ENSURE_EQUAL(g, rhs.g);
    #endif
    ENSURE_EQUAL(m, rhs.m);
    ENSURE_EQUAL(n, rhs.n);
    ENSURE_EQUAL(o, rhs.o);
    ENSURE_EQUAL(p, rhs.p);
    ENSURE_EQUAL(q, rhs.q);
    #ifndef BOOST_NO_CWCHAR
    ENSURE_EQUAL(r, rhs.r);
    #endif
    ENSURE_EQUAL(c, rhs.c);
    ENSURE_EQUAL(s, rhs.s);
    ENSURE_EQUAL(t, rhs.t);
    ENSURE_EQUAL(u, rhs.u);
    ENSURE_EQUAL(v, rhs.v);
    ENSURE_EQUAL(l, rhs.l);
    ENSURE(std::abs( boost::math::float_distance(w, rhs.w)) < 2);
    ENSURE(std::abs( boost::math::float_distance(x, rhs.x)) < 2);
    ENSURE(!(0 != y.compare(rhs.y)));
    #ifndef BOOST_NO_STD_WSTRING
    ENSURE(!(0 != z.compare(rhs.z)));
    #endif      
    return true;
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_simple_class");

    A a, a1;
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("a", a);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("a", a1);
    }
    a.check_equal(a1);
    std::remove(testfile.c_str());
}

TEST_GROUP(simple_class)

#define TEST_SET(name) \
TEST(name ## _simple_class){ \
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
