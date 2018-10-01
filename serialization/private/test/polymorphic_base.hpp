#ifndef POLYMORPHIC_BASE_HPP
#define POLYMORPHIC_BASE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_base.hpp    simple class test

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <serialization/access.hpp>
#include <serialization/assume_abstract.hpp>
#include <serialization/export.hpp>
#include <serialization/type_info_implementation.hpp>
#include <serialization/extended_type_info_no_rtti.hpp>

#include "test_decl.hpp"

#if defined(POLYMORPHIC_DERIVED_IMPORT)
    #define DLL_DECL IMPORT_DECL
#elif defined(POLYMORPHIC_DERIVED_EXPORT)
    #define DLL_DECL EXPORT_DECL
#else
    #define DLL_DECL(x)
#endif

class DLL_DECL(BOOST_PP_EMPTY()) polymorphic_base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(
        Archive & /* ar */, 
        const unsigned int /* file_version */
    ){}
public:
    // note that since this class uses the "no_rtti"
    // extended_type_info implementation, it MUST
    // implement this function
    virtual const char * get_key() const = 0;
    virtual ~polymorphic_base(){};
};

I3_SERIALIZATION_ASSUME_ABSTRACT(polymorphic_base)

// the no_rtti system requires this !!!
I3_CLASS_EXPORT_KEY(polymorphic_base)

I3_CLASS_TYPE_INFO(
    polymorphic_base,
    icecube::serialization::extended_type_info_no_rtti<polymorphic_base>
)

#endif // POLYMORPHIC_BASE_HPP
