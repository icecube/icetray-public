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
find_package(Curses)

# 
if(NOT CURSES_FOUND)

  set(NCURSES_FOUND FALSE)

else(NOT CURSES_FOUND)

  set(NCURSES_FOUND TRUE)

  set(NCURSES_INCLUDE_DIR ${CURSES_INCLUDE_PATH} 
    CACHE STRING "Include dir for tool 'ncurses'")
  set(NCURSES_LIBRARIES ${CURSES_LIBRARIES}
    CACHE STRING "Libraries for tool 'ncurses'")

endif(NOT CURSES_FOUND)

