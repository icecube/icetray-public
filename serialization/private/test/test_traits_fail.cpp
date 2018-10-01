/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_traits_fail.cpp: test implementation level trait

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// compile test for traits
#include <serialization/level.hpp>
#include <serialization/version.hpp>

namespace{
class A{};
}

I3_CLASS_IMPLEMENTATION(A, icecube::serialization::not_serializable)
// It can make no sense to assign a version number to a class that 
// is not serialized with class information
I3_CLASS_VERSION(A, 2) // should fail during compile
// It can make no sense to assign tracking behavior to a class that 
// is not serializable. Should fail during compile.
I3_CLASS_TRACKING(A, boost::serialization::track_never)

class B
{
};

I3_CLASS_IMPLEMENTATION(B, icecube::serialization::object_class_info)
I3_CLASS_VERSION(B, 2)
I3_CLASS_TRACKING(B, icecube::serialization::track_always)

// EOF
