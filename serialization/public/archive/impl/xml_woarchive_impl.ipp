/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_woarchive_impl.ipp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>
#ifndef BOOST_NO_STD_WSTREAMBUF

#include <ostream>
#include <string>
#include <algorithm> // std::copy
#include <locale>

#include <cstring> // strlen
#include <cstdlib> // mbtowc
#include <cwchar>  // wcslen

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::strlen;
    #if ! defined(BOOST_NO_INTRINSIC_WCHAR_T)
        using ::mbtowc;
        using ::wcslen;
    #endif
} // namespace std
#endif

#include <serialization/throw_exception.hpp>
#include <serialization/pfto.hpp>

#include <archive/iterators/xml_escape.hpp>
#include <archive/iterators/wchar_from_mb.hpp>
#include <archive/iterators/ostream_iterator.hpp>
#include <archive/iterators/dataflow_exception.hpp>

#include <archive/add_facet.hpp>
#ifndef BOOST_NO_CXX11_HDR_CODECVT
    #include <codecvt>
    namespace icecube { namespace archive { namespace detail {
        typedef std::codecvt_utf8<wchar_t> utf8_codecvt_facet;
    } } }
#else
    #include <archive/detail/utf8_codecvt_facet.hpp>
#endif

namespace icecube {
namespace archive {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// implemenations of functions specific to wide char archives

// copy chars to output escaping to xml and widening characters as we go
template<class InputIterator>
void save_iterator(std::wostream &os, InputIterator begin, InputIterator end){
    typedef iterators::wchar_from_mb<
        iterators::xml_escape<InputIterator>
    > xmbtows;
    std::copy(
        xmbtows(BOOST_MAKE_PFTO_WRAPPER(begin)),
        xmbtows(BOOST_MAKE_PFTO_WRAPPER(end)),
        icecube::archive::iterators::ostream_iterator<wchar_t>(os)
    );
}

template<class Archive>
BOOST_WARCHIVE_DECL(void)
xml_woarchive_impl<Archive>::save(const std::string & s){
    // note: we don't use s.begin() and s.end() because dinkumware
    // doesn't have string::value_type defined. So use a wrapper
    // around these values to implement the definitions.
    const char * begin = s.data();
    const char * end = begin + s.size();
    save_iterator(os, begin, end);
}

#ifndef BOOST_NO_STD_WSTRING
template<class Archive>
BOOST_WARCHIVE_DECL(void)
xml_woarchive_impl<Archive>::save(const std::wstring & ws){
#if 0
    typedef iterators::xml_escape<std::wstring::const_iterator> xmbtows;
    std::copy(
        xmbtows(BOOST_MAKE_PFTO_WRAPPER(ws.begin())),
        xmbtows(BOOST_MAKE_PFTO_WRAPPER(ws.end())),
        icecube::archive::iterators::ostream_iterator<wchar_t>(os)
    );
#endif
    typedef iterators::xml_escape<const wchar_t *> xmbtows;
    std::copy(
        xmbtows(BOOST_MAKE_PFTO_WRAPPER(ws.data())),
        xmbtows(BOOST_MAKE_PFTO_WRAPPER(ws.data() + ws.size())),
        icecube::archive::iterators::ostream_iterator<wchar_t>(os)
    );
}
#endif //BOOST_NO_STD_WSTRING

template<class Archive>
BOOST_WARCHIVE_DECL(void)
xml_woarchive_impl<Archive>::save(const char * s){
   save_iterator(os, s, s + std::strlen(s));
}

#ifndef BOOST_NO_INTRINSIC_WCHAR_T
template<class Archive>
BOOST_WARCHIVE_DECL(void)
xml_woarchive_impl<Archive>::save(const wchar_t * ws){
    os << ws;
    typedef iterators::xml_escape<const wchar_t *> xmbtows;
    std::copy(
        xmbtows(BOOST_MAKE_PFTO_WRAPPER(ws)),
        xmbtows(BOOST_MAKE_PFTO_WRAPPER(ws + std::wcslen(ws))),
        icecube::archive::iterators::ostream_iterator<wchar_t>(os)
    );
}
#endif

template<class Archive>
BOOST_WARCHIVE_DECL(BOOST_PP_EMPTY())
xml_woarchive_impl<Archive>::xml_woarchive_impl(
    std::wostream & os_,
    unsigned int flags
) :
    basic_text_oprimitive<std::wostream>(
        os_,
        true // don't change the codecvt - use the one below
    ),
    basic_xml_oarchive<Archive>(flags)
{
    // Standard behavior is that imbue can be called
    // a) before output is invoked or
    // b) after flush has been called.  This prevents one-to-many
    // transforms (such as one to many transforms from getting
    // mixed up.
    if(0 == (flags & no_codecvt)){
        icecube::archive::detail::utf8_codecvt_facet *pfacet;
        #if defined(__SGI_STL_PORT)
            // Unfortunately, STLPort doesn't respect b) above
            // so the restoration of the original archive locale done by
            // the locale_saver doesn't get processed,
            // before the current one is destroyed.
            // so the codecvt doesn't get replaced with the orginal
            // so closing the stream invokes codecvt::do_unshift
            // so it crashes because the corresponding locale that contained
            // the codecvt isn't around any more.
            // we can hack around this by using a static codecvt that never
            // gets destroyed.
            static icecube::archive::detail::utf8_codecvt_facet
                facet(static_cast<size_t>(1));
            pfacet = & facet;
        #else
            pfacet = new icecube::archive::detail::utf8_codecvt_facet;
        #endif
        archive_locale.reset(add_facet(os_.getloc(), pfacet));
        os.imbue(* archive_locale);
    }
    if(0 == (flags & no_header))
        this->init();
}

} // namespace archive
} // namespace icecube

#endif //BOOST_NO_STD_WSTREAMBUF
