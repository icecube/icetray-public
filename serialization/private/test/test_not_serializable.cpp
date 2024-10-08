/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_non_serializable.cpp: test implementation level trait

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// test implementation level "not_serializable"
// should fail compilation

#include <fstream>

#include <I3Test.h>
#include <serialization/level.hpp>
#include <serialization/nvp.hpp>

namespace{
class A{};
}

I3_CLASS_IMPLEMENTATION(A, icecube::serialization::not_serializable)

void out(A & a){
    test_ostream os("testfile", TEST_STREAM_FLAGS);
    test_oarchive oa(os, TEST_ARCHIVE_FLAGS);
    oa << I3_SERIALIZATION_NVP(a);
}

void in(A & a){
    test_istream is("testfile", TEST_STREAM_FLAGS);
    test_iarchive ia(is, TEST_ARCHIVE_FLAGS);
    ia >> I3_SERIALIZATION_NVP(a);
}

void test(){
    A a;
    out(a);
    in(a);
}

// EOF
