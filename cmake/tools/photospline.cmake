#
#  $Id: $
#  

colormsg(HICYAN "")
colormsg(HICYAN "photospline")

find_package(photospline REQUIRED)
set(PHOTOSPLINE_FOUND ${photospline_FOUND})
if (photospline_FOUND)
  set(PHOTOSPLINE_LIBRARIES photospline)
  message(STATUS "+ ${photospline_DIR}")
else ()
  message(STATUS "- photospline not found")
endif ()
