/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_object.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test implementation level "object_serializable"
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

#include <I3Test.h>

#include <serialization/level.hpp>
#include <serialization/nvp.hpp>

namespace{
class A{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & /* ar */, const unsigned int /* file_version */){}
};
}

I3_CLASS_IMPLEMENTATION(A, icecube::serialization::object_serializable)

// note: version can be assigned only to objects whose implementation
// level is object_class_info.  So, doing the following will result in
// a static assertion
// BOOST_CLASS_VERSION(A, 2);

template <typename TS /*test settings*/>
void out(const char* testfile, A & a)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(a);
}

template <typename TS /*test settings*/>
void in(const char* testfile, A & a)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(a);
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_object");

    A a;
    out<TS>(testfile.c_str(), a);
    in<TS>(testfile.c_str(), a);
    std::remove(testfile.c_str());
}

TEST_GROUP(object)

#define TEST_SET(name) \
TEST(name ## _object){ \
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
