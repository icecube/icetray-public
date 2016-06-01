// (C) Copyright 2007 Matthias Troyer

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Matthias Troyer

/** @file is_bitwise_serializable.hpp
 *
 *  This header provides a traits class for determining whether a class
 * can be serialized (in a non-portable way) just by copying the bits.
 */


#ifndef I3_SERIALIZATION_IS_BITWISE_SERIALIZABLE_HPP
#define I3_SERIALIZATION_IS_BITWISE_SERIALIZABLE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

#include <boost/mpl/bool.hpp>
#include <boost/type_traits/is_arithmetic.hpp>

namespace icecube {
namespace serialization {
    template<class T>
    struct is_bitwise_serializable
     : public boost::is_arithmetic< T >
    {};
} // namespace serialization
} // namespace icecube


// define a macro to make explicit designation of this more transparent
#define I3_IS_BITWISE_SERIALIZABLE(T)              \
namespace icecube {                                     \
namespace serialization {                             \
template<>                                            \
struct is_bitwise_serializable< T > : boost::mpl::true_ {};  \
}}                                                    \
/**/

#endif //I3_SERIALIZATION_IS_BITWISE_SERIALIZABLE_HPP
