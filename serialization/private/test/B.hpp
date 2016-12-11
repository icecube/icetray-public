#ifndef I3_SERIALIZATION_TEST_B_HPP
#define I3_SERIALIZATION_TEST_B_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// B.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstdlib> // for rand()
#include <cmath>
#include <boost/math/special_functions/next.hpp>

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::rand; 
}
#endif

#include <serialization/version.hpp>
#include <serialization/split_member.hpp>
#include <serialization/base_object.hpp>

#include "A.hpp"

///////////////////////////////////////////////////////
// Derived class test
class B : public A
{
private:
    friend class icecube::serialization::access;
    template<class Archive>
    void save(Archive &ar, const unsigned int /* file_version */) const
    {
        // write any base class info to the archive
        ar << I3_SERIALIZATION_BASE_OBJECT_NVP(A);

        // write out members
        ar << I3_SERIALIZATION_NVP(s);
        ar << I3_SERIALIZATION_NVP(t);
        ar << I3_SERIALIZATION_NVP(u);
        ar << I3_SERIALIZATION_NVP(v);
        ar << I3_SERIALIZATION_NVP(w);
        ar << I3_SERIALIZATION_NVP(x);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int file_version)
    {
        // read any base class info to the archive
        ar >> I3_SERIALIZATION_BASE_OBJECT_NVP(A);
        switch(file_version){
        case 1:
        case 2:
            ar >> I3_SERIALIZATION_NVP(s);
            ar >> I3_SERIALIZATION_NVP(t);
            ar >> I3_SERIALIZATION_NVP(u);
            ar >> I3_SERIALIZATION_NVP(v);
            ar >> I3_SERIALIZATION_NVP(w);
            ar >> I3_SERIALIZATION_NVP(x);
            break;
        default:
            break;
        }
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
    signed char s;
    unsigned char t;
    signed int u;
    unsigned int v;
    float w;
    double x;
public:
    B() :
    s(static_cast<signed char>(std::rand())),
    t(static_cast<unsigned char>(std::rand())),
    u(std::rand()),
    v(std::rand()),
    w((float)std::rand() / std::rand()),
    x((double)std::rand() / std::rand())
    {}
    virtual ~B(){};
    bool operator==(const B &rhs) const{
        return
        A::operator==(rhs)
        && s == rhs.s
        && t == rhs.t
        && u == rhs.u
        && v == rhs.v
        && std::abs( boost::math::float_distance(w, rhs.w)) < 2
        && std::abs( boost::math::float_distance(x, rhs.x)) < 2
        ;
    }
};

I3_CLASS_VERSION(B, 2)

#endif // I3_SERIALIZATION_TEST_B_HPP
