/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_private_ctor.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>

#include <I3Test.h>

#include <serialization/vector.hpp>

#include <archive/text_iarchive.hpp>
#include <archive/text_oarchive.hpp>

class V {
    friend void do_test();
    friend class icecube::serialization::access;
    int m_i;    
    V() :
        m_i(0)
    {}
    ~V(){}
    template<class Archive>
    void serialize(Archive& ar, unsigned /*version*/)
    {
        ar & m_i;
    }
    bool operator==(const V & v) const {
        return m_i == v.m_i;
    }
};

void do_test(){
    std::stringstream ss;
    const V v;
    {
        icecube::archive::text_oarchive oa(ss);
        oa << v;
    }
    V v1;
    {
        icecube::archive::text_iarchive ia(ss);
        ia >> v1;
    }
    ENSURE(v == v1);
}

TEST_GROUP(private_ctor)

TEST(test_private_ctor){
    do_test();
}
