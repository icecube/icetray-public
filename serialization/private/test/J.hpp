#ifndef I3_SERIALIZATION_TEST_J_HPP
#define I3_SERIALIZATION_TEST_J_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// J.hpp    simple class test

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <serialization/base_object.hpp>
#include <serialization/version.hpp>

#include "A.hpp"

///////////////////////////////////////////////////////
// class with a member which refers to itself
class J : public A
{
private:
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(
        Archive &ar,
        const unsigned int /* file_version */
    ){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(A);
        ar & I3_SERIALIZATION_NVP(j);
    }
public:
    bool operator==(const J &rhs) const{
        return static_cast<const A &>(*this) == static_cast<const A &>(rhs);
    }
    J* j;
    J(J* _j) : j(_j) {}
    J() : j(NULL){}
};

I3_CLASS_VERSION(J, 6)


#endif // I3_SERIALIZATION_TEST_J_HPP
