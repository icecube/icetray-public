#ifndef I3_ARCHIVE_DETAIL_DECL_HPP
#define I3_ARCHIVE_DETAIL_DECL_HPP 

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif 

/////////1/////////2///////// 3/////////4/////////5/////////6/////////7/////////8
//  decl.hpp
//
//  (c) Copyright Robert Ramey 2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/serialization

//----------------------------------------------------------------------------// 

// This header implements separate compilation features as described in
// http://www.boost.org/more/separate_compilation.html

#include <boost/config.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#if defined(BOOST_HAS_DECLSPEC)
    #if (defined(BOOST_ALL_DYN_LINK) || defined(I3_SERIALIZATION_DYN_LINK))
        #if defined(I3_ARCHIVE_SOURCE)
            #if defined(__BORLANDC__)
            #define I3_ARCHIVE_DECL(T) T __export
            #define I3_ARCHIVE_OR_WARCHIVE_DECL(T)  T __export
            #else
            #define I3_ARCHIVE_DECL(T) __declspec(dllexport) T
            #define I3_ARCHIVE_OR_WARCHIVE_DECL(T)  __declspec(dllexport) T
            #endif
        #else
            #if defined(__BORLANDC__)
            #define I3_ARCHIVE_DECL(T) T __import
            #else
            #define I3_ARCHIVE_DECL(T) __declspec(dllimport) T
            #endif
        #endif
        #if defined(I3_WARCHIVE_SOURCE)
            #if defined(__BORLANDC__)
            #define I3_WARCHIVE_DECL(T) T __export
            #define I3_ARCHIVE_OR_WARCHIVE_DECL(T) T __export
            #else
            #define I3_WARCHIVE_DECL(T) __declspec(dllexport) T
            #define I3_ARCHIVE_OR_WARCHIVE_DECL(T) __declspec(dllexport) T
            #endif
        #else
            #if defined(__BORLANDC__)
            #define I3_WARCHIVE_DECL(T) T __import
            #else
            #define I3_WARCHIVE_DECL(T) __declspec(dllimport) T
            #endif
        #endif
        #if !defined(I3_WARCHIVE_SOURCE) && !defined(I3_ARCHIVE_SOURCE)
            #if defined(__BORLANDC__)
            #define I3_ARCHIVE_OR_WARCHIVE_DECL(T) T __import
            #else
            #define I3_ARCHIVE_OR_WARCHIVE_DECL(T) __declspec(dllimport) T
            #endif
        #endif
    #endif
#endif // BOOST_HAS_DECLSPEC

#if ! defined(I3_ARCHIVE_DECL)
    #define I3_ARCHIVE_DECL(T) T
#endif
#if ! defined(I3_WARCHIVE_DECL)
    #define I3_WARCHIVE_DECL(T) T
#endif
#if ! defined(I3_ARCHIVE_OR_WARCHIVE_DECL)
    #define I3_ARCHIVE_OR_WARCHIVE_DECL(T) T
#endif

#endif // I3_ARCHIVE_DETAIL_DECL_HPP
