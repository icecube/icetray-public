i3_project(cmake
  DOCS_DIR resources/docs)

add_subdirectory(meta-project-docs)

configure_file(review.in ${I3_BUILD}/bin/review @ONLY)
configure_file(branch-metaproject.in ${I3_BUILD}/bin/branch-metaproject @ONLY)
execute_process(COMMAND chmod a+x ${I3_BUILD}/bin/review ${I3_BUILD}/bin/branch-metaproject)
