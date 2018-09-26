//
//   Copyright (c) 2009   Troy D. Straszheim and the IceCube Collaboration 
//   
//   This file is part of IceTray.
//
//   IceTray is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
//   IceTray is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

using namespace boost::python;
namespace bp = boost::python;

#include <phys-services/I3CascadeCutValues.h>

void register_I3CascadeCutValues()
{
#define MEMBERS (Nchan)(Nhit)(Nstring)(Ndir)(Nearly)(Nlate)(cog)

  class_<I3CascadeCutValues, bases<I3FrameObject>, I3CascadeCutValuesPtr>("I3CascadeCutValues")
    BOOST_PP_SEQ_FOR_EACH(WRAP_RW_RECASE, I3CascadeCutValues, MEMBERS)
    #undef MEMBERS
    .def_readwrite("n_1hit", &I3CascadeCutValues::N_1hit)
    ;

  register_pointer_conversions<I3CascadeCutValues>();
}
