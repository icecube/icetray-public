#ifndef I3_SERIALIZATION_TEST_BASE_HPP
#define I3_SERIALIZATION_TEST_BASE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// base.hpp    simple class test

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <serialization/access.hpp>
#include <serialization/assume_abstract.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include "test_decl.hpp"

#if defined(BASE_IMPORT)
    #define DLL_DECL IMPORT_DECL
#elif defined(BASE_EXPORT)
    #define DLL_DECL EXPORT_DECL
#else
    #define DLL_DECL(x)
#endif

class DLL_DECL(BOOST_PP_EMPTY()) base
{
    friend class icecube::serialization::access;
    template<class Archive>
    void serialize(Archive & /* ar */, const unsigned int /* file_version */);
public:
    virtual ~base(){};
};

I3_SERIALIZATION_ASSUME_ABSTRACT(base)

#undef  DLL_DECL

#endif // I3_SERIALIZATION_TEST_BASE_HPP
