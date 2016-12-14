/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_list.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef>
#include <fstream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <I3Test.h>

#include <boost/type_traits/is_pointer.hpp>
#include <boost/checked_delete.hpp>

#include <archive/archive_exception.hpp>
#include <serialization/nvp.hpp>

#include "A.hpp"
#include "A.ipp"

template<class T>
struct ptr_equal_to : public std::binary_function<T, T, bool> 
{
    static_assert(::boost::is_pointer< T >::value,"T must be a pointer type");
    bool operator()(T const _Left, T const _Right) const
    {
        if(NULL == _Left && NULL == _Right)
            return true;
        if(typeid(*_Left) != typeid(*_Right))
            return false;
        return *_Left == *_Right;
    }
};

#include <serialization/list.hpp>

template <typename TS /*test settings*/>
void test_list(){
    auto testfile = I3Test::testfile("test_list_ptrs");
    std::cout << std::endl;
    std::cout << "testfile is " << testfile << std::endl;

    std::list<A*> alist;
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        A* free_a_ptr = new A;
        alist.push_back(free_a_ptr);
        alist.push_back(new A);
        // verify that first element is the same as the free pointer
        ENSURE((*alist.begin()) == free_a_ptr);
        oa << icecube::serialization::make_nvp("alist", alist);
        oa << icecube::serialization::make_nvp("free_a_ptr", free_a_ptr);
    }
    std::cout << "foo" << std::endl;
    std::list<A*> alist1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        A* free_a_ptr1;
        std::cout << "bar" << std::endl;
        ia >> icecube::serialization::make_nvp("alist", alist1);
        std::cout << "baz" << std::endl;
        ia >> icecube::serialization::make_nvp("free_a_ptr", free_a_ptr1);
        std::cout << "quux" << std::endl;
        ENSURE(
            alist.size() == alist1.size() 
            && std::equal(alist.begin(),alist.end(),alist1.begin(),ptr_equal_to<A*>())
        );
        // verify that first element is the same as the free pointer
        ENSURE((*alist1.begin()) == free_a_ptr1);
    }

    std::for_each(
        alist.begin(), 
        alist.end(), 
        boost::checked_deleter<A>()
    );
    std::for_each(
        alist1.begin(), 
        alist1.end(), 
        boost::checked_deleter<A>()
    );
    std::remove(testfile.c_str());
}

#ifdef BOOST_HAS_SLIST
#include <serialization/slist.hpp>

template <typename TS /*test settings*/>
void test_slist(){
    auto testfile = I3Test::testfile("test_slist_ptrs");

    std::list<A*> aslist;
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        aslist.push_back(new A);
        aslist.push_back(new A);
        oa << icecube::serialization::make_nvp("aslist", aslist);
    }
    std::list<A*> aslist1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("aslist", aslist1);
        ENSURE(aslist.size() == aslist1.size() &&
            std::equal(aslist.begin(),aslist.end(),aslist1.begin(),ptr_equal_to<A*>())
        );
    }
    std::for_each(
        aslist.begin(), 
        aslist.end(), 
        boost::checked_deleter<A>()
    );
    std::for_each(
        aslist1.begin(), 
        aslist1.end(), 
        boost::checked_deleter<A>()
    );  
    std::remove(testfile.c_str());
}
#endif

#ifndef BOOST_NO_CXX11_HDR_FORWARD_LIST
#include <serialization/forward_list.hpp>

template <typename TS /*test settings*/>
void test_forward_list(){
    auto testfile = I3Test::testfile("test_forward_list_ptrs");

    std::forward_list<A*> aslist;
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        aslist.push_front(new A);
        aslist.push_front(new A);
        oa << icecube::serialization::make_nvp("aslist", aslist);
    }
    std::forward_list<A*> aslist1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("aslist", aslist1);
        ENSURE(
            std::equal(aslist.begin(),aslist.end(),aslist1.begin(),ptr_equal_to<A*>())
        );
    }
    std::for_each(
        aslist.begin(), 
        aslist.end(), 
        boost::checked_deleter<A>()
    );
    std::for_each(
        aslist1.begin(), 
        aslist1.end(), 
        boost::checked_deleter<A>()
    );
    std::remove(testfile.c_str());
}
#endif

TEST_GROUP(test_list_ptrs)

#ifdef BOOST_HAS_SLIST
    #define SLIST_TEST(name) \
    TEST(name ## _slist_ptrs){ \
        test_slist<test_settings>(); \
    }
#else
    #define SLIST_TEST(name)
#endif

#ifndef BOOST_NO_CXX11_HDR_FORWARD_LIST
    #define FORWARD_LIST_TEST(name) \
    TEST(name ## _test_forward_list_ptrs){ \
        test_forward_list<test_settings>(); \
    }
#else
    #define BOOST_NO_CXX11_HDR_FORWARD_LIST(name)
#endif

#define TEST_SET(name) \
TEST(name ## _list_ptrs){ \
    test_list<test_settings>(); \
} \
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
