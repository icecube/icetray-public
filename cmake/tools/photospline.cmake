#
#  $Id: $
#  

colormsg(HICYAN "")
colormsg(HICYAN "photospline")

find_package(photospline QUIET)
set(PHOTOSPLINE_FOUND ${photospline_FOUND})
if (photospline_FOUND)
  set(PHOTOSPLINE_LIBRARIES photospline)
  message(STATUS "+  Version: ${photospline_VERSION}")
  message(STATUS "+ Location: ${photospline_DIR}")
else ()
  message(STATUS "- photospline *not* found")
endif ()
