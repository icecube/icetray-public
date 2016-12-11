/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// A.ipp    simple class test

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/detail/workaround.hpp>
#if ! BOOST_WORKAROUND(BOOST_MSVC, <= 1300)

#if BOOST_WORKAROUND(BOOST_DINKUMWARE_STDLIB, == 1)
#include <archive/dinkumware.hpp>
#endif

#include "A.hpp"

template<class Archive>
void A::serialize(
    Archive &ar,
    const unsigned int /* file_version */
){
    ar & I3_SERIALIZATION_NVP(b);
    #ifndef BOOST_NO_INT64_T
    ar & I3_SERIALIZATION_NVP(f);
    ar & I3_SERIALIZATION_NVP(g);
    #endif
    #if BOOST_WORKAROUND(__BORLANDC__,  <= 0x551 )
        int i;
        if(BOOST_DEDUCED_TYPENAME Archive::is_saving::value){
            i = l;
            ar & I3_SERIALIZATION_NVP(i);
        }
        else{
            ar & I3_SERIALIZATION_NVP(i);
            l = i;
        }
    #else
        ar & I3_SERIALIZATION_NVP(l);
    #endif
    ar & I3_SERIALIZATION_NVP(m);
    ar & I3_SERIALIZATION_NVP(n);
    ar & I3_SERIALIZATION_NVP(o);
    ar & I3_SERIALIZATION_NVP(p);
    ar & I3_SERIALIZATION_NVP(q);
    #ifndef BOOST_NO_CWCHAR
    ar & I3_SERIALIZATION_NVP(r);
    #endif
    ar & I3_SERIALIZATION_NVP(c);
    ar & I3_SERIALIZATION_NVP(s);
    ar & I3_SERIALIZATION_NVP(t);
    ar & I3_SERIALIZATION_NVP(u);
    ar & I3_SERIALIZATION_NVP(v);
    ar & I3_SERIALIZATION_NVP(w);
    ar & I3_SERIALIZATION_NVP(x);
    ar & I3_SERIALIZATION_NVP(y);
    #ifndef BOOST_NO_STD_WSTRING
    ar & I3_SERIALIZATION_NVP(z);
    #endif
}

#endif // workaround BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
