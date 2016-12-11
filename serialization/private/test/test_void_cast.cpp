/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_void_cast.cpp: test implementation of run-time casting of void pointers

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// <gennadiy.rozental@tfn.com>

#include <cstddef> // NULL
#include <I3Test.h>
#include <serialization/extended_type_info_typeid.hpp>
#include <serialization/void_cast.hpp>
#include <serialization/singleton.hpp>

namespace{
class Base1
{
    char a;
};

class Base2
{
    int b;
};

class Derived : public Base1, public Base2
{
    long c;
};

class MostDerived : public Derived
{
    char d[32];
};
}

template<class T>
const icecube::serialization::extended_type_info& eti(){
    return icecube::serialization::singleton<
        icecube::serialization::extended_type_info_typeid< T >
    >::get_const_instance();
}

TEST_GROUP(void_cast)

TEST(void_cast){
    MostDerived md;
    MostDerived* pmd =& md;
    Derived* pd = static_cast<Derived*>(pmd);

    Base2* pb2 = static_cast<Base2*>(pmd);
    Base1* pb1 = static_cast<Base1*>(pd);

    void* vpmd = static_cast<void*>(pmd);
    void* vpb1 = static_cast<void*>(pb1);
    void* vpb2 = static_cast<void*>(pb2);
    void* vpd = static_cast<void*>(pd);

    // simple casts only requiring table lookup
    ENSURE(vpd == icecube::serialization::void_downcast(
        eti<Derived>(),
        eti<Base1>(),
        vpb1
    ));
    ENSURE(vpb1 == icecube::serialization::void_upcast(
        eti<Derived>(),
        eti<Base1>(),
        vpd  
    ));
    ENSURE(vpd == icecube::serialization::void_downcast(
        eti<Derived>(),
        eti<Base2>(),
        vpb2
    ));
    ENSURE(vpb2 == icecube::serialization::void_upcast(
        eti<Derived>(),
        eti<Base2>(),
        vpd
    ));
    ENSURE(vpmd == icecube::serialization::void_downcast(
        eti<MostDerived>(),
        eti<Derived>(),
        vpd
    ));
    ENSURE(vpd == icecube::serialization::void_upcast(
        eti<MostDerived>(),
        eti<Derived>(),
        vpmd
    ));
    // note relationship between MostDerived and Base1 is automatically derived
    ENSURE(vpmd == icecube::serialization::void_downcast(
        eti<MostDerived>(),
        eti<Base1>(),
        vpb1
    ));
    ENSURE(vpb1 == icecube::serialization::void_upcast(
        eti<MostDerived>(),
        eti<Base1>(),
        vpmd
    ));

    // note relationship between MostDerived and Base2 is automatically derived
    ENSURE(vpmd == icecube::serialization::void_downcast(
        eti<MostDerived>(),
        eti<Base2>(),
        vpb2
    ));
    ENSURE(vpb2 == icecube::serialization::void_upcast(
        eti<MostDerived>(),
        eti<Base2>(),
        vpmd
    ));

    // note: currently derivations are not optimised.  See void_cast.cpp
    // for and explanation.  These should still work though.

    // need to double check to validate speed up optimization of derivations
    ENSURE(vpmd == icecube::serialization::void_downcast(
        eti<MostDerived>(),
        eti<Base1>(),
        vpb1
    ));
    ENSURE(vpb1 == icecube::serialization::void_upcast(
        eti<MostDerived>(),
        eti<Base1>(),
        vpmd
    ));
    ENSURE(vpmd == icecube::serialization::void_downcast(
        eti<MostDerived>(),
        eti<Base2>(),
        vpb2
    ));
    ENSURE(vpb2 == icecube::serialization::void_upcast(
        eti<MostDerived>(),
        eti<Base2>(),
        vpmd
    ));

    // check things that should fail
    ENSURE(NULL == icecube::serialization::void_downcast(
        eti<Base2>(),
        eti<Base1>(),
        vpb1
    ));

    // note that a fundamental feature is that derived/base pairs are created
    // at compiler time so that all are registered before the main program starts
    // so leave the registration here at the end to verify this. Note bogus arguments
    // to workaround msvc 6 bug
    icecube::serialization::void_cast_register<Derived, Base1>(
        static_cast<Derived *>(NULL),
        static_cast<Base1 *>(NULL)
    );
    icecube::serialization::void_cast_register<Derived, Base2>(
        static_cast<Derived *>(NULL),
        static_cast<Base2 *>(NULL)
    );
    icecube::serialization::void_cast_register<MostDerived, Derived>(
        static_cast<MostDerived *>(NULL),
        static_cast<Derived *>(NULL)
    );
}

// EOF
