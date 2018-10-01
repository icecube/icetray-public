/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_singleton.cpp: test implementation of run-time casting of void pointers

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// <gennadiy.rozental@tfn.com>

#include <I3Test.h>
#include <serialization/singleton.hpp>

class x {};

void test1(const x& x1, const x& x2){
    ENSURE(&x1 == &x2);
}

TEST_GROUP(test_singleton)

TEST(test_singleton){
    const x& x1 = icecube::serialization::singleton<x>::get_const_instance();
    const x& x2 = icecube::serialization::singleton<x>::get_const_instance();

    ENSURE(&x1 == &x2);

    test1(
        icecube::serialization::singleton<x>::get_const_instance(),
        icecube::serialization::singleton<x>::get_const_instance()
    );
}
