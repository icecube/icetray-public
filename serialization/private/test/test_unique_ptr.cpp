/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_unique_ptr.cpp

// (C) Copyright 2002-14 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <cstdio> // remove, std::auto_ptr interface wrong in dinkumware
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif
#include <serialization/nvp.hpp>

#include <I3Test.h>

/////////////////////////////////////////////////////////////
// test std::unique_ptr serialization
namespace{
class A
{
private:
    friend class icecube::serialization::access;
    int x;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_NVP(x);
    }
public:
    A(){}    // default constructor
    ~A(){}   // default destructor
};
}

#include <serialization/unique_ptr.hpp>

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_unique_ptr");

    // create  a new auto pointer to ta new object of type A
    std::unique_ptr<A> spa(new A);
    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << I3_SERIALIZATION_NVP(spa);
    }
    {
        // reset the unique_ptr to NULL
        // thereby destroying the object of type A
        // note that the reset automagically maintains the reference count
        spa.reset();
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> I3_SERIALIZATION_NVP(spa);
    }
    std::remove(testfile.c_str());
}

TEST_GROUP(unique_ptr)

#define TEST_SET(name) \
TEST(name ## _std_unique_ptr){ \
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
