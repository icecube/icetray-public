colormsg("")
colormsg(HICYAN "doxygen")

find_package(doxygen QUIET)
if(DOXYGEN_FOUND)
  execute_process(COMMAND ${DOXYGEN_EXECUTABLE} --version
    OUTPUT_VARIABLE DOXYGEN_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif(DOXYGEN_FOUND)
