/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_multiple_inheritance.cpp

// (C) Copyright 2009 Robert Ramey. 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test of serialization library for multiple inheritence situations

#include <cassert>
#include <fstream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <I3Test.h>

#include <serialization/access.hpp>
#include <serialization/base_object.hpp>
#include <serialization/nvp.hpp>
#include <serialization/export.hpp>

namespace{
struct Base1 {
    int m_x;
    Base1(){}
    Base1(int x) : m_x(1 + x) {}
    virtual ~Base1() {}
    bool operator==(Base1 & rhs) const {
        return m_x == rhs.m_x;
    }
    // serialize
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
        ar & I3_SERIALIZATION_NVP(m_x);
    }
};

//BOOST_CLASS_EXPORT(Base1)

struct Base2 {
    int m_x;
    Base2(){}
    Base2(int x) : m_x(2 + x) {}
    virtual ~Base2() {}
    bool operator==(Base2 & rhs) const {
        return m_x == rhs.m_x;
    }
    // serialize
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
        ar & I3_SERIALIZATION_NVP(m_x);
    }
};

//BOOST_CLASS_EXPORT(Base2)

struct Sub :
    public Base1, 
    public Base2 
{
    int m_x;
    Sub(){}
    Sub(int x) :
        Base1(x),
        Base2(x),
        m_x(x) 
    {}
    bool operator==(Sub & rhs) const {
        if(! Base2::operator==(rhs))
            return false;
        if(! Base1::operator==(rhs))
            return false;
        return m_x == rhs.m_x;
    }
    virtual ~Sub() {}

    // serialize
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */)
    {
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(Base1);
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(Base2);
        ar & I3_SERIALIZATION_NVP(m_x);
    }
};
}

I3_CLASS_EXPORT(Sub)

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_mi");
    Base2* pb2;
    {
        // serialize
        pb2 = new Sub(2);

        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("Base2", pb2);
    }
    Base2* pb2_1;
    {
        // de-serialize
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("Base2", pb2_1);
    }
    Sub* s1 = dynamic_cast<Sub*>(pb2);
    ENSURE(0 != s1);
    Sub* s2 = dynamic_cast<Sub*>(pb2_1);
    ENSURE(0 != s2);
    ENSURE(*s1 == *s2);
}

TEST_GROUP(multiple_inheritance)

#define TEST_SET(name) \
TEST(name ## _multiple_inheritance){ \
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
