/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_cyclic_ptrs.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <fstream>

#include <cstdio> // remove
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <archive/archive_exception.hpp>
#include <serialization/nvp.hpp>
#include <serialization/version.hpp>
#include <serialization/base_object.hpp>

#include <I3Test.h>

#include "A.hpp"
#include "A.ipp"

///////////////////////////////////////////////////////
// class with a member which refers to itself
class J : public A
{
private:
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(A);
        ar & I3_SERIALIZATION_NVP(j);
    }
public:
    bool operator==(const J &rhs) const;
    J *j;
    J(J *_j) : j(_j) {}
    J() : j(NULL){}
};

I3_CLASS_VERSION(J, 6)

bool J::operator==(const J &rhs) const
{
    return static_cast<const A &>(*this) == static_cast<const A &>(rhs);
}

///////////////////////////////////////////////////////
// class with members that refer to each other
// this is an example of a class that, as written, cannot
// be serialized with this system.  The problem is that the
// serialization of the first member - j1 , provokes serialization
// of those objects which it points to either directly or indirectly.
// When those objects are subsequently serialized, it is discovered
// that have already been serialized through pointers.  This is
// detected by the system and an exception - pointer_conflict - 
// is thrown.  Permiting this to go undetected would result in the
// creation of multiple equal objects rather than the original
// structure.  
class K
{
    J j1;
    J j2;
    J j3;
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(
        Archive &ar,
        const unsigned int /* file_version */
    ){
        ar & I3_SERIALIZATION_NVP(j1);
        ar & I3_SERIALIZATION_NVP(j2);
        ar & I3_SERIALIZATION_NVP(j3);
    }
public:
    bool operator==(const K &rhs) const;
    K();
};

K::K()
: j1(&j2), j2(&j3), j3(&j1)
{
}

bool K::operator==(const K &rhs) const
{
    return
        j1.j == & j2
        && j2.j == & j3
        && j3.j == & j1
        && j1 == rhs.j1
        && j2 == rhs.j2
        && j3 == rhs.j3
    ;
}

template <typename TS /*test settings*/>
void test1(){
    auto testfile = I3Test::testfile("test_cyclic_ptrs_1");

    J j1, j2;
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << I3_SERIALIZATION_NVP(j1);
    }
    {
        // try to read the archive
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> I3_SERIALIZATION_NVP(j2);
    }
    ENSURE(j1 == j2);
    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test2(){
    auto testfile = I3Test::testfile("test_cyclic_ptrs_2");

    J *j1 = new J;
    j1->j = j1;
    J *j2 = reinterpret_cast<J *>(0xBAADF00D);
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << I3_SERIALIZATION_NVP(j1);
    }
    {
        // try to read the archive
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> I3_SERIALIZATION_NVP(j2);
    }
    ENSURE(*j1 == *j2);
    ENSURE(j2 == j2->j);
    std::remove(testfile.c_str());
}

template <typename TS /*test settings*/>
void test3(){
    auto testfile = I3Test::testfile("test_cyclic_ptrs_3");

    K k;
    icecube::archive::archive_exception exception(
        icecube::archive::archive_exception::no_exception
    );
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        try{
            oa << I3_SERIALIZATION_NVP(k);
        }
        catch(icecube::archive::archive_exception &ae){
            exception = ae;
        }
        ENSURE(
            exception.code == icecube::archive::archive_exception::pointer_conflict
        );
    }
    // if exception wasn't invoked
    if(exception.code == icecube::archive::archive_exception::no_exception){
        // try to read the archive
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        exception = icecube::archive::archive_exception(
            icecube::archive::archive_exception::no_exception
        );
        try{
            ia >> I3_SERIALIZATION_NVP(k);
        }
        catch(icecube::archive::archive_exception &ae){
            exception = ae;
        }
        ENSURE(
            exception.code == icecube::archive::archive_exception::pointer_conflict
        );
    }
    std::remove(testfile.c_str());
}

TEST_GROUP(cyclic_ptrs)

#define TEST_SET(name) \
TEST(name ## _test1){ \
    test1<test_settings>(); \
} \
TEST(name ## _test2){ \
    test2<test_settings>(); \
} \
TEST(name ## _test3){ \
    test3<test_settings>(); \
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
