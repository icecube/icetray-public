/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_shared_ptr.cpp

// (C) Copyright 2002 Robert Ramey- http://www.rrsd.com - David Tonge  . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <serialization/nvp.hpp>
#include <serialization/export.hpp>
#include <serialization/shared_ptr.hpp>
#include <serialization/weak_ptr.hpp>

#include <I3Test.h>

namespace{
// This is a simple class.  It contains a counter of the number
// of objects of this class which have been instantiated.
class A{
private:
    friend class icecube::serialization::access;
    int x;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_NVP(x);
    }
    A(A const & rhs);
    A& operator=(A const& rhs);
public:
    static int count;
    bool operator==(const A& rhs) const {
        return x == rhs.x;
    }
    A(){++count;}    // default constructor
    A(int x):x(x){++count;}
    virtual ~A(){--count;}   // destructor
};

int A::count = 0;
}

I3_SERIALIZATION_SHARED_PTR(A)

namespace{
// B is a subclass of A
class B : public A
{
private:
    friend class icecube::serialization::access;
    int y;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(A);
        ar & I3_SERIALIZATION_NVP(y);
    }
public:
    static int count;
    B(){};
    B(int x):A(x){};
    virtual ~B() {};
};
}

// B needs to be exported because its serialized via a base class pointer
I3_CLASS_EXPORT(B)
I3_SERIALIZATION_SHARED_PTR(B)

namespace{
// test a non-polymorphic class
class C
{
private:
    friend class icecube::serialization::access;
    int z;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_NVP(z);
    }
public:
    static int count;
    bool operator==(const C& rhs) const {
        return z == rhs.z;
    }
    C() :
        z(++count)    // default constructor
    {}
    virtual ~C(){--count;}   // destructor
};

int C::count = 0;
}

I3_CLASS_EXPORT(C)
I3_SERIALIZATION_SHARED_PTR(C)

template<typename TS /*test settings*/, class SP>
void save(const char* testfile, const SP& spa)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(spa);
}

template<typename TS /*test settings*/, class SP>
void load(const char* testfile, SP& spa)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(spa);
}

// trivial test
template<typename TS /*test settings*/, class SP>
void save_and_load(SP& spa)
{
    auto testfile = I3Test::testfile("shared_ptr_save_and_load");
    save<TS>(testfile.c_str(), spa);
    SP spa1;
    load<TS>(testfile.c_str(), spa1);

    ENSURE(
        (spa.get() == NULL && spa1.get() == NULL)
        || *spa == *spa1
        );
    std::remove(testfile.c_str());
}

template<typename TS /*test settings*/, class SP>
void save2(
    const char* testfile,
    const SP& first,
    const SP& second
){
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(first);
    oa << I3_SERIALIZATION_NVP(second);
}

template<typename TS /*test settings*/, class SP>
void load2(
    const char* testfile,
    SP& first,
    SP& second)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(first);
    ia >> I3_SERIALIZATION_NVP(second);
}

// Run tests by serializing two shared_ptrs into an archive,
// clearing them (deleting the objects) and then reloading the
// objects back from an archive.
template<typename TS /*test settings*/, class SP>
void save_and_load2(SP & first, SP & second)
{
    auto testfile = I3Test::testfile("shared_ptr_save_and_load2");

    save2<TS>(testfile.c_str(), first, second);

    // Clear the pointers, thereby destroying the objects they contain
    first.reset();
    second.reset();

    load2<TS>(testfile.c_str(), first, second);

    ENSURE(first == second);
    std::remove(testfile.c_str());
}

template<typename TS /*test settings*/, class SP, class WP>
void save3(
    const char* testfile,
    SP& first,
    SP& second,
    WP& third
){
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(third);
    oa << I3_SERIALIZATION_NVP(first);
    oa << I3_SERIALIZATION_NVP(second);
}

template<typename TS /*test settings*/, class SP, class WP>
void load3(
    const char* testfile,
    SP& first,
    SP& second,
    WP& third
){
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    // note that we serialize the weak pointer first
    ia >> I3_SERIALIZATION_NVP(third);
    // in order to test that a temporarily solitary weak pointer
    // correctly restored.
    ia >> I3_SERIALIZATION_NVP(first);
    ia >> I3_SERIALIZATION_NVP(second);
}

template<typename TS /*test settings*/, class SP, class WP>
void save_and_load3(
    SP& first,
    SP& second,
    WP& third
){
    auto testfile = I3Test::testfile("shared_ptr_save_and_load3");

    save3<TS>(testfile.c_str(), first, second, third);

    // Clear the pointers, thereby destroying the objects they contain
    first.reset();
    second.reset();
    third.reset();

    load3<TS>(testfile.c_str(), first, second, third);

    ENSURE(first == second);
    ENSURE(first == third.lock());
    std::remove(testfile.c_str());
}

template<typename TS /*test settings*/, class SP>
void save4(const char* testfile, const SP& spc)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(spc);
}

template<typename TS /*test settings*/, class SP>
void load4(const char* testfile, SP& spc)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(spc);
}

// trivial test
template<typename TS /*test settings*/, class SP>
void save_and_load4(SP & spc)
{
    auto testfile = I3Test::testfile("shared_ptr_save_and_load4");
    save4<TS>(testfile.c_str(), spc);
    SP spc1;
    load4<TS>(testfile.c_str(), spc1);

    ENSURE(
        (spc.get() == NULL && spc1.get() == NULL)
        || *spc == *spc1
    );
    std::remove(testfile.c_str());
}

// This does the tests
template<typename TS /*test settings*/, template<class T> class SPT , template<class T> class WPT >
void test(){
    {
        SPT<A> spa;
        // These are our shared_ptrs
        spa = SPT<A>(new A);
        SPT<A> spa1 = spa;
        spa1 = spa;
    }
    {
        // These are our shared_ptrs
        SPT<A> spa;

        // trivial test 1
        save_and_load<TS>(spa);
    
        //trivival test 2
        spa = SPT<A>(new A(5));
        save_and_load<TS>(spa);

        // Try to save and load pointers to As
        spa = SPT<A>(new A(17));
        SPT<A> spa1 = spa;
        save_and_load2<TS>(spa, spa1);

        // Try to save and load pointers to Bs
        spa = SPT<A>(new B(-12));
        spa1 = spa;
        save_and_load2<TS>(spa, spa1);

        // test a weak pointer
        spa = SPT<A>(new A(2));
        spa1 = spa;
        WPT<A> wp = spa;
        save_and_load3<TS>(spa, spa1, wp);
        
        // obj of type B gets destroyed
        // as smart_ptr goes out of scope
    }
    ENSURE(A::count == 0);
    {
        // Try to save and load pointers to Cs
        SPT<C> spc;
        spc = SPT<C>(new C);
        save_and_load4<TS>(spc);
    }
    ENSURE(C::count == 0);
}

TEST_GROUP(shared_ptr)

#define TEST_SET(name) \
TEST(name ## _boost_shared_ptr){ \
    test<test_settings, boost::shared_ptr, boost::weak_ptr>(); \
} \
TEST(name ## _std_shared_ptr){ \
    test<test_settings, std::shared_ptr, std::weak_ptr>(); \
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
