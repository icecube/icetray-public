/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_exported.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef>
#include <fstream>

#include <boost/config.hpp>
#include <cstdio> // remove
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <serialization/export.hpp>
#include <serialization/base_object.hpp>
#include <serialization/nvp.hpp>
#include <serialization/type_info_implementation.hpp>
#include <serialization/extended_type_info_typeid.hpp>

#include <serialization/force_include.hpp>

#include <archive/archive_exception.hpp>
#include <I3Test.h>

#include "polymorphic_base.hpp"

namespace{
class polymorphic_derived1 : 
    public polymorphic_base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(polymorphic_base);
    }
    virtual const char * get_key() const {
        return "polymorphic_derived1";
    }
public:
    ~polymorphic_derived1(){}
};
}

I3_CLASS_EXPORT(polymorphic_derived1)

// MWerks users can do this to make their code work
I3_SERIALIZATION_MWERKS_BASE_AND_DERIVED(polymorphic_base, polymorphic_derived1)

#include "polymorphic_derived2.hpp"

// MWerks users can do this to make their code work
I3_SERIALIZATION_MWERKS_BASE_AND_DERIVED(polymorphic_base, polymorphic_derived2)

template<class Archive>
void polymorphic_derived2::serialize(
    Archive &ar, 
    const unsigned int // file_version
){
    ar & I3_SERIALIZATION_BASE_OBJECT_NVP(polymorphic_base);
}

I3_CLASS_EXPORT_IMPLEMENT(polymorphic_derived2)

//What was this even for?
/*template EXPORT_DECL(void) polymorphic_derived2::serialize(
    test_oarchive & ar,
    const unsigned int version
);
template EXPORT_DECL(void) polymorphic_derived2::serialize(
    test_iarchive & ar,
    const unsigned int version
);*/

// save exported polymorphic class
template <typename TS /*test settings*/>
void save_exported(const char *testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_base* rb1 = new polymorphic_derived1;
    polymorphic_base* rb2 = new polymorphic_derived2;

    // export will permit correct serialization
    // through a pointer to a base class
    oa << I3_SERIALIZATION_NVP(rb1);
    oa << I3_SERIALIZATION_NVP(rb2);

    delete rb1;
    delete rb2;
}

// save exported polymorphic class
template <typename TS /*test settings*/>
void load_exported(const char *testfile)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_base* rb1 = NULL;
    polymorphic_base* rb2 = NULL;

    // export will permit correct serialization
    // through a pointer to a base class
    ia >> I3_SERIALIZATION_NVP(rb1);
    ENSURE(
        icecube::serialization::type_info_implementation<polymorphic_derived1>
            ::type::get_const_instance()
        == 
        * icecube::serialization::type_info_implementation<polymorphic_base>
            ::type::get_const_instance().get_derived_extended_type_info(*rb1),
        "restored pointer b1 not of correct type"
    );

    ia >> I3_SERIALIZATION_NVP(rb2);
    ENSURE(
        icecube::serialization::type_info_implementation<polymorphic_derived2>
            ::type::get_const_instance()
        == 
        * icecube::serialization::type_info_implementation<polymorphic_base>
            ::type::get_const_instance().get_derived_extended_type_info(*rb2),
        "restored pointer b2 not of correct type"
    );

    delete rb1;
    delete rb2;
}

TEST_GROUP(test_exported)

#define TEST_SET(name) \
TEST(name ## _exported){ \
    auto testfile = I3Test::testfile("test_exported"); \
    save_exported<test_settings>(testfile.c_str()); \
    load_exported<test_settings>(testfile.c_str()); \
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
