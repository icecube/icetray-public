#
#  copyright (C) 2008
#  troy d. straszheim  <troy@resophonic.com>
#  
#  This file is part of Icetray
#  
#  Traash is free software; you can redistribute it and/or modify
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
set(TMP_LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH})

i3_add_pybindings(icetray
  empty.cxx
  USE_TOOLS boost python log4cplus
  USE_PROJECTS icetray
  )

# 
# These are special, they go in the toplevel 'lib' directory, for backwards-compatibility
#
foreach(pyfile I3Tray.py I3Units.py)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${pyfile} 
    DESTINATION lib)

  add_custom_command(OUTPUT ${TMP_LIBRARY_OUTPUT_PATH}/${pyfile}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${pyfile}
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/${pyfile} ${TMP_LIBRARY_OUTPUT_PATH}
    )
  add_custom_target(${pyfile}
    DEPENDS ${TMP_LIBRARY_OUTPUT_PATH}/${pyfile}
    )
  add_dependencies(icetray-pybindings ${pyfile})
endforeach(pyfile I3Tray.py I3Units.py)


