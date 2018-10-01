/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_shared_ptr.cpp

// (C) Copyright 2002 Robert Ramey- http://www.rrsd.com - David Tonge  . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org for updates, documentation, and revision history.

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

#include <serialization/shared_ptr_132.hpp>
#include <serialization/shared_ptr.hpp>
#include <serialization/weak_ptr.hpp>
#include <serialization/split_member.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <serialization/nvp.hpp>
#include <serialization/export.hpp>

namespace{
// This is a simple class.  It contains a counter of the number
// of objects of this class which have been instantiated.
class A
{
private:
    friend class icecube::serialization::access;
    int x;
    template<class Archive>
    void save(Archive & ar, const unsigned int /* file_version */) const {
        ar << I3_SERIALIZATION_NVP(x);
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */) {
        ar >> I3_SERIALIZATION_NVP(x);
    }
    I3_SERIALIZATION_SPLIT_MEMBER()
public:
    static int count;
    bool operator==(const A & rhs) const {
        return x == rhs.x;
    }
    A(){++count;}    // default constructor
    virtual ~A(){--count;}   // default destructor
};

I3_SERIALIZATION_SHARED_PTR(A)

// B is a subclass of A
class B : public A
{
private:
    friend class icecube::serialization::access;
    int y;
    template<class Archive>
    void save(Archive & ar, const unsigned int /* file_version */ )const {
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(A);
        ar << I3_SERIALIZATION_NVP(y);
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int /* file_version */){
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(A);
        ar >> I3_SERIALIZATION_NVP(y);
    }
    I3_SERIALIZATION_SPLIT_MEMBER()
public:
    static int count;
    B() : A() {};
    virtual ~B() {};
};
}
// B needs to be exported because its serialized via a base class pointer
I3_SHARED_POINTER_EXPORT(B)
I3_SERIALIZATION_SHARED_PTR(B)
namespace{
int A::count = 0;
}

template<class T, typename TS /*test settings*/>
void save(const char* testfile, const T& spa)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(spa);
}

template<class T, typename TS /*test settings*/>
void load(const char* testfile, T& spa)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(spa);
}

// trivial test
template<class T, typename TS /*test settings*/>
void save_and_load(const T& spa){
    auto testfile = I3Test::testfile("shared_ptr_132");
    save<T,TS>(testfile.c_str(), spa);

    // note that we're loading to a current version of shared_ptr 
    // regardless of the orignal saved type - this tests backward
    // archive compatibility
    boost::shared_ptr<A> spa1;
    load<boost::shared_ptr<A>,TS>(testfile.c_str(), spa1);

    ENSURE(
        (spa.get() == NULL && spa1.get() == NULL)
        || * spa == * spa1
    );
    std::remove(testfile.c_str());
}

template<class T, typename TS /*test settings*/>
void save2(
    const char* testfile,
    const T& first, 
    const T& second
){
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(first);
    oa << I3_SERIALIZATION_NVP(second);
}

template<class T, typename TS /*test settings*/>
void load2(
    const char* testfile,
    T& first, 
    T& second)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(first);
    ia >> I3_SERIALIZATION_NVP(second);
}

// Run tests by serializing two shared_ptrs into an archive,
// clearing them (deleting the objects) and then reloading the
// objects back from an archive.
template<class T, typename TS /*test settings*/>
void save_and_load2(T & first, T & second)
{
    auto testfile = I3Test::testfile("shared_ptr_132_2");

    save2<T,TS>(testfile.c_str(), first, second);

    // Clear the pointers, thereby destroying the objects they contain
    first.reset();
    second.reset();

    boost::shared_ptr<A> first1, second1;
    load2<boost::shared_ptr<A>,TS>(testfile.c_str(), first1, second1);

    ENSURE(first1 == second1);
    std::remove(testfile.c_str());
}

template<class T, typename TS /*test settings*/>
void save3(
    const char* testfile,
    const T& first, 
    const T& second,
    const T& third
){
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(third);
    oa << I3_SERIALIZATION_NVP(first);
    oa << I3_SERIALIZATION_NVP(second);
}

template<class T, typename TS /*test settings*/>
void load3(
    const char* testfile,
    T& first, 
    T& second,
    T& third
){
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    // note that we serialize the weak pointer first
    ia >> I3_SERIALIZATION_NVP(third);
    // inorder to test that a temporarily solitary weak pointer
    // correcttly restored.
    ia >> I3_SERIALIZATION_NVP(first);
    ia >> I3_SERIALIZATION_NVP(second);
}

template <typename TS /*test settings*/>
void do_test(){
    // These are our shared_ptrs
    boost_132::shared_ptr<A> spa;

    // trivial test 1
    save_and_load<boost_132::shared_ptr<A>,TS>(spa);

    //trivial test 2
    spa.reset();
    spa = boost_132::shared_ptr<A>(new A);
    save_and_load<boost_132::shared_ptr<A>,TS>(spa);

    // Try to save and load pointers to As, to a text archive
    spa = boost_132::shared_ptr<A>(new A);
    boost_132::shared_ptr<A> spa1 = spa;
    save_and_load2<boost_132::shared_ptr<A>,TS>(spa, spa1);
    
    // Try to save and load pointers to Bs, to a text archive
    spa = boost_132::shared_ptr<A>(new B);
    save_and_load<boost_132::shared_ptr<A>,TS>(spa);

    spa1 = spa;
    save_and_load2<boost_132::shared_ptr<A>,TS>(spa, spa1);

    // obj of type B gets destroyed
    // as smart_ptr goes out of scope
}

TEST_GROUP(shared_ptr_132)

#define TEST_SET(name) \
TEST(name ## _shared_ptr_132){ \
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
