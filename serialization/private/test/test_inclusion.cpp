/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_const.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <serialization/access.hpp>
#include <serialization/base_object.hpp>
#include <serialization/export.hpp>
#include <serialization/level.hpp>
#include <serialization/level_enum.hpp>
#include <serialization/nvp.hpp>
#include <serialization/split_free.hpp>
#include <serialization/split_member.hpp>
#include <serialization/tracking.hpp>
#include <serialization/tracking_enum.hpp>
#include <serialization/traits.hpp>
#include <serialization/type_info_implementation.hpp>
#include <serialization/version.hpp> 

struct foo
{
    int x;
private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // In compilers implementing 2-phase lookup, the call to
        // make_nvp is resolved even if foo::serialize() is never
        // instantiated.
        ar & boost::serialization::make_nvp("x",x);
    }
};

int
main(int /*argc*/, char * /*argv*/[]){
    return 0;
}
