/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_pimpl.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <boost/compatibility/cpp_c_headers/cstdio> // for tmpnam

#include <fstream>
#include <serialization/nvp.hpp>

#include <I3Test.h>

class B;

namespace{
class A {
    A():pimpl(new B){}
    
    friend icecube::serialization::access;
    B* pimpl;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version);
    
    bool operator==(const A& other) const{
        return(*pimpl == *other.pimpl);
    }
};
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_pimpl");

    A a, a1;
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << I3_SERIALIZATION_NVP(a);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> I3_SERIALIZATION_NVP(a1);
    }
    ENSURE(a == a1);
}

TEST_GROUP(pimpl)

#define TEST_SET(name) \
TEST(name ## _pimpl){ \
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
