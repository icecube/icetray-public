/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_tracking_save.cpp

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

#include <I3Test.h>
#include <serialization/tracking.hpp>
#include <serialization/tracking_enum.hpp>
#include <serialization/nvp.hpp>

#define TEST_CLASS(N, TRACKING) \
namespace{ \
class N \
{ \
    friend class icecube::serialization::access; \
    template<class Archive> \
    void serialize(Archive & /* ar */, const unsigned int /* file_version */){ \
        ++count; \
    } \
public: \
    static unsigned int count; \
    static void reset_count(){ count=0; } \
}; \
unsigned int N::count = 0; \
} \
I3_CLASS_TRACKING(N, TRACKING)

TEST_CLASS(AN, ::icecube::serialization::track_never)
TEST_CLASS(AS, ::icecube::serialization::track_selectively)
TEST_CLASS(AA, ::icecube::serialization::track_always)

// test pointers
TEST_CLASS(PAN, ::icecube::serialization::track_never)
TEST_CLASS(PAS, ::icecube::serialization::track_selectively)
TEST_CLASS(PAA, ::icecube::serialization::track_always)

template <typename TS /*test settings*/>
void out(const char* testfile){
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    // reset counts in case other tests have already run and set them
    AN::count = 0;
    AS::count = 0;
    AA::count = 0;
    // write object twice to check tracking
    AN an;
    AS as;
    AA aa;
    oa << I3_SERIALIZATION_NVP(an) << I3_SERIALIZATION_NVP(an);
    ENSURE_EQUAL(an.count,2u);
    oa << I3_SERIALIZATION_NVP(as) << I3_SERIALIZATION_NVP(as);
    ENSURE_EQUAL(as.count,2u);
    oa << I3_SERIALIZATION_NVP(aa) << I3_SERIALIZATION_NVP(aa);
    ENSURE_EQUAL(aa.count,1u);

    PAN::count = 0;
    PAS::count = 0;
    PAA::count = 0;
    PAN *pan = new PAN;
    PAS *pas = new PAS;
    PAA *paa = new PAA;
    oa << I3_SERIALIZATION_NVP(pan) << I3_SERIALIZATION_NVP(pan);
    ENSURE_EQUAL(pan->count,2u);
    oa << I3_SERIALIZATION_NVP(pas) << I3_SERIALIZATION_NVP(pas);
    ENSURE_EQUAL(pas->count,1u);
    oa << I3_SERIALIZATION_NVP(paa) << I3_SERIALIZATION_NVP(paa);
    ENSURE_EQUAL(paa->count,1u);
    delete pan;
    delete pas;
    delete paa;
}

template <typename TS /*test settings*/>
void in(const char* testfile){
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    // read object twice to check tracking
    AN an;
    AS as;
    AA aa;

    AN::count = 0;
    AS::count = 0;
    AA::count = 0;

    ia >> I3_SERIALIZATION_NVP(an) >> I3_SERIALIZATION_NVP(an);
    ENSURE(an.count == 2);
    ia >> I3_SERIALIZATION_NVP(as) >> I3_SERIALIZATION_NVP(as);
    ENSURE(as.count == 2);
    ia >> I3_SERIALIZATION_NVP(aa) >> I3_SERIALIZATION_NVP(aa);
    ENSURE(aa.count == 1);

    PAN::count = 0;
    PAS::count = 0;
    PAA::count = 0;

    PAN *pan = NULL;
    PAS *pas = NULL;
    PAA *paa = NULL;
    ia >> I3_SERIALIZATION_NVP(pan);
    ia >> I3_SERIALIZATION_NVP(pan);
    ENSURE(pan->count == 2);
    ia >> I3_SERIALIZATION_NVP(pas);
    ia >> I3_SERIALIZATION_NVP(pas);
    ENSURE(pas->count == 1);
    ia >> I3_SERIALIZATION_NVP(paa);
    ia >> I3_SERIALIZATION_NVP(paa);
    ENSURE(paa->count == 1);
    delete pan;
    delete pas;
    delete paa;
}

TEST_GROUP(tracking)

#define TEST_SET(name) \
TEST(name ## _test_tracking){ \
    auto testfile = I3Test::testfile("test_tracking_" BOOST_PP_STRINGIZE(name) ); \
    out<test_settings>(testfile.c_str()); \
    in<test_settings>(testfile.c_str()); \
    std::remove(testfile.c_str()); \
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
