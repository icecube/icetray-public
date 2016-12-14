/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_contained_class.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef>
#include <fstream>

#include <cstdio> // remove
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <serialization/nvp.hpp>

#include "B.hpp"
#include "A.ipp"

#include <I3Test.h>

///////////////////////////////////////////////////////
// Contained class
namespace{
class C
{
private:
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_NVP(b);
    }
    B b;
public:
    bool operator==(const C &rhs) const
    {
        return b == rhs.b;
    }
    C(){}
};
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_contained_class");

    const C c;
    C c1;
    {   
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << icecube::serialization::make_nvp("c", c);
    }
    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> icecube::serialization::make_nvp("c", c1);
    }
    ENSURE(c == c1);
    std::remove(testfile.c_str());
}

TEST_GROUP(contained_class)

#define TEST_SET(name) \
TEST(name ## _contained_class){ \
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
