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
TOOLDEF (hdf5
  include
  hdf5.h
  lib
  NONE
  hdf5 hdf5_hl 
  )

if(NOT HDF5_FOUND)

  unset(HDF5_CONFIG_ERROR)
  unset(HDF5_INCLUDE_DIR)

  TOOLDEF (hdf5
    /usr/include/hdf5/serial
    hdf5.h
    lib/x86_64-linux-gnu
    NONE
    hdf5_serial hdf5_serial_hl 
    )

  if(HDF5_FOUND)
    set(HDF5_FOUND TRUE CACHE BOOL "Tool HDF5 found successfully")
    set(HDF5_LIBRARIES "${HDF5_LIBRARIES}" CACHE PATH "Libraries for tool HDF5")
  endif(HDF5_FOUND)

endif(NOT HDF5_FOUND)
