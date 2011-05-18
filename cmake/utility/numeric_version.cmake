macro(NUMERIC_VERSION _version _prefix)
  string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.?([0-9]*)"
    tmp_OUT
    ${_version})
  if(NOT CMAKE_MATCH_3)
    set(CMAKE_MATCH_3 0)
  endif()

  if((${CMAKE_MATCH_2} GREATER 99) OR (${CMAKE_MATCH_3} GREATER 99))
    message(FATAL_ERROR "Don't know how to handle ${_version}")
  else()
    string(TOUPPER "${_prefix}" prefix)
    math(EXPR ${prefix}_NUMERIC_VERSION "${CMAKE_MATCH_1} * 10000 + ${CMAKE_MATCH_2} * 100 + ${CMAKE_MATCH_3}")
  endif()
endmacro(NUMERIC_VERSION version prefix)
