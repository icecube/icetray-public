## This macro takes 2 arguments. The first, _d, is an "integer date"
## in the format of "YYYYmmdd". The second, _m, is a message that will
## be passed to message(WARNING ...) or message(FATAL_ERROR ...). A
## one line message is recomended.
##
## It is recomended that you provide some explanitory warning prior to
## calling this macro.

macro(deprecation_warning _d _m)
  if(NOT APPLE)
    execute_process(COMMAND "date" "+%s" "-d${_d}"
      COMMAND tr -d \\n
      OUTPUT_VARIABLE _hence)

      execute_process(COMMAND "date" "-d" "@${_hence}" "+'%B %d, %Y'"
      COMMAND tr -d \\n
      OUTPUT_VARIABLE _fhence)
  else()
    execute_process(COMMAND "date" "-j" "-f" "'%Y%m%d %T'" "'${_d} 00:00:00'" "+%s"
      COMMAND tr -d \\n
      OUTPUT_VARIABLE _hence)

    execute_process(COMMAND "date" "-j" "-r" "${_hence}" "+'%B %d, %Y'"
      COMMAND tr -d \\n
      OUTPUT_VARIABLE _fhence)
  endif()

  execute_process(COMMAND "date" "+%s"
    COMMAND tr -d \\n
    OUTPUT_VARIABLE _now)

  if(_now LESS _hence)
    message("***")
    message("*** This will become a fatal error on ${_fhence}")
    message("***")
    message(WARNING "${_m}")
  else()
    message("***")
    message(FATAL_ERROR "${_m}")
  endif()
endmacro()
