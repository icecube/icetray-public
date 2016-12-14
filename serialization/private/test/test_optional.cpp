/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_optional.cpp

// (C) Copyright 2004 Pavel Vozenilek
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


#include <archive/archive_exception.hpp>
#include <I3Test.h>

#include <serialization/optional.hpp>

#include "A.hpp"
#include "A.ipp"

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_optional");

    const boost::optional<int> aoptional1;
    const boost::optional<int> aoptional2(123);
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("aoptional1",aoptional1);
        oa << icecube::serialization::make_nvp("aoptional2",aoptional2);
    }

    boost::optional<int> aoptional1a(999);
    boost::optional<int> aoptional2a;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("aoptional1",aoptional1a);
        ia >> icecube::serialization::make_nvp("aoptional2",aoptional2a);
    }
    ENSURE(aoptional1 == aoptional1a);
    ENSURE(aoptional2 == aoptional2a);
    
    std::remove(testfile.c_str());
}

TEST_GROUP(boost_optional)

#define TEST_SET(name) \
TEST(name ## _boost_optional){ \
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
