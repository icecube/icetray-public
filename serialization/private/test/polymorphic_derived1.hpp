#ifndef POLYMORPHIC_DERIVED1_HPP
#define POLYMORPHIC_DERIVED1_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_derived1.hpp    simple class test

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <serialization/access.hpp>
#include <serialization/nvp.hpp>
#include <serialization/base_object.hpp>
#include <serialization/type_info_implementation.hpp>
#include <serialization/extended_type_info_no_rtti.hpp>

#include "polymorphic_base.hpp"

class polymorphic_derived1 : public polymorphic_base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int  /* file_version */){
        ar & I3_SERIALIZATION_BASE_OBJECT_NVP(polymorphic_base);
    }
public:
    virtual const char * get_key() const ;
};

I3_CLASS_EXPORT_KEY(polymorphic_derived1)

I3_CLASS_TYPE_INFO(
    polymorphic_derived1,
    extended_type_info_no_rtti<polymorphic_derived1>
)

#endif // POLYMORPHIC_DERIVED1_HPP
