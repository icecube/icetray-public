#
#  cmake/tools/minuit2.cmake
#
#  Copyright (C) 2010
#  the IceCube Collaboration <http://www.icecube.wisc.edu>
#
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#
TOOLDEF (minuit2
    include/Minuit2-5.24.00
    Minuit2/MnConfig.h
    lib/Minuit2-5.24.00
    NONE  # bin is n/a, placeholder
    Minuit2
    )
