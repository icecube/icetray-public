/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_enable_shared_from_this.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This demonstrates a problem with boost::serialization and boost::enable_shared_from_this.
// (boost version 1.53)
// See boost TRAC ticket #9567
//
// Given the following class structure:
//    Base is a simple class
//    Derived inherits from Base
//    Derived also inherits from boost::enable_shared_from_this<Derived>
//    Base and Derived implement boost::serialization
//
// When deserializing an instance of Derived into a vector of boost::shared_ptr<Derived>:
//    Base and Derived members are reconstructed correctly.
//    Derived::shared_from_this() works as expected.
//
// But when deserializing an instance of Derived into a vector of boost::shared_ptr<Base>:
//    Base and Derived members are still reconstructed correctly.
//    Derived::shared_from_this() throws a bad_weak_ptr exception.
//    This is because enable_shared_from_this::weak_ptr is NOT reconstructed - It is zero.

#include <sstream>
#include <set>

#include <archive/text_oarchive.hpp>
#include <archive/text_iarchive.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <serialization/shared_ptr.hpp>
#include <serialization/export.hpp>
#include <serialization/split_free.hpp>

#include <I3Test.h>

namespace icecube {
namespace serialization {

struct enable_shared_from_this_helper {
    std::set<boost::shared_ptr<void> > m_esfth;
    void record(boost::shared_ptr<void>  sp){
        m_esfth.insert(sp);
    }
};

template<class Archive, class T>
void serialize(
    Archive & ar,
    boost::enable_shared_from_this<T> & t,
    const unsigned int file_version
){
    enable_shared_from_this_helper & h =
        ar.template get_helper<
            enable_shared_from_this_helper
        >();

    boost::shared_ptr<T> sp = t.shared_from_this();
    h.record(sp);
}

} // serialization
} // icecube

class Base {
    friend class icecube::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & I3_SERIALIZATION_NVP(m_base);
    }
public:
    Base() {}
    virtual ~Base() {}    // "virtual" forces RTTI, to enable serialization of Derived from Base pointer
    int m_base;
};

class Derived : public Base, public boost::enable_shared_from_this<Derived> {
    friend class icecube::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(Base);
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(
            boost::enable_shared_from_this<Derived>
        );
        ar & I3_SERIALIZATION_NVP(m_derived);
    }
public:
    boost::shared_ptr<Derived> SharedPtr() { return shared_from_this(); }
    int m_derived;
    Derived() {}
    ~Derived() {}
};
// The following is required to enable serialization from Base pointer
I3_CLASS_EXPORT(Derived)

// This test passes
template <typename TS /*test settings*/>
void test_passes(){
    std::stringstream ss;
    {
        boost::shared_ptr<Derived> d(new Derived());
        d->m_base = 1;
        d->m_derived = 2;

        // Get a raw pointer to Derived
        Derived* raw_d = d.get();

        // Verify base and derived members
        ENSURE(raw_d->m_base==1);
        ENSURE(raw_d->m_derived==2);

        // verify shared_from_this
        ENSURE(d == raw_d->SharedPtr());

        icecube::archive::text_oarchive oa(ss);
        oa & I3_SERIALIZATION_NVP(d);
    }
    {
        // Deserialize it back into a vector of shared_ptr<Derived>
        boost::shared_ptr<Derived> d;
        ss.seekg(0);
        icecube::archive::text_iarchive ia(ss);
        ia & I3_SERIALIZATION_NVP(d);

        // Get a raw pointer to Derived
        Derived* raw_d = d.get();

        // Verify base and derived members
        ENSURE(raw_d->m_base==1);
        ENSURE(raw_d->m_derived==2);

        // verify shared_from_this
        ENSURE(d == raw_d->SharedPtr());
    }    
}

// This test fails
template <typename TS /*test settings*/>
void test_fails(){
    std::stringstream ss;
    {
        boost::shared_ptr<Base> b(new Derived());
        Derived* raw_d1 = static_cast<Derived*>(b.get());
        raw_d1->m_base = 1;
        raw_d1->m_derived = 2;

        // Get a raw pointer to Derived via shared_ptr<Base>
        Derived* raw_d = static_cast<Derived*>(b.get());

        // Verify base and derived members
        ENSURE(raw_d->m_base==1);
        ENSURE(raw_d->m_derived==2);

        // verify shared_from_this
        boost::shared_ptr<Derived> d = raw_d->SharedPtr();
        ENSURE(d == b);
        // Serialize the vector
        icecube::archive::text_oarchive oa(ss);
        oa & I3_SERIALIZATION_NVP(b);
    }
    {
        // Deserialize it back into a vector of shared_ptr<Base>
        ss.seekg(0);
        icecube::archive::text_iarchive ia(ss);
        boost::shared_ptr<Base> b;
        ia & I3_SERIALIZATION_NVP(b);

        // Get a raw pointer to Derived via shared_ptr<Base>
        Derived* raw_d = static_cast<Derived*>(b.get());
        // Verify base and derived members
        ENSURE(raw_d->m_base==1);
        ENSURE(raw_d->m_derived==2);

        // verify shared_from_this
        // FAIL: The following line throws bad_weak_ptr exception 
        boost::shared_ptr<Derived> d = raw_d->SharedPtr();
        ENSURE(d == b);
    }    
}

TEST_GROUP(test_enable_shared_from_this)

#define TEST_SET(name) \
TEST(name ## _enable_shared_from_this_pass){ \
    test_passes<test_settings>(); \
} \
TEST(name ## _enable_shared_from_this_fail){ \
    test_fails<test_settings>(); \
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
