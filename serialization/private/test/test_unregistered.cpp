/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_unregistered.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// should pass compilation and execution

#include <fstream>

#include <cstddef> // NULL
#include <cstdio> // remove
#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::remove;
}
#endif

#include <I3Test.h>

#include <archive/archive_exception.hpp>
#include <serialization/base_object.hpp>
#include <serialization/type_info_implementation.hpp>

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

// save unregistered polymorphic classes
template <typename TS /*test settings*/>
void save_unregistered1(const char* testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_base* rb1 =  new polymorphic_derived1;

    // registration IS necessary when serializing a polymorphic class
    // through pointer to the base class
    bool except = false;
    try{
        oa << I3_SERIALIZATION_NVP(rb1);
    }
    catch(icecube::archive::archive_exception aex){
        except = true;
    }
    ENSURE(except, "lack of registration not detected !");

    delete rb1;
}

// note: the corresponding save function above will not result in
// valid archive - hence, the following code which attempts to load
// and archive will fail.  Leave this as a reminder not to do this
#if 0
// load unregistered polymorphic classes
void load_unregistered1(const char* testfile)
{
    std::ifstream is(testfile);
    boost::archive::iarchive ia(is);

    polymorphic_base* rb1(nullptr);

    // registration IS necessary when serializing a polymorphic class
    // through pointer to the base class
    bool except = false;
    try{
        ia >> I3_SERIALIZATION_NVP(rb1);
    }
    catch(icecube::archive::archive_exception aex){
        except = true;
        ENSURE(
            NULL == rb1,
            "failed load resulted in a non-null pointer"
        );
    }
    ENSURE(except, "lack of registration not detected !");

    delete rb1;
}
#endif

// save unregistered polymorphic classes
template <typename TS /*test settings*/>
void save_unregistered2(const char *testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_derived1 *rd1 = new polymorphic_derived1;

    // registration is NOT necessary when serializing a polymorphic class
    // through pointer to a derived class
    bool except = false;
    try{
        oa << I3_SERIALIZATION_NVP(rd1);
    }
    catch(icecube::archive::archive_exception aex){
        except = true;
    }
    ENSURE(!except, "registration not detected !");

    delete rd1;
}

// note: the corresponding save function above will not result in
// valid archive - hence, the following code which attempts to load
// and archive will fail.  Leave this as a reminder not to do this
// load unregistered polymorphic classes
template <typename TS /*test settings*/>
void load_unregistered2(const char *testfile)
{
    typename TS::test_istream is(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_iarchive ia(is, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_derived1* rd1 = nullptr;

    // registration is NOT necessary when serializing a polymorphic class
    // or through pointer to a derived class
    bool except = false;
    try{
        ia >> I3_SERIALIZATION_NVP(rd1);
    }
    catch(boost::archive::archive_exception aex){
        except = true;
        ENSURE(
            NULL == rd1,
            "failed load resulted in a non-null pointer"
        );
    }
    ENSURE(! except, "registration not detected !");

    delete rd1;
}

// save registered polymorphic class
template <typename TS /*test settings*/>
void save_registered(const char* testfile)
{
    typename TS::test_ostream os(testfile, TS::TEST_STREAM_FLAGS);
    typename TS::test_oarchive oa(os, TS::TEST_ARCHIVE_FLAGS);

    polymorphic_base* rb1 = new polymorphic_derived1;
    polymorphic_base* rb2 = new polymorphic_derived2;

    // registration (forward declaration) will permit correct serialization
    // through a pointer to a base class
    oa.register_type(static_cast<polymorphic_derived1*>(nullptr));
    oa.register_type(static_cast<polymorphic_derived2*>(nullptr));
    oa << I3_SERIALIZATION_NVP(rb1);
    oa << I3_SERIALIZATION_NVP(rb2);

    delete rb1;
    delete rb2;
}

// load registered polymorphic class
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

    ia >> I3_SERIALIZATION_NVP(rb1);

    ENSURE(NULL != rb1, "Load resulted in NULL pointer");
    ENSURE(
        icecube::serialization::type_info_implementation<
            polymorphic_derived1
        >::type::get_const_instance()
        ==
        *icecube::serialization::type_info_implementation<
            polymorphic_base
        >::type::get_const_instance().get_derived_extended_type_info(*rb1),
        "restored pointer b1 not of correct type"
    );

    ia >> I3_SERIALIZATION_NVP(rb2);
    ENSURE(NULL != rb2, "Load resulted in NULL pointer");
    ENSURE(
        icecube::serialization::type_info_implementation<
            polymorphic_derived2
        >::type::get_const_instance()
        ==
        *icecube::serialization::type_info_implementation<
            polymorphic_base
        >::type::get_const_instance().get_derived_extended_type_info(*rb2),
        "restored pointer b2 not of correct type"
    );

    delete rb1;
    delete rb2;
}

int
test_main( int /* argc */, char* /* argv */[] )
{
    const char * testfile = boost::archive::tmpnam(NULL);
    BOOST_REQUIRE(NULL != testfile);
    save_unregistered1(testfile);
//  load_unregistered1(testfile);
    save_unregistered2(testfile);
    load_unregistered2(testfile);
    save_registered(testfile);
    load_registered(testfile);
    std::remove(testfile);
    return EXIT_SUCCESS;
}

// EOF
