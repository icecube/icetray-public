#
#  $Id: $
#  

colormsg(HICYAN "")
colormsg(HICYAN "photospline")

find_package(photospline REQUIRED)
set(PHOTOSPLINE_FOUND ${photospline_FOUND})
if (photospline_FOUND)
  set(PHOTOSPLINE_LIBRARIES photospline)
  message(STATUS "+  Version: ${photospline_VERSION}")
  message(STATUS "+ Location: ${photospline_DIR}")
else ()
  message(STATUS "- photospline not found")
endif ()

# get_cmake_property(_variableNames VARIABLES)
#   list (SORT _variableNames)
#   foreach (_variableName ${_variableNames})
#     message("${_variableName} : ${${_variableName}}")
#   endforeach()
#   set(_variableNames "")