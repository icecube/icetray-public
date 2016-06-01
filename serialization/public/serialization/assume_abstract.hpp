#ifndef I3_SERIALIZATION_ASSUME_ABSTRACT_HPP
#define I3_SERIALIZATION_ASSUME_ABSTRACT_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// assume_abstract_class.hpp:

// (C) Copyright 2008 Robert Ramey
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// this is useful for compilers which don't support the boost::is_abstract

#include <boost/type_traits/is_abstract.hpp>

#ifndef BOOST_NO_IS_ABSTRACT

// if there is an intrinsic is_abstract defined, we don't have to do anything
#define I3_SERIALIZATION_ASSUME_ABSTRACT(T)

// but forward to the "official" is_abstract
namespace icecube {
namespace serialization {
    template<class T>
    struct is_abstract : boost::is_abstract< T > {} ;
} // namespace serialization
} // namespace icecube

#else
// we have to "make" one

namespace icecube {
namespace serialization {
    template<class T>
    struct is_abstract : boost::false_type {};
} // namespace serialization
} // namespace icecube

// define a macro to make explicit designation of this more transparent
#define I3_SERIALIZATION_ASSUME_ABSTRACT(T)        \
namespace icecube {                                     \
namespace serialization {                             \
template<>                                            \
struct is_abstract< T > : boost::true_type {};        \
template<>                                            \
struct is_abstract< const T > : boost::true_type {};  \
}}                                                    \
/**/

#endif // BOOST_NO_IS_ABSTRACT

#endif //I3_SERIALIZATION_ASSUME_ABSTRACT_HPP
