#
# generate a "false" url with some system information. this url
# can then be parsed on the server for gathering metrics
#

#include(CMakePrintHelpers)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  execute_process(COMMAND sh -c "cat /sys/class/net/*/address"
    COMMAND md5sum
    COMMAND awk "{print $1}"
    COMMAND tr -d \\n
    OUTPUT_VARIABLE MACHASH
    ERROR_QUIET)
elseif(APPLE)
  execute_process(COMMAND networksetup -listallhardwareports
    COMMAND grep ^Ether
    COMMAND awk "{print $3}"
    COMMAND md5
    COMMAND awk "{print $1}"
    COMMAND tr -d \\n
    OUTPUT_VARIABLE MACHASH
    ERROR_QUIET)
else()
  set(MACHASH "")
endif()
set(MACHASH ${MACHASH} CACHE INTERNAL "MD5 hash of the system MAC addresses")

if(NOT APPLE)
  execute_process(COMMAND hostid
    COMMAND tr -d \\n
    OUTPUT_VARIABLE HOSTID
    ERROR_QUIET)
else()
  execute_process(COMMAND sysctl kern.hostid
    COMMAND tr -d \\n
    OUTPUT_VARIABLE HOSTID
    ERROR_QUIET)
endif()
set(HOSTID ${HOSTID} CACHE INTERNAL "HOSTID ala gethostid()")

set(d "")
foreach(v MACHASH HOSTID HOSTNAME COMPILER_ID_TAG CMAKE_VERSION CMAKE_INSTALL_PREFIX)
  set(d "${d}${v}:${${v}},")
endforeach()

foreach(v Boost ROOT PYTHON GEANT4 GENIE)
  set(v "${v}_VERSION")
  set(d "${d}${v}:${${v}},")
endforeach()

execute_process(COMMAND ${CMAKE_SOURCE_DIR}/cmake/system_report.py ${d}
                OUTPUT_QUIET
                ERROR_QUIET)
