/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_null_ptr.cpp: test implementation level trait

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
#include <serialization/base_object.hpp>
#include <serialization/nvp.hpp>

namespace{
class polymorphic_base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & /* ar */, const unsigned int /* file_version */){
    }
public:
    virtual ~polymorphic_base(){};
};

class polymorphic_derived1 : public polymorphic_base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(polymorphic_base);
    }
};
}

// save unregistered polymorphic classes
template <typename TS /*test settings*/>
void save(const char* testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_base* rb1 =  nullptr;
    polymorphic_derived1* rd1 = nullptr;
    
    oa << I3_SERIALIZATION_NVP(rb1);
    oa << I3_SERIALIZATION_NVP(rd1);
}

// load unregistered polymorphic classes
template <typename TS /*test settings*/>
void load(const char* testfile)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_derived1 dummy;

    polymorphic_base* rb1 = &dummy;
    polymorphic_derived1* rd1 = &dummy;

    ia >> I3_SERIALIZATION_NVP(rb1);
    ENSURE(nullptr == rb1, "Null pointer not restored");

    ia >> I3_SERIALIZATION_NVP(rd1);
    ENSURE(nullptr == rd1, "Null pointer not restored");

    delete rb1;
    delete rd1;
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_complex");
    save<TS>(testfile.c_str());
    load<TS>(testfile.c_str());
    std::remove(testfile.c_str());
}

TEST_GROUP(test_null_ptr)

#define TEST_SET(name) \
TEST(name ## _null_ptr){ \
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
