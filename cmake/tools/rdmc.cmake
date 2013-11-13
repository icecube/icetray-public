#
#  $Id$
#  
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
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

if (SYSTEM_PACKAGES)
  find_path(RDMC_ROOT_DIR
    NAMES include/rdmc-2.9.5/rdmc/rdmc.h
  )
TOOLDEF (rdmc
    ${RDMC_ROOT_DIR}/include/rdmc-2.9.5
    rdmc/rdmc.h
    ${RDMC_ROOT_DIR}/lib/rdmc-2.9.5
    NONE  # bin is n/a, placeholder
    rdmc
    )
else (SYSTEM_PACKAGES)
TOOLDEF (rdmc
    include/rdmc-2.9.5
    rdmc/rdmc.h
    lib/rdmc-2.9.5
    NONE  # bin is n/a, placeholder
    rdmc
    )
endif (SYSTEM_PACKAGES)
