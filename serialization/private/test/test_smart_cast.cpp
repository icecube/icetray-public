/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_smart_cast.cpp: 

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// <gennadiy.rozental@tfn.com>

#include <exception>
#include <serialization/smart_cast.hpp>

#include <I3Test.h>
#include <boost/noncopyable.hpp>

TEST_GROUP(smart_cast)

using namespace icecube::serialization;

namespace{
class Base1 : public boost::noncopyable
{
    char a;
};

class Base2
{
    int b;
};

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

class Derived : public Base1, public Base2
{
    long c;
};

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
}

// if compiler doesn't support TPS, the smart_cast syntax doesn't
// work for references.  One has to use the smart_cast_reference
// syntax (tested below ) instead.

TEST(static_reference_cast_2){
    Derived d;
    Base1& b1 = static_cast<Base1&>(d);
    Base2& b2 = static_cast<Base2&>(d);

    Base1& scb1 = smart_cast<Base1&, Derived&>(d);
    Base2& scb2 = smart_cast<Base2&, Derived&>(d);
    ENSURE_EQUAL(&b1, &scb1);
    ENSURE_EQUAL(&b2, &scb2);

    // downcast
//    BOOST_CHECK_EQUAL(& d, & (smart_cast<Derived &, Base1 &>(b1)));
//    BOOST_CHECK_EQUAL(& d, & (smart_cast<Derived &, Base2 &>(b2)));

    // crosscast pointers fails at compiler time
    // ENSURE_EQUAL(pB2,smart_cast<B2 *>(pB1));
    // though explicit cross cast will always work
    ENSURE_EQUAL(&b2,(
        &smart_cast<Base2&, Derived&>(
            smart_cast<Derived&, Base1&>(b1)
        ))
    );
}

TEST(static_reference_cast_1){
    Derived d;
    Base1& b1 = static_cast<Base1&>(d);
    Base2& b2 = static_cast<Base2&>(d);

    Base1& scb1 = smart_cast_reference<Base1&>(d);
    Base2& scb2 = smart_cast_reference<Base2&>(d);
    ENSURE_EQUAL(&b1, &scb1);
    ENSURE_EQUAL(&b2, &scb2);

    // downcast
    ENSURE_EQUAL(&d, &(smart_cast_reference<Derived&>(b1)));
    ENSURE_EQUAL(&d, &(smart_cast_reference<Derived&>(b2)));

    // crosscast pointers fails at compiler time
    // ENSURE_EQUAL(pB2,smart_cast<B2 *>(pB1));
    // though explicit cross cast will always work
    ENSURE_EQUAL(&b2,(
        &smart_cast_reference<Base2&>(
            smart_cast_reference<Derived&>(b1)
        ))
    );
}

TEST(static_pointer_cast){
    // pointers
    Derived d;
    Derived* pD = &d;
    Base1* pB1 = pD;
    Base2* pB2 = pD;

    // upcast
    ENSURE_EQUAL(pB1, smart_cast<Base1 *>(pD));
    ENSURE_EQUAL(pB2, smart_cast<Base2 *>(pD));

    // downcast
    ENSURE_EQUAL(pD, smart_cast<Derived *>(pB1));
    ENSURE_EQUAL(pD, smart_cast<Derived *>(pB2));

    // crosscast pointers fails at compiler time
    // ENSURE_EQUAL(pB2, smart_cast<Base2 *>(pB1));

    // though explicit cross cast will always work
    ENSURE_EQUAL(pB2,
        smart_cast<Base2*>(
            smart_cast<Derived*>(pB1)
        )
    );
}

namespace{
class VBase1 : public boost::noncopyable
{
    char a;
public:
    virtual ~VBase1(){};
};

class VBase2
{
    int b;
public:
    virtual ~VBase2(){};
};

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

class VDerived : public VBase1, public VBase2
{
    long c;
public:
    virtual ~VDerived(){};
};

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
}

// see above

TEST(dynamic_reference_cast_2){
    VDerived d;
    VBase1& b1 = dynamic_cast<VBase1&>(d);
    VBase2& b2 = static_cast<VBase2&>(d);

    VBase1& vb1 = smart_cast<VBase1&, VDerived&>(d);
    ENSURE_EQUAL(&b1, &vb1);
    ENSURE_EQUAL(&b2, (&smart_cast<VBase2&, VDerived&>(d)));

    // downcast
    ENSURE_EQUAL(&d, (&smart_cast<VDerived&, VBase1&>(b1)));
    ENSURE_EQUAL(&d, (&smart_cast<VDerived&, VBase2&>(b2)));

    // crosscast
    ENSURE_EQUAL(&b2, (&smart_cast<VBase2&, VBase1&>(b1)));

    // explicit cross cast should always work
    ENSURE_EQUAL(&b2, (
        &smart_cast<VBase2&, VDerived&>(
            smart_cast<VDerived&, VBase1&>(b1)
        ))
    );
}

TEST(dynamic_reference_cast_1){
    VDerived d;
    VBase1& b1 = dynamic_cast<VBase1&>(d);
    VBase2& b2 = static_cast<VBase2&>(d);

    VBase1& vb1 = smart_cast_reference<VBase1&>(d);
    ENSURE_EQUAL(&b1, &vb1);
    ENSURE_EQUAL(&b2, (&smart_cast_reference<VBase2&>(d)));

    // downcast
    ENSURE_EQUAL(&d, (&smart_cast_reference<VDerived&>(b1)));
    ENSURE_EQUAL(&d, (&smart_cast_reference<VDerived&>(b2)));

    // crosscast
    ENSURE_EQUAL(&b2, (&smart_cast_reference<VBase2&>(b1)));

    // explicit cross cast should always work
    ENSURE_EQUAL(&b2, (
        &smart_cast_reference<VBase2&>(
            smart_cast_reference<VDerived&>(b1)
        ))
    );
}

TEST(dynamic_pointer_cast){
    // pointers
    VDerived d;
    VDerived* pD = &d;
    VBase1* pB1 = pD;
    VBase2* pB2 = pD;

    // upcast
    ENSURE_EQUAL(pB1, smart_cast<VBase1*>(pD));
    ENSURE_EQUAL(pB2, smart_cast<VBase2*>(pD));

    // downcast
    ENSURE_EQUAL(pD, smart_cast<VDerived*>(pB1));
    ENSURE_EQUAL(pD, smart_cast<VDerived*>(pB2));

    // crosscast pointers fails at compiler time
    ENSURE_EQUAL(pB2, smart_cast<VBase2*>(pB1));
    // though explicit cross cast will always work
    ENSURE_EQUAL(pB2,
        smart_cast<VBase2*>(
            smart_cast<VDerived*>(pB1)
        )
    );
}
