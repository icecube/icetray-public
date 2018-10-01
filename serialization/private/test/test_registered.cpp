/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_registered.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <cstdio> // remove
#include <fstream>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <archive/archive_exception.hpp>
#include <I3Test.h>

#include <serialization/base_object.hpp>
#include <serialization/nvp.hpp>
#include <serialization/type_info_implementation.hpp>
#include <serialization/extended_type_info_typeid.hpp>

namespace{
class polymorphic_base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & /* ar */, const unsigned int /* file_version */){
    }
public:
    virtual ~polymorphic_base(){};
};

class polymorphic_derived1 : public polymorphic_base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(polymorphic_base);
    }
};

class polymorphic_derived2 : public polymorphic_base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(polymorphic_base);
    }
};
}

// save derived polymorphic class
template <typename TS /*test settings*/>
void save_derived(const char* testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_derived1* rd1 = new polymorphic_derived1;
    polymorphic_derived2* rd2 = new polymorphic_derived2;

    // registration IS necessary when serializing pointers of
    // polymorphic classes
    oa.register_type(static_cast<polymorphic_derived1*>(nullptr));
    oa.register_type(static_cast<polymorphic_derived2*>(nullptr));
    oa << I3_SERIALIZATION_NVP(rd1);
    oa << I3_SERIALIZATION_NVP(rd2);

    // the above opereration registers the derived classes as a side
    // effect.  Hence, instances can now be correctly serialized through
    // a base class pointer.
    polymorphic_base* rb1 =  rd1;
    polymorphic_base* rb2 =  rd2;
    oa << I3_SERIALIZATION_NVP(rb1);
    oa << I3_SERIALIZATION_NVP(rb2);

    delete rd1;
    delete rd2;
}

// save derived polymorphic class
template <typename TS /*test settings*/>
void load_derived(const char* testfile)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_derived1* rd1 = nullptr;
    polymorphic_derived2* rd2 = nullptr;

    // registration IS necessary when serializing pointers of
    // polymorphic classes
    ia.register_type(static_cast<polymorphic_derived1*>(nullptr));
    ia.register_type(static_cast<polymorphic_derived2*>(nullptr));

    ia >> I3_SERIALIZATION_NVP(rd1);

    const icecube::serialization::extended_type_info * p1;
    p1 = & icecube::serialization::type_info_implementation<polymorphic_derived1>
        ::type::get_const_instance();

    ENSURE(nullptr != p1);

    const icecube::serialization::extended_type_info * p2;
    p2 = icecube::serialization::type_info_implementation<polymorphic_derived1>
        ::type::get_const_instance().get_derived_extended_type_info(*rd1);

    ENSURE(nullptr != p2);

    ENSURE(p1 == p2, "restored pointer d1 not of correct type");

    ia >> I3_SERIALIZATION_NVP(rd2);

    ENSURE(
        & icecube::serialization::type_info_implementation<polymorphic_derived2>
            ::type::get_const_instance()
        == 
        icecube::serialization::type_info_implementation<polymorphic_derived2>
            ::type::get_const_instance().get_derived_extended_type_info(*rd2),
        "restored pointer d2 not of correct type"
    );

    polymorphic_base* rb1 = nullptr;
    polymorphic_base* rb2 = nullptr;

    // the above opereration registers the derived classes as a side
    // effect.  Hence, instances can now be correctly serialized through
    // a base class pointer.
    ia >> I3_SERIALIZATION_NVP(rb1);

    ENSURE(
        rb1 == dynamic_cast<polymorphic_base*>(rd1),
        "serialized pointers not correctly restored"
    );

    p1 = & icecube::serialization::type_info_implementation<polymorphic_derived1>
        ::type::get_const_instance();

    ENSURE(nullptr != p1);

    p2 = icecube::serialization::type_info_implementation<polymorphic_base>
        ::type::get_const_instance().get_derived_extended_type_info(*rb1);

    ENSURE(nullptr != p2);

    ENSURE(p1 == p2, "restored pointer b1 not of correct type");

    ia >> I3_SERIALIZATION_NVP(rb2);

    ENSURE(
        rb2 ==  dynamic_cast<polymorphic_base*>(rd2),
        "serialized pointers not correctly restored"
    );

    ENSURE(
        & icecube::serialization::type_info_implementation<polymorphic_derived2>
            ::type::get_const_instance()
        == icecube::serialization::type_info_implementation<polymorphic_base>
            ::type::get_const_instance().get_derived_extended_type_info(*rb2),
        "restored pointer b2 not of correct type"
    );

    delete rb1;
    delete rb2;
}

// save registered polymorphic class
template <typename TS /*test settings*/>
void save_registered(const char *testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_base* rb1 = new polymorphic_derived1;
    polymorphic_base* rb2 = new polymorphic_derived2;

    // registration (forward declaration) will permit correct serialization
    // through a pointer to a base class
    oa.register_type(static_cast<polymorphic_derived1*>(nullptr));
    oa.register_type(static_cast<polymorphic_derived2*>(nullptr));
    oa << I3_SERIALIZATION_NVP(rb1) << I3_SERIALIZATION_NVP(rb2);

    delete rb1;
    delete rb2;
}

// save registered polymorphic class
template <typename TS /*test settings*/>
void load_registered(const char *testfile)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_base* rb1 = nullptr;
    polymorphic_base* rb2 = nullptr;

    // registration (forward declaration) will permit correct serialization
    // through a pointer to a base class
    ia.register_type(static_cast<polymorphic_derived1*>(nullptr));
    ia.register_type(static_cast<polymorphic_derived2*>(nullptr));
    ia >> I3_SERIALIZATION_NVP(rb1) >> I3_SERIALIZATION_NVP(rb2);

    ENSURE(
        & icecube::serialization::type_info_implementation<polymorphic_derived1>
            ::type::get_const_instance()
        == icecube::serialization::type_info_implementation<polymorphic_base>
            ::type::get_const_instance().get_derived_extended_type_info(*rb1),
        "restored pointer b1 not of correct type"
    );

    ENSURE(
        & icecube::serialization::type_info_implementation<polymorphic_derived2>
            ::type::get_const_instance()
        == icecube::serialization::type_info_implementation<polymorphic_base>
            ::type::get_const_instance().get_derived_extended_type_info(*rb2),
        "restored pointer b2 not of correct type"
    );

    delete rb1;
    delete rb2;
}

TEST_GROUP(test_registered)

#define TEST_SET(name) \
TEST(name ## _test_derived){ \
    auto testfile = I3Test::testfile( BOOST_PP_STRINGIZE(name) "_test_derived"); \
    save_derived<test_settings>(testfile.c_str()); \
    load_derived<test_settings>(testfile.c_str()); \
    std::remove(testfile.c_str()); \
} \
TEST(name ## _test_registered){ \
    auto testfile = I3Test::testfile( BOOST_PP_STRINGIZE(name) "_test_registered"); \
    save_registered<test_settings>(testfile.c_str()); \
    load_registered<test_settings>(testfile.c_str()); \
    std::remove(testfile.c_str()); \
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
