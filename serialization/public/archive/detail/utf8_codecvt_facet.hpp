// Copyright (c) 2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// SPDX-License-Identifier: BSL-1.0
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu).
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef I3_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP
#define I3_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP

#ifdef BOOST_NO_CXX11_HDR_CODECVT
    #define I3_UTF8_BEGIN_NAMESPACE \
         namespace icecube { namespace archive { namespace detail {
    #define I3_UTF8_DECL
    #define I3_UTF8_END_NAMESPACE }}}

    #include <boost/detail/utf8_codecvt_facet.hpp>

    #undef BOOST_UTF8_END_NAMESPACE
    #undef BOOST_UTF8_DECL
    #undef BOOST_UTF8_BEGIN_NAMESPACE
#endif // BOOST_NO_CXX11_HDR_CODECVT
#endif // I3_ARCHIVE_DETAIL_UTF8_CODECVT_FACET_HPP

