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
if(NOT SYSTEM_PACKAGES)
TOOLDEF (mysql
    include/mysql-4.1.20
    mysql/mysql.h
    lib/mysql-4.1.20/mysql
    NONE
    mysqlclient
    )
endif()

if (NOT MYSQL_FOUND)
    set(MYSQL_CONFIG_ERROR)
    IF (MYSQL_INCLUDE_DIR)
        # Already in cache, be silent
        SET(MYSQL_FIND_QUIETLY TRUE)
    ENDIF (MYSQL_INCLUDE_DIR)

    FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
        /usr/local/include/mysql
        /usr/include/mysql
        )

    SET(MYSQL_NAMES mysqlclient mysqlclient_r)
    FIND_LIBRARY(MYSQL_LIBRARY
        NAMES ${MYSQL_NAMES}
        PATHS /usr/lib /usr/lib64 /usr/local/lib
        PATH_SUFFIXES mysql
        )

    IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
        SET(MYSQL_FOUND TRUE CACHE BOOL "Tool 'MYSQL' found successfully")
        SET(MYSQL_LIBRARIES ${MYSQL_LIBRARY} CACHE PATH "Libraries for tool MYSQL")
    ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
        SET(MYSQL_FOUND FALSE)
        SET(MYSQL_LIBRARIES)
    ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

    IF (MYSQL_FOUND)
        IF (NOT MYSQL_FIND_QUIETLY)
            MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
        ENDIF (NOT MYSQL_FIND_QUIETLY)
    ELSE (MYSQL_FOUND)
        IF (MYSQL_FIND_REQUIRED)
            MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
            MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
        ENDIF (MYSQL_FIND_REQUIRED)
    ENDIF (MYSQL_FOUND)

    MARK_AS_ADVANCED(
        MYSQL_LIBRARY
        MYSQL_INCLUDE_DIR
        )
endif (NOT MYSQL_FOUND)
