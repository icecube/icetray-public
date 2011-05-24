## This macro takes an x.y.z style version number and converts it to an integer.
## It attempts to protect against overflow, where the y or z component is
## greater than 99. This "version integer" has limited utility due to the 
## following:
##
##    2.3.456 == 2.7.56 
##    2.3.45-patch9 == 2.3.45
##
## The macro takes 2 arguments. The first, an x.y.z style version. The second,
## a string prefix. The prefix is upcased and returned as part of the variable
## name. For instance, numeric_version(2.3.45 "foo") will return a variable
## called FOO_NUMERIC_VERSION containing '20345'.

macro(NUMERIC_VERSION _version _prefix)
  string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.?([0-9]*)"
    tmp_OUT
    ${_version})
  if(NOT CMAKE_MATCH_3)
    set(CMAKE_MATCH_3 0)
  endif()

  if((${CMAKE_MATCH_2} GREATER 99) OR (${CMAKE_MATCH_3} GREATER 99))
    message(FATAL_ERROR "Don't know how to handle version number: ${_version}")
  else()
    string(TOUPPER "${_prefix}" prefix)
    math(EXPR ${prefix}_NUMERIC_VERSION "${CMAKE_MATCH_1} * 10000 + ${CMAKE_MATCH_2} * 100 + ${CMAKE_MATCH_3}")
  endif()
endmacro(NUMERIC_VERSION version prefix)
