#
#  $Id$
#
i3_project(interfaces
  DOCS_DIR resources/docs)

i3_add_library(interfaces
  private/interfaces/*.cxx
  USE_PROJECTS icetray
  USE_TOOLS boost
  )

i3_test_executable(test 
  private/test/main.cxx
  USE_PROJECTS icetray interfaces
  )

i3_test_scripts(resources/test/*.py)

add_subdirectory(private/pybindings)

