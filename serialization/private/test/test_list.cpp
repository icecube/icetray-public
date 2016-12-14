/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_list.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <fstream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <archive/archive_exception.hpp>
#include <I3Test.h>

#include "A.hpp"
#include "A.ipp"

#include <serialization/list.hpp>

template <typename TS /*test settings*/>
void test_list(){
    auto testfile = I3Test::testfile("test_list");

    std::list<A> alist;
    alist.push_back(A());
    alist.push_back(A());
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("alist",alist);
    }

    std::list<A> alist1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("alist",alist1);
    }
    ENSURE(alist == alist1);
    std::remove(testfile.c_str());
}

#ifdef BOOST_HAS_SLIST
#include <serialization/slist.hpp>

template <typename TS /*test settings*/>
void test_slist(){
    auto testfile = I3Test::testfile("test_slist");
    
    BOOST_STD_EXTENSION_NAMESPACE::slist<A> aslist;
    aslist.push_front(A());
    aslist.push_front(A());
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("aslist", aslist);
    }
    BOOST_STD_EXTENSION_NAMESPACE::slist<A> aslist1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("aslist", aslist1);
    }
    ENSURE(aslist == aslist1);
    std::remove(testfile.c_str());
}
#endif

#ifndef BOOST_NO_CXX11_HDR_FORWARD_LIST
#include <serialization/forward_list.hpp>

template <typename TS /*test settings*/>
void test_forward_list(){
    auto testfile = I3Test::testfile("test_forward_list");
    
    try{
    std::forward_list<A> aslist;
    aslist.push_front(A());
    aslist.push_front(A());
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("aslist", aslist);
    }
    std::forward_list<A> aslist1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("aslist", aslist1);
    }
    ENSURE(aslist == aslist1);
    }catch(icecube::archive::archive_exception& ae){
        std::cout << "Exception: " << ae.code << ' ' << ae.what() << std::endl;
    }
    std::remove(testfile.c_str());
}
#endif

TEST_GROUP(test_list)

#ifdef BOOST_HAS_SLIST
    #define SLIST_TEST(name) \
    TEST(name ## _slist){ \
        test_slist<test_settings>(); \
    }
#else
    #define SLIST_TEST(name)
#endif

#ifndef BOOST_NO_CXX11_HDR_FORWARD_LIST
    #define FORWARD_LIST_TEST(name) \
    TEST(name ## _test_forward_list){ \
        test_forward_list<test_settings>(); \
    }
#else
    #define BOOST_NO_CXX11_HDR_FORWARD_LIST(name)
#endif

#define TEST_SET(name) \
TEST(name ## _list){ \
    test_list<test_settings>(); \
}\
SLIST_TEST(name) \
FORWARD_LIST_TEST(name)

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

