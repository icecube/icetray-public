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

# we don't use the tooldef() macro, so we have to fudge pretty-printing
colormsg("")
colormsg(HICYAN "jni")

SET(_jni_PATH_SUFFIXES jre/lib/i386 jre/lib/i386/server jre/lib/amd64 jre/lib/amd64/server jre/lib/ia64 jre/lib/ia64/server lib ../Libraries)

IF(APPLE)
  FOUND_OK("Using the JavaVM Framework because we're on Apple")

  FIND_PATH(jni_h_include_dir
    NAMES jni.h
    PATHS
    /System/Library/Frameworks/JavaVM.framework/Headers
    $ENV{JAVA_HOME}/include
    ${TOOL_SYSTEM_PATH})
  REPORT_FIND(jni jni.h ${jni_h_include_dir})

  FIND_PATH(jni_md_h_include_dir
    NAMES jni_md.h
    PATHS
    $ENV{JAVA_HOME}/include
    /System/Library/Frameworks/JavaVM.framework/Headers
    ${TOOL_SYSTEM_PATH})
  REPORT_FIND(jni jni_md.h ${jni_md_h_include_dir})

#  FIND_LIBRARY(jni_jvm_lib JavaVM)
#  REPORT_FIND(jni "JavaVM library" ${jni_jvm_lib})

  SET(JNI_LINK_FLAGS "-framework JavaVM")
  FOUND_OK("Setting JNI_LINK_FLAGS to ${JNI_LINK_FLAGS}")

ELSE(APPLE)

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
  # libverify
  #
  FIND_LIBRARY(jni_verify_lib
    NAMES verify
    PATHS $ENV{JAVA_HOME}  
    PATH_SUFFIXES ${_jni_PATH_SUFFIXES}
    ${TOOL_SYSTEM_PATH}
    )
  REPORT_FIND(jni "verify library" ${jni_verify_lib})

  FIND_LIBRARY(jni_zip_lib
    NAMES zip
    PATHS $ENV{JAVA_HOME}    
    PATH_SUFFIXES ${_jni_PATH_SUFFIXES}
    ${TOOL_SYSTEM_PATH}
    )
  REPORT_FIND(jni "zip library" ${jni_zip_lib})

ENDIF(APPLE)

IF(NOT IS_DIRECTORY $ENV{JAVA_HOME})
  FOUND_NOT_OK("JAVA_HOME ($ENV{JAVA_HOME}) is either unset or does not point to a valid directory.")
  FOUND_NOT_OK("This is only an error if some project in your workspace requires java.")
ELSE(NOT IS_DIRECTORY $ENV{JAVA_HOME})
  FOUND_OK("JAVA_HOME=$ENV{JAVA_HOME}")
ENDIF(NOT IS_DIRECTORY $ENV{JAVA_HOME})

SET(JNI_INCLUDE_DIR ${jni_h_include_dir} ${jni_md_h_include_dir}
  CACHE STRING "Include dirs for jni" FORCE)

IF(NOT APPLE)  #if'd because we use Frameworks on Apple
SET(JNI_LIBRARIES ${jni_jvm_lib} ${jni_verify_lib} ${jni_zip_lib}
  CACHE STRING "Libraries for tool jni" FORCE)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(JNI_LIBRARIES ${jni_jvm_lib} ${jni_jvmlinkage_lib} ${jni_verify_lib} ${jni_zip_lib}
    CACHE STRING "Libraries for tool jni" FORCE)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
ENDIF(NOT APPLE)

SET(JNI_FOUND TRUE CACHE BOOL "Jni found flag" FORCE)
