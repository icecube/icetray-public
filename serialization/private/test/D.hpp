#ifndef I3_SERIALIZATION_TEST_D_HPP
#define I3_SERIALIZATION_TEST_D_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// D.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // NULL

#include <serialization/throw_exception.hpp>
#include <serialization/split_member.hpp>

#include "B.hpp"

///////////////////////////////////////////////////////
// Contained class with multiple identical pointers
class D{
private:
    friend class icecube::serialization::access;
    B* b1;
    B* b2;
    template<class Archive>
    void save(Archive &ar, const unsigned int file_version) const{
        ar << I3_SERIALIZATION_NVP(b1);
        ar << I3_SERIALIZATION_NVP(b2);
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int file_version){
        try{
            ar >> icecube::serialization::make_nvp("b", b1);
            ar >> icecube::serialization::make_nvp("b", b2);
        }
        catch(...){
            // eliminate invalid pointers
            b1 = NULL;
            b2 = NULL;
            FAIL( "multiple identical pointers failed to load" );
        }
        // check that loading was correct
        ENSURE(b1 == b2);
    }

    I3_SERIALIZATION_SPLIT_MEMBER()
public:
    D(){
        b1 = new B();
        b2 = b1;
    }
    ~D(){
        delete b1;
    }
    bool operator==(const D &rhs) const{
        if(! (*b1 == *(rhs.b1)) )
            return false;
        if(! (*b2 == *(rhs.b2)) )
            return false;
        return true;
    }
};

I3_CLASS_VERSION(D, 3)

#endif // I3_SERIALIZATION_TEST_D_HPP
