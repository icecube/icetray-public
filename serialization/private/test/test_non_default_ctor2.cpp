/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_non_default_ctor2.cpp

// (C) Copyright 2002 Martin Ecker. 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

// this tests complex usage of non-default constructor.  Specifically
// the case where a constructor serializes a pointer member.

#include <fstream>

#include <boost/config.hpp>

#include <serialization/nvp.hpp>

#include <I3Test.h>

class IntValueHolder
{
public:
    IntValueHolder()
        : value(0)
    {}

    IntValueHolder(int newvalue)
        : value(newvalue)
    {}

    int GetValue() const { return value; }

private:
    int value;

    friend class icecube::serialization::access;

    template <class ArchiveT>
    void serialize(ArchiveT& ar, const unsigned int /* file_version */)
    {
        ar & I3_SERIALIZATION_NVP(value);
    }
};

class FloatValueHolder
{
public:
    FloatValueHolder()
        : value(0)
    {}

    FloatValueHolder(float newvalue)
        : value(newvalue)
    {}

    float GetValue() const { return value; }

private:
    float value;

    friend class icecube::serialization::access;

    template <class ArchiveT>
    void serialize(ArchiveT& ar, const unsigned int /* file_version */)
    {
        ar & I3_SERIALIZATION_NVP(value);
    }
};

namespace{
class A
{
public:
    A(const IntValueHolder& initialValue)
        : value(initialValue), floatValue(new FloatValueHolder(10.0f))
    {}

    ~A()
    {
        delete floatValue;
    }

    IntValueHolder value;
    FloatValueHolder* floatValue;

private:
    friend class icecube::serialization::access;

    template <class ArchiveT>
    void serialize(ArchiveT& ar, const unsigned int /* file_version */)
    {
        ar & I3_SERIALIZATION_NVP(floatValue);
    }
};
}

namespace icecube {
namespace serialization {

template <class ArchiveT>
void save_construct_data(
    ArchiveT& archive, 
    const A* p, 
    const I3_PFTO unsigned int /*version*/
){
    archive & icecube::serialization::make_nvp("initialValue", p->value);
}

template <class ArchiveT>
void load_construct_data(
    ArchiveT& archive, 
    A* p, 
    const unsigned int /*version*/
){
    IntValueHolder initialValue;
    archive & icecube::serialization::make_nvp("initialValue", initialValue);

    ::new (p) A(initialValue);
}

} // serialization
} // icecube

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_non_default_ctor_2");
    A* a = new A(5);

    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << I3_SERIALIZATION_NVP(a);
    }

    A* a_new;

    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> I3_SERIALIZATION_NVP(a_new);
    }
    ENSURE(a->value.GetValue() == a_new->value.GetValue());
    ENSURE(a->floatValue->GetValue() == a_new->floatValue->GetValue());
    delete a;
    delete a_new;
}

TEST_GROUP(test_non_default_ctor_2)

#define TEST_SET(name) \
TEST(name ## _test_non_default_ctor_2){ \
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
