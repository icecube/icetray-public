/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_no_rtti.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// note: this program tests the inter-operability of different
// extended typeinfo systems.  In this example, one class is
// identified using the default RTTI while the other uses a custom
// system based on the export key.
// 
// As this program uses RTTI for one of the types, the test will fail
// on a system for which RTTI is not enabled or not existent.

#include <cstddef>
#include <fstream>
#include <iostream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <archive/archive_exception.hpp>

#include <serialization/type_info_implementation.hpp>
#include <serialization/extended_type_info_no_rtti.hpp>
#include <serialization/export.hpp>
#include <serialization/nvp.hpp>

#include <I3Test.h>

#include "polymorphic_base.hpp"
#include "polymorphic_derived1.hpp"
#include "polymorphic_derived2.hpp"

template<class Archive>
void polymorphic_derived2::serialize(
    Archive &ar, 
    const unsigned int /* file_version */
){
    ar & I3_SERIALIZATION_BASE_OBJECT_NVP(polymorphic_base);
}

/*template void polymorphic_derived2::serialize(
    test_oarchive & ar,
    const unsigned int version
);
template void polymorphic_derived2::serialize(
    test_iarchive & ar,
    const unsigned int version
);*/

// save derived polymorphic class
template <typename TS /*test settings*/>
void save_derived(const char *testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_derived1* rd1 = new polymorphic_derived1;
    polymorphic_derived2* rd2 = new polymorphic_derived2;

    std::cout << "saving polymorphic_derived1 (no_rtti)\n";
    oa << I3_SERIALIZATION_NVP(rd1);

    std::cout << "saving polymorphic_derived2\n";
    oa << I3_SERIALIZATION_NVP(rd2);

    const polymorphic_base *rb1 =  rd1;
    polymorphic_base *rb2 =  rd2;
    std::cout << "saving polymorphic_derived1 (no_rtti) through base (no_rtti)\n";
    oa << I3_SERIALIZATION_NVP(rb1);
    std::cout << "saving polymorphic_derived2 through base\n";
    oa << I3_SERIALIZATION_NVP(rb2);

    delete rd1;
    delete rd2;
}

// load derived polymorphic class
template <typename TS /*test settings*/>
void load_derived(const char *testfile)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_derived1 *rd1 = NULL;
    polymorphic_derived2 *rd2 = NULL;

    std::cout << "loading polymorphic_derived1 (no_rtti)\n";
    ia >> I3_SERIALIZATION_NVP(rd1);
    ENSURE(
        icecube::serialization::type_info_implementation<
            polymorphic_derived1
        >::type::get_const_instance()
        == 
        *icecube::serialization::type_info_implementation<
            polymorphic_derived1
        >::type::get_const_instance().get_derived_extended_type_info(*rd1)
        ,
        "restored pointer d1 not of correct type"
    );

    std::cout << "loading polymorphic_derived2\n";
    ia >> I3_SERIALIZATION_NVP(rd2);
    ENSURE(
        icecube::serialization::type_info_implementation<
            polymorphic_derived2
        >::type::get_const_instance()
        == 
        *icecube::serialization::type_info_implementation<
            polymorphic_derived2
        >::type::get_const_instance().get_derived_extended_type_info(*rd2)
        ,
        "restored pointer d2 not of correct type"
    );
    polymorphic_base *rb1 = NULL;
    polymorphic_base *rb2 = NULL;

    // the above opereration registers the derived classes as a side
    // effect.  Hence, instances can now be correctly serialized through
    // a base class pointer.
    std::cout << "loading polymorphic_derived1 (no_rtti) through base (no_rtti)\n";
    ia >> I3_SERIALIZATION_NVP(rb1);

    ENSURE(
        rb1 == dynamic_cast<polymorphic_base *>(rd1),
        "serialized pointers not correctly restored"
    );

    ENSURE(
        icecube::serialization::type_info_implementation<
            polymorphic_derived1
        >::type::get_const_instance()
        == 
        *icecube::serialization::type_info_implementation<
            polymorphic_base
        >::type::get_const_instance().get_derived_extended_type_info(*rb1)
        ,
        "restored pointer b1 not of correct type"
    );
    std::cout << "loading polymorphic_derived2 through base (no_rtti)\n";
    ia >> I3_SERIALIZATION_NVP(rb2);

    ENSURE(
        rb2 ==  dynamic_cast<polymorphic_base *>(rd2),
        "serialized pointers not correctly restored"
    );
    ENSURE(
        icecube::serialization::type_info_implementation<
            polymorphic_derived2
        >::type::get_const_instance()
        == 
        *icecube::serialization::type_info_implementation<
            polymorphic_base
        >::type::get_const_instance().get_derived_extended_type_info(*rb2)
        ,
        "restored pointer b2 not of correct type"
    );
    delete rb1;
    delete rb2;
}

template <typename TS /*test settings*/>
void do_test(){
    auto testfile = I3Test::testfile("test_mi");

    save_derived<TS>(testfile.c_str());
    load_derived<TS>(testfile.c_str());

    std::remove(testfile.c_str());
}

TEST_GROUP(no_rtti)

#define TEST_SET(name) \
TEST(name ## _no_rtti){ \
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
