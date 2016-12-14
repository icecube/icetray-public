/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_class_info_save.cpp: test implementation level trait

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test implementation level "object_class_info"
// should pass compilation and execution

#include <fstream>
#include <string>

#include <boost/preprocessor/stringize.hpp>

#include <serialization/level.hpp>
#include <serialization/version.hpp>
#include <serialization/tracking.hpp>
#include <serialization/nvp.hpp>

#include <I3Test.h>

// first case : serialize WITHOUT class information
class A
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & /*ar*/, const unsigned int file_version){
        // classes that don't save class info always have a version number of 0
        ENSURE(file_version == 0);
        static_assert(0 == ::icecube::serialization::version<A>::value,
                      "classes that don't save class info always have a version number of 0");
        ++count;
    }
public:
    unsigned int count;
    A() : count(0) {}
};

I3_CLASS_IMPLEMENTATION(A, ::icecube::serialization::object_serializable)
I3_CLASS_TRACKING(A, ::icecube::serialization::track_never)

// second case : serialize WITH class information
// note: GCC compile fails if this is after the class declaration
class B;
I3_CLASS_VERSION(B, 2)

class B
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & /*ar*/, const unsigned int file_version){
        // verify at execution that correct version number is passed on save
        ENSURE(
            static_cast<const int>(file_version) 
            == ::icecube::serialization::version<B>::value
        );
        ++count;
    }
public:
    unsigned int count;
    B() : count(0) {}
};

I3_CLASS_IMPLEMENTATION(B, ::icecube::serialization::object_class_info)
I3_CLASS_TRACKING(B, icecube::serialization::track_always)

#include <iostream>

template <typename TS /*test settings*/>
void out(const char *testfile, const A & a, const B & b)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    // write object twice to check tracking
    oa << I3_SERIALIZATION_NVP(a);
    oa << I3_SERIALIZATION_NVP(a);
    ENSURE(a.count == 2);  // no tracking => redundant saves
    oa << I3_SERIALIZATION_NVP(b);
    oa << I3_SERIALIZATION_NVP(b);
    ENSURE(b.count == 1);  // tracking => no redundant saves
}

TEST_GROUP(class_info_save)

#define TEST_SET(name)\
TEST(name ## _test_class_info_save){ \
    A a; \
    B b; \
    std::string filename = I3Test::testfile(BOOST_PP_STRINGIZE(name) "_class_info_load_" BOOST_PP_STRINGIZE(I3_ARCHIVE_TEST)); \
    out<test_settings>(filename.c_str(), a, b); \
}
//do _not_ remove files, as they are needed by test_class_info_load

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
