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
MESSAGE(STATUS "jni")

SET(_jni_PATH_SUFFIXES jre/lib/i386 jre/lib/i386/server jre/lib/amd64 jre/lib/amd64/server jre/lib/ia64 jre/lib/ia64/server lib ../Libraries)

FIND_PATH(jni_h_include_dir
  NAMES jni.h
  PATHS
  $ENV{JAVA_HOME}/include
  $ENV{JAVA_HOME}/include/linux
  ${TOOL_SYSTEM_PATH}
  )
REPORT_FIND(jni jni.h ${jni_h_include_dir})

FIND_PATH(jni_md_h_include_dir
  NAMES jni_md.h
  PATHS
  $ENV{JAVA_HOME}/include
  $ENV{JAVA_HOME}/include/linux
  ${TOOL_SYSTEM_PATH}
  )
REPORT_FIND(jni jni_md.h ${jni_md_h_include_dir})

#
# libjvm
#
FIND_LIBRARY(jni_jvm_lib
  NAMES jvm
  PATHS $ENV{JAVA_HOME}  
  PATH_SUFFIXES ${_jni_PATH_SUFFIXES}
  ${TOOL_SYSTEM_PATH}
)
REPORT_FIND(jni "jvm library" ${jni_jvm_lib})

#
# libjvmlinkage
#
IF(${CMAKE_SYSTEM} MATCHES "Darwin-10.3.0")
FIND_LIBRARY(jni_jvmlinkage_lib
	NAMES jvmlinkage
	PATHS $ENV{JAVA_HOME}
	PATH_SUFFIXES ${_jni_PATH_SUFFIXES}
	${TOOL_SYSTEM_PATH}
)
REPORT_FIND(jni "jvmlinkage library" ${jni_jvm_lib})
ENDIF(${CMAKE_SYSTEM} MATCHES "Darwin-10.3.0")

#
# libverify
#
FIND_LIBRARY(jni_verify_lib
  NAMES verify
  PATHS $ENV{JAVA_HOME}  
  PATH_SUFFIXES ${_jni_PATH_SUFFIXES}
  ${TOOL_SYSTEM_PATH}
)
REPORT_FIND(jni "verify library" ${jni_verify_lib})

IF(NOT APPLE)
  #
  # libzip
  #
  FIND_LIBRARY(jni_zip_lib
    NAMES zip
    PATHS $ENV{JAVA_HOME}    
    PATH_SUFFIXES ${_jni_PATH_SUFFIXES}
    ${TOOL_SYSTEM_PATH}
  )
  REPORT_FIND(jni "zip library" ${jni_zip_lib})
ELSE(NOT APPLE)
  SET(jni_zip_lib "")
ENDIF(NOT APPLE)

IF(APPLE)
  SET(JNI_LINK_FLAGS "-framework JavaVM")
ENDIF(APPLE)

IF(NOT IS_DIRECTORY $ENV{JAVA_HOME})
  FOUND_NOT_OK("JAVA_HOME ($ENV{JAVA_HOME}) is either unset or does not point to a valid directory.")
  FOUND_NOT_OK("This is only an error if some project in your workspace requires java.")
ELSE(NOT IS_DIRECTORY $ENV{JAVA_HOME})
  FOUND_OK("JAVA_HOME=$ENV{JAVA_HOME}")
ENDIF(NOT IS_DIRECTORY $ENV{JAVA_HOME})

SET(JNI_INCLUDE_DIR ${jni_h_include_dir} ${jni_md_h_include_dir}
  CACHE STRING "Include dirs for jni" FORCE)

SET(JNI_LIBRARIES ${jni_jvm_lib} ${jni_verify_lib} ${jni_zip_lib}
  CACHE STRING "Libraries for tool jni" FORCE)
IF(${CMAKE_SYSTEM} MATCHES "Darwin-10.3.0")
SET(JNI_LIBRARIES ${jni_jvm_lib} ${jni_jvmlinkage_lib} ${jni_verify_lib} ${jni_zip_lib}
  CACHE STRING "Libraries for tool jni" FORCE)
ENDIF(${CMAKE_SYSTEM} MATCHES "Darwin-10.3.0")

SET(JNI_FOUND TRUE CACHE BOOL "Jni found flag" FORCE)

