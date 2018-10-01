#ifndef I3_ARCHIVE_DETAIL_BASIC_CONFIG_HPP
#define I3_ARCHIVE_DETAIL_BASIC_CONFIG_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

//  basic_config.hpp  ---------------------------------------------//

//  (c) Copyright Robert Ramey 2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/serialization

//----------------------------------------------------------------------------// 

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <boost/config.hpp>

//#ifdef BOOST_HAS_DECLSPEC // defined in config system
//// we need to import/export our code only if the user has specifically
//// asked for it by defining either BOOST_ALL_DYN_LINK if they want all boost
//// libraries to be dynamically linked, or I3_ARCHIVE_DYN_LINK
//// if they want just this one to be dynamically linked:
//#if defined(BOOST_ALL_DYN_LINK) || defined(I3_ARCHIVE_DYN_LINK)
//// export if this is our own source, otherwise import:
//#ifdef I3_ARCHIVE_SOURCE
//# define I3_ARCHIVE_DECL __declspec(dllexport)
//#else
//# define I3_ARCHIVE_DECL __declspec(dllimport)
//#endif  // I3_ARCHIVE_SOURCE
//#endif  // DYN_LINK
//#endif  // BOOST_HAS_DECLSPEC

// we need to import/export our code only if the user has specifically
// asked for it by defining either BOOST_ALL_DYN_LINK if they want all boost
// libraries to be dynamically linked, or I3_ARCHIVE_DYN_LINK
// if they want just this one to be dynamically linked:
#if defined(BOOST_ALL_DYN_LINK) || defined(I3_ARCHIVE_DYN_LINK)
#warning BOOST_ALL_DYN_LINK or I3_ARCHIVE_DYN_LINK
// export if this is our own source, otherwise import:
#ifdef I3_ARCHIVE_SOURCE
# define I3_ARCHIVE_DECL BOOST_SYMBOL_EXPORT
#else
# define I3_ARCHIVE_DECL BOOST_SYMBOL_IMPORT
#endif  // I3_ARCHIVE_SOURCE
#endif  // DYN_LINK

//
// if I3_ARCHIVE_DECL isn't defined yet define it now:
#ifndef I3_ARCHIVE_DECL
#warning defining away I3_ARCHIVE_DECL
#define I3_ARCHIVE_DECL
#endif

#endif // I3_ARCHIVE_DETAIL_BASIC_CONFIG_HPP
