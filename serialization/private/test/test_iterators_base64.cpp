/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_iterators.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <list>

#if (defined _MSC_VER) && (_MSC_VER == 1200)
#  pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <cstdlib>
#include <cstddef> // size_t

#include <boost/config.hpp>
#ifdef BOOST_NO_STDC_NAMESPACE
namespace std{ 
    using ::rand; 
    using ::size_t;
}
#endif

#include <serialization/pfto.hpp>

#include <archive/iterators/binary_from_base64.hpp>
#include <archive/iterators/base64_from_binary.hpp>
#include <archive/iterators/insert_linebreaks.hpp>
#include <archive/iterators/remove_whitespace.hpp>
#include <archive/iterators/transform_width.hpp>

#include <I3Test.h>

#include <iostream>

template<typename CharType>
void test_base64(unsigned int size){
    CharType rawdata[150];
    CharType * rptr;
    for(rptr = rawdata + size; rptr-- > rawdata;)
        *rptr = static_cast<CharType>(std::rand()& 0xff);

    // convert to base64
    typedef std::list<CharType> text_base64_type;
    text_base64_type text_base64;

    typedef 
        icecube::archive::iterators::insert_linebreaks<
            icecube::archive::iterators::base64_from_binary<
                icecube::archive::iterators::transform_width<
                    CharType *
                    ,6
                    ,sizeof(CharType) * 8
                >
            > 
            ,76
        > 
        translate_out;

    std::copy(
        translate_out(I3_MAKE_PFTO_WRAPPER(static_cast<CharType *>(rawdata))),
        translate_out(I3_MAKE_PFTO_WRAPPER(rawdata + size)),
        std::back_inserter(text_base64)
    );

    // convert from base64 to binary and compare with the original 
    typedef 
        icecube::archive::iterators::transform_width<
            icecube::archive::iterators::binary_from_base64<
                icecube::archive::iterators::remove_whitespace<
                    typename text_base64_type::iterator
                >
            >,
            sizeof(CharType) * 8,
            6
        > translate_in;
    
    ENSURE(
        std::equal(
            rawdata,
            rawdata + size,
            translate_in(I3_MAKE_PFTO_WRAPPER(text_base64.begin()))
        )
    );

}

TEST_GROUP(test_iterators_base64)

TEST(test_iterators_base64){
    test_base64<char>(1);
    test_base64<char>(2);
    test_base64<char>(3);
    test_base64<char>(4);
    test_base64<char>(150);
    #ifndef BOOST_NO_CWCHAR
    test_base64<wchar_t>(1);
    test_base64<wchar_t>(2);
    test_base64<wchar_t>(3);
    test_base64<wchar_t>(4);
    test_base64<wchar_t>(150);
    #endif
}
