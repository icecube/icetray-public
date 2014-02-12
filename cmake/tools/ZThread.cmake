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
  find_path(ZTHREAD_INCLUDE_DIR
    NAMES include/zthread/ZThread.h)
TOOLDEF (ZThread
  ${ZTHREAD_INCLUDE_DIR}
  ZThread.h
  lib
  NONE
  ZThread
  )
else (SYSTEM_PACKAGES)

TOOLDEF (ZThread
  include/zthread-2.3.2
  zthread/ZThread.h
  lib/zthread-2.3.2
  bin/zthread-2.3.2
  ZThread
  )

endif (SYSTEM_PACKAGES)
