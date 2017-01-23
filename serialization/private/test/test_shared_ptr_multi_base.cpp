/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_shared_ptr_multi_base.cpp

// (C) Copyright 2002 Robert Ramey- http://www.rrsd.com and Takatoshi Kondo.
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
struct Base1 {
    Base1() {}
    Base1(int x) : m_x(1 + x) {}
    virtual ~Base1(){}
    int m_x;
    // serialize
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */)
    {
        ar & I3_SERIALIZATION_NVP(m_x);
    }
};

struct Base2 {
    Base2() {}
    Base2(int x) : m_x(2 + x) {}
    int m_x;
    virtual ~Base2(){}
    // serialize
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */)
    {
        ar & I3_SERIALIZATION_NVP(m_x);
    }
};

struct Base3 {
    Base3() {}
    Base3(int x) : m_x(3 + x) {}
    virtual ~Base3(){}
    int m_x;
    // serialize
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */)
    {
        ar & I3_SERIALIZATION_NVP(m_x);
    }
};

// Sub is a subclass of Base1, Base1 and Base3.
struct Sub:public Base1, public Base2, public Base3 {
    static int count;
    Sub() {
        ++count;
    }
    Sub(int x) :
        Base1(x),
        Base2(x),
        m_x(x)
    {   
        ++count;
    }
    Sub(const Sub & rhs) :
        m_x(rhs.m_x)
    {
        ++count;
    }
    virtual ~Sub() {
        assert(0 < count);
        --count;
    }
    int m_x;
    // serialize
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */)
    {
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(Base1);
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(Base2);
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(Base3);
        ar & I3_SERIALIZATION_NVP(m_x);
    }
};
}

// Sub needs to be exported because its serialized via a base class pointer
I3_CLASS_EXPORT(Sub)
I3_SERIALIZATION_SHARED_PTR(Sub)

int Sub::count = 0;

template <typename TS /*test settings*/, class FIRST, class SECOND>
void save2(
    const char* testfile,
    const FIRST& first, 
    const SECOND& second
){
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(first);
    oa << I3_SERIALIZATION_NVP(second);
}

template <typename TS /*test settings*/, class FIRST, class SECOND>
void load2(
    const char* testfile,
    FIRST& first, 
    SECOND& second)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(first);
    ia >> I3_SERIALIZATION_NVP(second);
}

// Run tests by serializing two shared_ptrs into an archive,
// clearing them (deleting the objects) and then reloading the
// objects back from an archive.

template<class T, class U>
boost::shared_ptr<T> my_dynamic_pointer_cast(boost::shared_ptr<U> const& u)
{
    return boost::dynamic_pointer_cast<T>(u);
}

template<class T, class U>
std::shared_ptr<T> my_dynamic_pointer_cast(std::shared_ptr<U> const& u)
{
    return std::dynamic_pointer_cast<T>(u);
}

// Serialization sequence
// First,  shared_ptr
// Second, weak_ptr
template <typename TS /*test settings*/, class SP, class WP>
void shared_weak(
    SP& first,
    WP& second
){
    auto testfile = I3Test::testfile("shared_weak");
    int firstm = first->m_x;
    
    ENSURE(!second.expired());
    int secondm = second.lock()->m_x;
    save2<TS>(testfile.c_str(), first, second);

    // Clear the pointers, thereby destroying the objects they contain
    second.reset();
    first.reset();

    load2<TS>(testfile.c_str(), first, second);
    ENSURE(! second.expired());

    // Check data member
    ENSURE(firstm == first->m_x);
    ENSURE(secondm == second.lock()->m_x);
    // Check pointer to vtable
    ENSURE(!!my_dynamic_pointer_cast<Sub>(first));
    ENSURE(!!my_dynamic_pointer_cast<Sub>(second.lock()));

    std::remove(testfile.c_str());
}

// Serialization sequence
// First,  weak_ptr
// Second, shared_ptr
template <typename TS /*test settings*/, class WP, class SP>
void weak_shared(
    WP& first,
    SP& second
){
    auto testfile = I3Test::testfile("weak_shared");
    ENSURE(!first.expired());
    int firstm = first.lock()->m_x;
    int secondm = second->m_x;
    save2<TS>(testfile.c_str(), first, second);

    // Clear the pointers, thereby destroying the objects they contain
    first.reset();
    second.reset();

    load2<TS>(testfile.c_str(), first, second);
    ENSURE(!first.expired());

    // Check data member
    ENSURE(firstm == first.lock()->m_x);
    ENSURE(secondm == second->m_x);
    // Check pointer to vtable
    ENSURE(!!my_dynamic_pointer_cast<Sub>(first.lock()));
    ENSURE(!!my_dynamic_pointer_cast<Sub>(second));

    std::remove(testfile.c_str());
}

// This does the tests
template<typename TS /*test settings*/, template<class T> class SPT, template<class T> class WPT>
void test(){
    // Both Sub
    SPT<Sub> tc1_sp(new Sub(10));
    WPT<Sub> tc1_wp(tc1_sp);
    shared_weak<TS>(tc1_sp, tc1_wp);
    weak_shared<TS>(tc1_wp, tc1_sp);
    tc1_sp.reset();
    ENSURE(0 == Sub::count);

    // Sub and Base1
    SPT<Sub> tc2_sp(new Sub(10));
    WPT<Base1> tc2_wp(tc2_sp);
    shared_weak<TS>(tc2_sp, tc2_wp);
    weak_shared<TS>(tc2_wp, tc2_sp);
    tc2_sp.reset();
    ENSURE(0 == Sub::count);

    // Sub and Base2
    SPT<Sub> tc3_sp(new Sub(10));
    WPT<Base2> tc3_wp(tc3_sp);
    shared_weak<TS>(tc3_sp, tc3_wp);
    weak_shared<TS>(tc3_wp, tc3_sp);
    tc3_sp.reset();
    ENSURE(0 == Sub::count);

    // Sub and Base3
    SPT<Sub> tc4_sp(new Sub(10));
    WPT<Base3> tc4_wp(tc4_sp);
    shared_weak<TS>(tc4_sp, tc4_wp);
    weak_shared<TS>(tc4_wp, tc4_sp);
    tc4_sp.reset();
    ENSURE(0 == Sub::count);

    // Base1 and Base2
    SPT<Sub> tc5_sp_tmp(new Sub(10));
    SPT<Base1> tc5_sp(tc5_sp_tmp);
    WPT<Base2> tc5_wp(tc5_sp_tmp);
    tc5_sp_tmp.reset();
    shared_weak<TS>(tc5_sp, tc5_wp);
    weak_shared<TS>(tc5_wp, tc5_sp);
    tc5_sp.reset();
    ENSURE(0 == Sub::count);

    // Base2 and Base3
    SPT<Sub> tc6_sp_tmp(new Sub(10));
    SPT<Base2> tc6_sp(tc6_sp_tmp);
    WPT<Base3> tc6_wp(tc6_sp_tmp);
    tc6_sp_tmp.reset();
    shared_weak<TS>(tc6_sp, tc6_wp);
    weak_shared<TS>(tc6_wp, tc6_sp);
    tc6_sp.reset();
    ENSURE(0 == Sub::count);

    // Base3 and Base1
    SPT<Sub> tc7_sp_tmp(new Sub(10));
    SPT<Base3> tc7_sp(tc7_sp_tmp);
    WPT<Base1> tc7_wp(tc7_sp_tmp);
    tc7_sp_tmp.reset();
    shared_weak<TS>(tc7_sp, tc7_wp);
    weak_shared<TS>(tc7_wp, tc7_sp);
    tc7_sp.reset();
    ENSURE(0 == Sub::count);
}

TEST_GROUP(shared_ptr_multi_base)

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
