#ifndef I3_SERIALIZATION_TEST_DERIVED2_HPP
#define I3_SERIALIZATION_TEST_DERIVED2_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// derived2.hpp    simple class test

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <serialization/export.hpp>
#include <serialization/access.hpp>

#define BASE_IMPORT
#include "base.hpp"

#include "test_decl.hpp"

#if defined(DERIVED2_IMPORT)
    #define DLL_DECL IMPORT_DECL
#elif defined(DERIVED2_EXPORT)
    #define DLL_DECL EXPORT_DECL
#else
    #define DLL_DECL(x)
#endif

class DLL_DECL(BOOST_PP_EMPTY()) derived2 :
    public base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int /* file_version */);
public:
    ~derived2(){}
};

#undef DLL_DECL

// MWerks users can do this to make their code work
BOOST_SERIALIZATION_MWERKS_BASE_AND_DERIVED(base, derived2)

#endif // I3_SERIALIZATION_TEST_DERIVED2_HPP
