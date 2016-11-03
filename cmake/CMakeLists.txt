i3_project(cmake
  DOCS_DIR resources/docs)

i3_executable_script(docs resources/scripts/cmake-docs.py)

add_subdirectory(meta-project-docs)

configure_file(get_failed_tests.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/get_failed_tests.cmake @ONLY)

configure_file(review.in ${I3_BUILD}/bin/review @ONLY)
configure_file(coverage.sh.in ${I3_BUILD}/bin/coverage.sh @ONLY)
configure_file(branch-metaproject.in ${I3_BUILD}/bin/branch-metaproject @ONLY)
execute_process(COMMAND chmod a+x ${I3_BUILD}/bin/review ${I3_BUILD}/bin/coverage.sh ${I3_BUILD}/bin/branch-metaproject)
