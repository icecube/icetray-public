/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_text_iprimitive.ipp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstddef> // size_t
#include <cstddef> // NULL

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{
    using ::size_t;
} // namespace std
#endif

#include <serialization/throw_exception.hpp>
#include <serialization/pfto.hpp>

#include <archive/basic_text_iprimitive.hpp>
#include <archive/codecvt_null.hpp>
#include <archive/add_facet.hpp>

#include <archive/iterators/remove_whitespace.hpp>
#include <archive/iterators/istream_iterator.hpp>
#include <archive/iterators/binary_from_base64.hpp>
#include <archive/iterators/transform_width.hpp>

namespace icecube {
namespace archive {

namespace detail {
    template<class CharType>
    static inline bool is_whitespace(CharType c);

    template<>
    inline bool is_whitespace(char t){
        return 0 != std::isspace(t);
    }

    #ifndef BOOST_NO_CWCHAR
    template<>
    inline bool is_whitespace(wchar_t t){
        return 0 != std::iswspace(t);
    }
    #endif
} // detail

// translate base64 text into binary and copy into buffer
// until buffer is full.
template<class IStream>
I3_ARCHIVE_OR_WARCHIVE_DECL(void)
basic_text_iprimitive<IStream>::load_binary(
    void *address,
    std::size_t count
){
    typedef typename IStream::char_type CharType;

    if(0 == count)
        return;

    BOOST_ASSERT(
        static_cast<std::size_t>((std::numeric_limits<std::streamsize>::max)())
        > (count + sizeof(CharType) - 1)/sizeof(CharType)
    );

    if(is.fail())
        icecube::serialization::throw_exception(
            archive_exception(archive_exception::input_stream_error)
        );
    // convert from base64 to binary
    typedef typename
        iterators::transform_width<
            iterators::binary_from_base64<
                iterators::remove_whitespace<
                    iterators::istream_iterator<CharType>
                >
                ,CharType
            >
            ,8
            ,6
            ,CharType
        >
        binary;

    binary i = binary(
        I3_MAKE_PFTO_WRAPPER(
            iterators::istream_iterator<CharType>(is)
        )
    );

    char * caddr = static_cast<char *>(address);

    // take care that we don't increment anymore than necessary
    while(count-- > 0){
        *caddr++ = static_cast<char>(*i++);
    }

    // skip over any excess input
    for(;;){
        typename IStream::int_type r;
        r = is.get();
        if(is.eof())
            break;
        if(detail::is_whitespace(static_cast<CharType>(r)))
            break;
    }
}

template<class IStream>
I3_ARCHIVE_OR_WARCHIVE_DECL(BOOST_PP_EMPTY())
basic_text_iprimitive<IStream>::basic_text_iprimitive(
    IStream  &is_,
    bool no_codecvt
) :
#ifndef BOOST_NO_STD_LOCALE
    is(is_),
    flags_saver(is_),
    precision_saver(is_),
    archive_locale(NULL),
    locale_saver(* is_.rdbuf())
{
    if(! no_codecvt){
        archive_locale.reset(
            add_facet(
                std::locale::classic(),
                new codecvt_null<typename IStream::char_type>
            )
        );
        is.imbue(* archive_locale);
    }
    is >> std::noboolalpha;
}
#else
    is(is_),
    flags_saver(is_),
    precision_saver(is_)
{}
#endif

template<class IStream>
I3_ARCHIVE_OR_WARCHIVE_DECL(BOOST_PP_EMPTY())
basic_text_iprimitive<IStream>::~basic_text_iprimitive(){
    is.sync();
}

} // namespace archive
} // namespace icecube
