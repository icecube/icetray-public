/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_mi.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test of serialization of base classes with multiple inheritance
// contributed by Peter Dimov

#include <cstddef>
#include <iostream>
#include <fstream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <I3Test.h>

#include <serialization/nvp.hpp>
#include <serialization/base_object.hpp>

namespace{
class A{
private:
    friend class icecube::serialization::access;
    int x;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_NVP(x);
    }
public:
    explicit A(int x = 0): x(x) {}
    virtual ~A(){}
    int get_x() const{
        return x;
    }
};

class B{
private:
    int y;
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_NVP(y);
    }
public:
    explicit B(int y = 0): y(y) {}
    virtual ~B(){}
    int get_y() const{
        return y;
    }
};

class C: public A, public B{
private:
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(A);
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(B);
    }
public:
    C(){}
    C(int x, int y): A(x), B(y){}
};
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_mi");

    C* pc = new C(1, 2);
    A* pa = pc;
    B* pb = pc;

    ENSURE(pa == pc);
    ENSURE(pb == pc);

    int x = pa->get_x();
    int y = pb->get_y();

    std::cout << "pa->get_x(): " << pa->get_x() << std::endl;
    std::cout << "pb->get_y(): " << pb->get_y() << std::endl;

    {
        typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
        oa << I3_SERIALIZATION_NVP(pc);
        oa << I3_SERIALIZATION_NVP(pa);
        oa << I3_SERIALIZATION_NVP(pb);
    }

    delete pc;
    pc = NULL;
    pa = NULL;
    pb = NULL;

    {
        typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
        typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
        ia >> I3_SERIALIZATION_NVP(pc);
        ia >> I3_SERIALIZATION_NVP(pa);
        ia >> I3_SERIALIZATION_NVP(pb);
    }

    ENSURE(pa == pc);
    ENSURE(pb == pc);

    ENSURE(x == pa->get_x());
    ENSURE(y == pb->get_y());

    std::cout << "pa->get_x(): " << pa->get_x() << std::endl;
    std::cout << "pb->get_y(): " << pb->get_y() << std::endl;

    delete pc;
    std::remove(testfile.c_str());
}

TEST_GROUP(mi)

#define TEST_SET(name) \
TEST(name ## _multiple_inheritance){ \
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
