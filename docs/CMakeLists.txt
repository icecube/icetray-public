
find_program(SPHINX_BUILD sphinx-build)

## this is for extensions in the "sphinx.ext" namespace, and could
## be made more generic. the file "conf.py.in" needs to have the
## corresponding variables inserted in the "extensions" directive.
foreach(e "napoleon")
    string(TOUPPER ${e} eU)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import sphinx.ext.${e}"
      RESULT_VARIABLE sphinx_${eU}
      OUTPUT_QUIET
      ERROR_QUIET)
    if(NOT sphinx_${eU})
      set(sphinx_${eU} "sphinx.ext.${e}")
    else()
      set(sphinx_${eU} "")
    endif()
  endforeach()
  unset(e)
  unset(eU)

## for sphinxcontrib-email
foreach(e "email" "traclinks")
    string(TOUPPER ${e} eU)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import sphinxcontrib.${e}"
      RESULT_VARIABLE sphinx_${eU}
      OUTPUT_QUIET
      ERROR_QUIET)
    if(NOT sphinx_${eU})
      set(sphinx_${eU} "sphinxcontrib.${e}")
    else()
      set(sphinx_${eU} "")
    endif()
  endforeach()
  unset(e)
  unset(eU)

## for sphinxcontrib-email
foreach(e "breathe" )
    string(TOUPPER ${e} eU)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import ${e}"
      RESULT_VARIABLE sphinx_${eU}
      OUTPUT_QUIET
      ERROR_QUIET)
    if(NOT sphinx_${eU})
      set(sphinx_${eU} "${e}")
    else()
      set(sphinx_${eU} "")
    endif()
  endforeach()
  unset(e)
  unset(eU)


i3_project(docs)

i3_executable_script(build resources/scripts/docs-build.py)

set(SPHINX_DIR "${CMAKE_BINARY_DIR}/docs")

configure_file(
    ${CMAKE_SOURCE_DIR}/docs/conf/metaproject_metainfo.rst.in
    ${SPHINX_DIR}/conf/metaproject_metainfo.rst
    @ONLY)

configure_file(
    ${CMAKE_SOURCE_DIR}/docs/conf/conf.py.in
    ${SPHINX_DIR}/conf/conf.py
    @ONLY)
    
execute_process(COMMAND svn pg svn:externals ${CMAKE_SOURCE_DIR}
  OUTPUT_FILE ${SPHINX_DIR}/conf/svn-externals.txt
  ERROR_QUIET)

