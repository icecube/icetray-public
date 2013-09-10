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

## as far as I know, only pfauxiliary uses expat. it should probably be
## tested with a version > 1.2

if (SYSTEM_PACKAGES)
  find_path(EXPAT_INCLUDE_DIR
    NAMES include/expat.h)
  TOOLDEF (expat
    ${EXPAT_INCLUDE_DIR}
    expat.h
    lib
    NONE
    expat)
else (SYSTEM_PACKAGES)  
  TOOLDEF (expat
    include/expat-1.2
    xmlparse/xmlparse.h
    lib/expat-1.2
    NONE
    expat)
endif (SYSTEM_PACKAGES)
  
