#
#  $Id$
#  
#  Copyright (C) 2007
#  Troy D. Straszheim  <troy@icecube.umd.edu>
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
i3_project(dataio
  PYTHON_DIR python
  DOCS_DIR resources/docs
  )

i3_add_library (dataio 
  private/dataio-modules/*.cxx private/dataio/*.cxx
  USE_PROJECTS icetray dataclasses interfaces
  USE_TOOLS log4cplus root boost python 
)
target_link_libraries(dataio ${BOOST_DATE_TIME})

i3_executable(shovel private/shovel/*.cxx
  USE_PROJECTS dataio
  USE_TOOLS cdk ncurses
  )

i3_executable(extract private/extract/*.cxx
  USE_PROJECTS dataio)

i3_executable(strip private/strip/*.cxx
  USE_PROJECTS dataio)

i3_executable(scan private/scan/*.cxx
  USE_PROJECTS dataio)

i3_test_executable(test private/test/*.cxx
  USE_PROJECTS dataio)

no_dotfile_glob(tmp_SCRIPTS resources/test/*.py)
if (NOT LIBARCHIVE_FOUND)
  LIST(REMOVE_ITEM tmp_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/test/o7_libarchive_read.py")
endif (NOT LIBARCHIVE_FOUND)
i3_test_scripts(${tmp_SCRIPTS})

add_subdirectory(private/pybindings)
