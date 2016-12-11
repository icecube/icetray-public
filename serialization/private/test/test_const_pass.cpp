/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_const.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// compile only

#include <archive/text_oarchive.hpp>
#include <archive/text_iarchive.hpp>
#include <serialization/nvp.hpp>

struct A {
    template<class Archive>
    void serialize(Archive & ar, unsigned int version) {
    }
};

// should compile w/o problem
void f1(icecube::archive::text_oarchive & oa, const A & a){
    oa & a;
    oa & I3_SERIALIZATION_NVP(a);
    oa << a;
    oa << I3_SERIALIZATION_NVP(a);
}
void f2(icecube::archive::text_oarchive & oa, const A * const & a){
    oa & a;
    oa & I3_SERIALIZATION_NVP(a);
    oa << a;
    oa << I3_SERIALIZATION_NVP(a);
}
void f3(icecube::archive::text_iarchive & ia, A & a){
    ia & a;
    ia & I3_SERIALIZATION_NVP(a);
    ia >> a;
    ia >> I3_SERIALIZATION_NVP(a);
}
void f4(icecube::archive::text_iarchive & ia, A * & a){
    ia & a;
    ia & I3_SERIALIZATION_NVP(a);
    ia >> a;
    ia >> I3_SERIALIZATION_NVP(a);
}
#if 0
void f5(icecube::archive::text_oarchive & oa, const A * & a){
    oa & a;
    oa & I3_SERIALIZATION_NVP(a);
    oa << a;
    oa << I3_SERIALIZATION_NVP(a);
}
#endif

