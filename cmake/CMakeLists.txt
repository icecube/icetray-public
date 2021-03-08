i3_project(cmake
  DOCS_DIR resources/docs)

configure_file(get_failed_tests.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/get_failed_tests.cmake @ONLY)

configure_file(coverage.sh.in ${I3_BUILD}/bin/coverage.sh @ONLY)
execute_process(COMMAND chmod a+x ${I3_BUILD}/bin/coverage.sh)
