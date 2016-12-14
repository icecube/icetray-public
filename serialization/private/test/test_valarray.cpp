/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_valarrray.cpp

// (C) Copyright 2005 Matthias Troyer . 
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

#include <I3Test.h>

#include <serialization/valarray.hpp>

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_valarray");

    // test array of objects
    std::valarray<int> avalarray(2);
    avalarray[0] = 42;
    avalarray[1] = -42;
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("avalarray", avalarray);
    }
    std::valarray<int> avalarray1;
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("avalarray", avalarray1);
    }
    bool equal = (avalarray.size() == avalarray1.size()
                   && avalarray[0] == avalarray1[0]
                   && avalarray[1] == avalarray1[1]
                 );
                  
    ENSURE(equal);
    std::remove(testfile.c_str());
}

TEST_GROUP(valarray)

#define TEST_SET(name) \
TEST(name ## _std_valarray){ \
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
