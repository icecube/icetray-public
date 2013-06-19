#ifndef BOOST_SHARED_PTR_HPP_INCLUDED
#define BOOST_SHARED_PTR_HPP_INCLUDED
#ifndef __CINT__

//
//  shared_ptr.hpp
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001-2008 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/shared_ptr.htm for documentation.
//

#ifdef __clang__
// prevent warnings from clang in boost <= 1.53 (should be fixed in 1.54)
// when using shared_ptr with libc++ in c++03 mode
// (see https://svn.boost.org/trac/boost/ticket/8055 )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-extensions"
#endif

#include <boost/smart_ptr/shared_ptr.hpp>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#else  // #ifndef __CINT__
namespace boost {
  template <typename T> struct shared_ptr { };
}
#endif

#endif  // #ifndef BOOST_SHARED_PTR_HPP_INCLUDED

