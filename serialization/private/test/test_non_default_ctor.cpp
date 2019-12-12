/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_non_default_ctor.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

// this tests:
// a) non-intrusive method of implementing serialization
// b) usage of a non-default constructor

#include <cstddef> // NULL
#include <cstdio>  // remove()
#include <fstream>
#include <cstdlib> // for rand()
#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/limits.hpp>
#include <boost/math/special_functions/next.hpp>

#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::rand; 
    using ::remove;
    #if BOOST_WORKAROUND(BOOST_MSVC, >= 1400) && !defined(UNDER_CE)
        using ::numeric_limits;
    #endif
}
#endif

#include <archive/archive_exception.hpp>
#include <serialization/access.hpp>
#include <serialization/nvp.hpp>
#include <I3Test.h>

namespace{
///////////////////////////////////////////////////////
// simple class test - using non-intrusive syntax
// illustrates the usage of the non-intrusve syntax
class A
{
    friend class icecube::serialization::access;

    // note const values can only be initialized with a non
    // non-default constructor
    const int i;

    signed char s;
    unsigned char t;
    signed int u;
    unsigned int v;
    float w;
    double x;

    template<class Archive>
    void serialize(Archive & ar,const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_NVP(s);
        ar & I3_SERIALIZATION_NVP(t);
        ar & I3_SERIALIZATION_NVP(u);
        ar & I3_SERIALIZATION_NVP(v);
        ar & I3_SERIALIZATION_NVP(w);
        ar & I3_SERIALIZATION_NVP(x);
    }
    A(const A & rhs);
    A & operator=(const A & rhs);
public:
    static int count;
    const int & get_i() const {
        return i;
    }
    A(int i_);
    ~A();
};

int A::count = 0;

A::A(int i_) : 
    i(i_),
    s(static_cast<signed char>(0xff & std::rand())),
    t(static_cast<signed char>(0xff & std::rand())),
    u(std::rand()),
    v(std::rand()),
    w((float)std::rand() / std::rand()),
    x((double)std::rand() / std::rand())
{
    ++count;
}

A::~A(){
    --count;
}


}

namespace icecube {
namespace serialization {

template<class Archive>
inline void save_construct_data(
    Archive & ar, 
    const A * a, 
    const I3_PFTO unsigned int /* file_version */
){
    // variable used for construction
    ar << icecube::serialization::make_nvp("i", a->get_i());
}

template<class Archive>
inline void load_construct_data(
    Archive & ar, 
    A * a, 
    const unsigned int /* file_version */
){
    int i;
    ar >> icecube::serialization::make_nvp("i", i);
    ::new(a)A(i);
}

} // serialization
} // namespace boost

template <typename TS /*test settings*/>
void save(const char* testfile){
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    A a(2);

    oa << I3_SERIALIZATION_NVP(a);
    
    // save a copy pointer to this item
    A* pa1 = &a;
    oa << I3_SERIALIZATION_NVP(pa1);

    // save pointer to a new object
    A* pa2 = new A(4);
    oa << I3_SERIALIZATION_NVP(pa2);

    delete pa2;
}

template <typename TS /*test settings*/>
void load(const char * testfile){
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    A a(4);
    ia >> I3_SERIALIZATION_NVP(a);

    A* pa1;
    ia >> I3_SERIALIZATION_NVP(pa1);
    ENSURE(pa1 == &a, "Copy of pointer not correctly restored");

    A* pa2;
    ia >> I3_SERIALIZATION_NVP(pa2);
    ENSURE(pa2 != &a, "Pointer not correctly restored");

    delete pa2;
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_non_default_ctor");
    save<TS>(testfile.c_str());
    load<TS>(testfile.c_str());
    ENSURE(0 == A::count);
    std::remove(testfile.c_str());
}

TEST_GROUP(test_non_default_ctor)

#define TEST_SET(name) \
TEST(name ## _test_non_default_ctor){ \
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
