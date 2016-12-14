//  (C) Copyright 2009 Brian Ravnsgaard and Kenneth Riddile
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version including documentation.

// Test that serialization of std::bitset works.
// Should pass compilation and execution
// 16.09.2004, updated 04.03.2009

#include <I3Test.h>

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>

#include <boost/config.hpp>

#if defined( BOOST_NO_STDC_NAMESPACE )
namespace std
{ 
    using ::remove;
}
#endif

#include <serialization/bitset.hpp>
#include <serialization/nvp.hpp>

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_bitset");

    std::bitset<8> bitsetA;
    bitsetA.set( 0, false );
    bitsetA.set( 1, true  );
    bitsetA.set( 2, false );
    bitsetA.set( 3, true  );
    bitsetA.set( 4, false );
    bitsetA.set( 5, false );
    bitsetA.set( 6, true  );
    bitsetA.set( 7, true  );
    
    {
        typename TS::test_ostream os( testfile, TS::TEST_STREAM_FLAGS );
        typename TS::test_oarchive oa( os, TS::TEST_ARCHIVE_FLAGS );
        oa << icecube::serialization::make_nvp( "bitset", bitsetA );
    }
    
    std::bitset<8> bitsetB;
    {
        typename TS::test_istream is( testfile, TS::TEST_STREAM_FLAGS );
        typename TS::test_iarchive ia( is, TS::TEST_ARCHIVE_FLAGS );
        ia >> icecube::serialization::make_nvp( "bitset", bitsetB );
    }
    
    ENSURE( bitsetA == bitsetB );
    
    std::remove(testfile.c_str());
}

TEST_GROUP(bitset)

#define TEST_SET(name) \
TEST(name ## _std_bitset){ \
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
