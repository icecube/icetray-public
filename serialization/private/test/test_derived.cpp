/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_derived.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <cstddef> // NULL
#include <fstream>

#include <cstdio> // remove
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::remove;
}
#endif

#include <archive/archive_exception.hpp>

#include <serialization/base_object.hpp>
#include <serialization/extended_type_info.hpp>
#include <serialization/nvp.hpp>
#include <serialization/extended_type_info_typeid.hpp> //?
#include <serialization/type_info_implementation.hpp>

#include <I3Test.h>

class base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & /* ar */, const unsigned int /* file_version */){
    }
protected:
    virtual ~base(){};
};

class derived1 : public base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(base);
    }
};

class derived2 : public base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(base);
    }
};

// save non-polymorphic classes through a base class pointer
template <typename TS /*test settings*/>
void save_derived(const char *testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    // registration not necessary when serializing the most derived pointer
    derived1* d1 = new derived1;
    derived2* d2 = new derived2;
    oa << I3_SERIALIZATION_NVP(d1) << I3_SERIALIZATION_NVP(d2);

    // upcasting non-polymorphic pointers may not lead to the expected
    // result. In the current type id system
    base* b1 = d1;
    base* b2 = d2;
    
    // Warning, the current type id system does not yield true
    // type id for non-polymorphic types
    const icecube::serialization::extended_type_info & this_type
        = icecube::serialization::type_info_implementation<base>::type
            ::get_const_instance();
    // retrieve the true type of the object pointed to
    const icecube::serialization::extended_type_info & true_type
        = * icecube::serialization::type_info_implementation<base>::type
            ::get_const_instance().get_derived_extended_type_info(*b1);

    ENSURE(
        !(this_type == true_type),
        "current type id system does not support non-polymorphic types"
    );

    oa << I3_SERIALIZATION_NVP(b1);
    oa << I3_SERIALIZATION_NVP(b2);

    delete d1;
    delete d2;
}

// save non-polymorphic classes through a base class pointer
template <typename TS /*test settings*/>
void load_derived(const char *testfile)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    // registration not necessary when serializing the most derived pointer
    derived1* d1 = NULL;
    derived2* d2 = NULL;
    ia >> I3_SERIALIZATION_NVP(d1) >> I3_SERIALIZATION_NVP(d2);

    // upcasting non-polymorphic pointers may not lead to the expected
    // result. In the current type id system
    base* b1 = NULL;
    base* b2 = NULL;
    
    // note: this will produce incorrect results for non-polymorphic classes
    ia >> I3_SERIALIZATION_NVP(b1);
    ia >> I3_SERIALIZATION_NVP(b2);

    // Warning, the current type id system does not yield true
    // type id for non-polymorphic types
    const icecube::serialization::extended_type_info & this_type
        = icecube::serialization::type_info_implementation<base>::type
            ::get_const_instance();
    // retrieve the true type of the object pointed to
    const icecube::serialization::extended_type_info & true_type
        = * icecube::serialization::type_info_implementation<base>::type
            ::get_const_instance().get_derived_extended_type_info(*b1);
            
    ENSURE(
        !(this_type == true_type),
        "current type id system does fails for non-polymorphic types"
    );

    ENSURE(b1 == static_cast<base*>(d1));
    ENSURE(b2 == static_cast<base*>(d2));

    delete d1;
    delete d2;
}

TEST_GROUP(test_derived)

#define TEST_SET(name) \
TEST(name ## _derived){ \
    auto testfile = I3Test::testfile("test_derived_" BOOST_PP_STRINGIZE(I3_ARCHIVE_TEST)); \
    save_derived<test_settings>(testfile.c_str()); \
    load_derived<test_settings>(testfile.c_str()); \
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
