#ifndef I3_ARCHIVE_ITERATORS_BASE64_EXCEPTION_HPP
#define I3_ARCHIVE_ITERATORS_BASE64_EXCEPTION_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// base64_exception.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp>
#ifndef BOOST_NO_EXCEPTIONS
#include <exception>

#include <boost/assert.hpp>

namespace icecube {
namespace archive {
namespace iterators {

//////////////////////////////////////////////////////////////////////
// exceptions thrown by base64s
//
class base64_exception : public std::exception
{
public:
    typedef enum {
        invalid_code,       // attempt to encode a value > 6 bits
        invalid_character,  // decode a value not in base64 char set
        other_exception
    } exception_code;
    exception_code code;

    base64_exception(exception_code c = other_exception) : code(c)
    {}

    virtual const char *what( ) const throw( )
    {
        const char *msg = "unknown exception code";
        switch(code){
        case invalid_code:
            msg = "attempt to encode a value > 6 bits";
            break;
        case invalid_character:
            msg = "attempt to decode a value not in base64 char set";
            break;
        default:
            BOOST_ASSERT(false);
            break;
        }
        return msg;
    }
};

} // namespace iterators
} // namespace archive
} // namespace icecube

#endif //BOOST_NO_EXCEPTIONS
#endif //I3_ARCHIVE_ITERATORS_BASE64_EXCEPTION_HPP
