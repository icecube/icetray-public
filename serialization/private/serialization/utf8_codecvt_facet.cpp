// Copyright Vladimir Prus 2004.
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>
#ifdef BOOST_NO_STD_WSTREAMBUF
#error "wide char i/o not supported on this platform"
#else
//    #ifdef BOOST_NO_CXX11_HDR_CODECVT
        #define BOOST_UTF8_BEGIN_NAMESPACE \
             namespace icecube { namespace archive { namespace detail {
        #define BOOST_UTF8_DECL
        #define BOOST_UTF8_END_NAMESPACE }}}
        #include <boost/detail/utf8_codecvt_facet.ipp>
        #undef BOOST_UTF8_END_NAMESPACE
        #undef BOOST_UTF8_DECL
        #undef BOOST_UTF8_BEGIN_NAMESPACE
//    #endif // BOOST_NO_CXX11_HDR_CODECVT
#endif // BOOST_NO_STD_WSTREAMBUF
