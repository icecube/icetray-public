/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_traits_pass.cpp: test implementation level trait

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// compile test for traits

#include <serialization/level.hpp>
#include <serialization/tracking.hpp>
#include <serialization/version.hpp>

class B
{
};

I3_CLASS_IMPLEMENTATION(B, icecube::serialization::object_class_info)
I3_CLASS_VERSION(B, 2)
I3_CLASS_TRACKING(B, icecube::serialization::track_always)

// EOF
