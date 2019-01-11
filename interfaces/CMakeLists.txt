#
#  $Id: CMakeLists.txt 165886 2018-10-01 14:37:58Z nwhitehorn $
#
i3_project(interfaces)

i3_add_library(interfaces
  private/interfaces/*.cxx
  USE_PROJECTS icetray
  USE_TOOLS log4cplus boost
  )

i3_test_executable(test 
  private/test/main.cxx
  USE_PROJECTS icetray interfaces
  )

i3_test_scripts(resources/scripts/*.py)

add_subdirectory(private/pybindings)