#
#  $Id$
#  
#  Copyright (C) 2012 the IceCube Collaboration <http://www.icecube.wisc.edu>
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
  find_path(ZMQ_INCLUDE_DIR
    NAMES include/zmq.hpp)
  tooldef(zmq
    ${ZMQ_INCLUDE_DIR}
    zmq.hpp
    lib
    NONE
    zmq
    )
else (SYSTEM_PACKAGES)
  if(NOT ZMQ_VERSION)
    foreach(ZMQVER 3.2.4 2.1.11)
      if(IS_DIRECTORY ${I3_PORTS}/include/zmq-${ZMQVER})
        set(ZMQ_VERSION "${ZMQVER}")
        break()
      endif()
    endforeach()
  endif(NOT ZMQ_VERSION)
  TOOLDEF (zmq
    include/zmq-${ZMQ_VERSION}
    zmq.hpp 
    lib/zmq-${ZMQ_VERSION}
    NONE
    zmq
    )
endif (SYSTEM_PACKAGES)
