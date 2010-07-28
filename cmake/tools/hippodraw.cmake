# make sure this file comes after "python" in "set(ALL_TOOLS" in tools.cmake

message(STATUS "hippodraw")
find_library(HIPPO_LIB NAMES hippo PATHS ${I3_PORTS}/lib ${TOOL_SYSTEM_PATH})

if(PYTHON_VERSION)
  if(${PYTHON_VERSION} MATCHES "Python 2.4")
    set(pyver "python2.4")
  endif(${PYTHON_VERSION} MATCHES "Python 2.4")
  if(${PYTHON_VERSION} MATCHES "Python 2.5")
    set(pyver "python2.5")
  endif(${PYTHON_VERSION} MATCHES "Python 2.5")
  if(${PYTHON_VERSION} MATCHES "Python 2.6")
    set(pyver "python2.6")
  endif(${PYTHON_VERSION} MATCHES "Python 2.6")
else()
  message(FATAL_ERROR "whoa! where's python?")
endif()

if(HIPPO_LIB)
  find_path(HIPPO_PYTHON hippomodule.so
    PATHS
    ${I3_PORTS}/lib64/${pyver}/site-packages
    ${I3_PORTS}/lib64/${pyver}/dist-packages
    ${I3_PORTS}/lib/${pyver}/site-packages
    ${I3_PORTS}/lib/${pyver}/dist-packages)

  find_path(HIPPO_LIBRARY_DIR libhippo.so
    PATHS
    ${I3_PORTS}/lib ${TOOL_SYSTEM_PATH})
endif(HIPPO_LIB)

if(HIPPO_LIB AND HIPPO_PYTHON AND HIPPO_LIBRARY_DIR)
  set(HIPPO_FOUND 1)
endif(HIPPO_LIB AND HIPPO_PYTHON AND HIPPO_LIBRARY_DIR)

if(HIPPO_FOUND)
  message(STATUS "+  library: ${HIPPO_LIB}")
  message(STATUS "+   python: ${HIPPO_PYTHON}")
endif(HIPPO_FOUND)

