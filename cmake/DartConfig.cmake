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
SET(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)
SET(NIGHTLY_START_TIME "00:01:00 EST")

SET(DART_TESTING_TIMEOUT 600)

SET(CTEST_NOTES_FILES 
  ${CMAKE_BINARY_DIR}/svn_info.txt
  ${CMAKE_BINARY_DIR}/svn_externals.txt
  ${CMAKE_BINARY_DIR}/env.txt
  )



