/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_split.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
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

#include <I3Test.h>

#include <serialization/nvp.hpp>
#include <serialization/split_member.hpp>
#include <serialization/split_free.hpp>

namespace{
class A
{
    friend class icecube::serialization::access;
    template<class Archive>
    void save(
        Archive & /* ar */, 
        const unsigned int /* file_version */
    ) const {
        ++(const_cast<A &>(*this).count);
    }
    template<class Archive>
    void load(
        Archive & /* ar */, 
        const unsigned int /* file_version */
    ){
        --count;
    }
    I3_SERIALIZATION_SPLIT_MEMBER()
    int count;
public:
    A() : count(0) {}
    ~A() {
        ENSURE(0 == count);
    }
};

class B
{
    friend class icecube::serialization::access;
    template<class Archive>
    void save(
        Archive & /* ar */, 
        const unsigned int /* file_version */
    ) const {
        ++(const_cast<B &>(*this).count);
    }
    template<class Archive>
    void load(
        Archive & /* ar */, 
        const unsigned int /* file_version */
    ){
        --count;
    }
    int count;
public:
    B() : count(0) {}
    ~B() {
        ENSURE(0 == count);
    }
};
}

// function specializations must be defined in the appropriate
// namespace - icecube::serialization
namespace icecube {
namespace serialization {

template<class Archive>
void serialize(
    Archive & ar,
    B & b,
    const unsigned int file_version
){ 
    icecube::serialization::split_member(ar, b, file_version);
} 

} // serialization
} // icecube

namespace{
class C
{
public:
    int count;
    C() : count(0) {}
    ~C() {
        ENSURE(0 == count);
    }
};
}

namespace icecube {
namespace serialization {

template<class Archive>
void save(
    Archive & /* ar */, 
    const C & c,
    const unsigned int /* file_version */
){
    ++(const_cast<C &>(c).count);
}

template<class Archive>
void load(
    Archive & /* ar */, 
    C & c,
    const unsigned int /* file_version */
){
    --c.count;
}

} // serialization
} // icecube

I3_SERIALIZATION_SPLIT_FREE(C)

template <typename TS /*test settings*/>
void out(const char* testfile, A & a, B & b, C & c)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(a);
    oa << I3_SERIALIZATION_NVP(b);
    oa << I3_SERIALIZATION_NVP(c);
}

template <typename TS /*test settings*/>
void in(const char* testfile, A & a, B & b, C & c)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(a);
    ia >> I3_SERIALIZATION_NVP(b);
    ia >> I3_SERIALIZATION_NVP(c);
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_split");

    A a;
    B b;
    C c;

    out<TS>(testfile.c_str(), a, b, c);
    in<TS>(testfile.c_str(), a, b, c);
    std::remove(testfile.c_str());
}

TEST_GROUP(test_split)

#define TEST_SET(name) \
TEST(name ## _test_split){ \
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
