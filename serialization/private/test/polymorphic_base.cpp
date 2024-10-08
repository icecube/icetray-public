/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_base.cpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// SPDX-License-Identifier: BSL-1.0
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <serialization/export.hpp>
#include "polymorphic_base.hpp"

I3_CLASS_EXPORT_IMPLEMENT(polymorphic_base)

const char * polymorphic_base::get_key() const{
    return
        icecube::serialization::type_info_implementation<
            polymorphic_base
        >::type::get_const_instance().get_key();
}
