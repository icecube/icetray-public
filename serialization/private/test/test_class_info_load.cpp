/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_class_info_load.cpp: test implementation level trait

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test implementation level "object_class_info"
// should pass compilation and execution

#include <string>
#include <fstream>

#include <boost/preprocessor/stringize.hpp>

#include <serialization/level.hpp>
#include <serialization/tracking.hpp>
#include <serialization/version.hpp>
#include <serialization/nvp.hpp>

#include <I3Test.h>

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
class B
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & /*ar*/, const unsigned int file_version){
        // verify at execution that the version number corresponds to the saved
        // one
        ENSURE(file_version == 2);
        ++count;
    }
public:
    unsigned int count;
    B() : count(0) {}
};

I3_CLASS_IMPLEMENTATION(B, ::icecube::serialization::object_class_info)
I3_CLASS_TRACKING(B, ::icecube::serialization::track_never)
I3_CLASS_VERSION(B, 4)

template <typename TS /*test settings*/>
void in(const char *testfile, A & a, B & b)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(a);
    ia >> I3_SERIALIZATION_NVP(a);
    ENSURE(a.count == 2);  // no tracking => redundant loads
    ia >> I3_SERIALIZATION_NVP(b);
    ia >> I3_SERIALIZATION_NVP(b);
    // note: archive was saved with tracking so that is what determines
    // whether tracking is perform on load - regardless of the latest
    // tracking setting.
    ENSURE(b.count == 1);
}

TEST_GROUP(class_info_load)

#define TEST_SET(name)\
TEST(name ## _test_class_info_load){ \
    A a; \
    B b; \
    std::string filename = I3Test::testfile(BOOST_PP_STRINGIZE(name) "_class_info_load_" BOOST_PP_STRINGIZE(I3_ARCHIVE_TEST)); \
    in<test_settings>(filename.c_str(), a, b); \
    std::remove(filename.c_str()); \
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
