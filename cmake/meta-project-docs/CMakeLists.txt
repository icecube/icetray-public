# PAPER         =
# 
# # Internal variables.
# PAPEROPT_a4     = -D latex_paper_size=a4
# PAPEROPT_letter = -D latex_paper_size=letter
# ALLSPHINXOPTS   = -d $(I3_BUILD)/docs/build/doctrees $(PAPEROPT_$(PAPER)) $(SPHINXOPTS) source
# 

set(SPHINX_EXTRA_OPTIONS "-N;-a;-E" CACHE STRING "Sphinx options")

find_program(SPHINX_BUILD sphinx-build)

execute_process(COMMAND touch test_write
		COMMAND rm -f test_write
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		RESULT_VARIABLE SPHINX_DIR_NOT_WRITABLE
		OUTPUT_VARIABLE QUIET
		ERROR_VARIABLE QUIET)

if(SPHINX_BUILD AND NOT SPHINX_DIR_NOT_WRITABLE)
  colormsg(GREEN "+-- sphinx-build found, building sphinx documentation")
  set(SPHINX_PAPER_SIZE "letter" CACHE STRING "Size of paper when generating latex/pdf of documentation.  Options are 'letter' or 'a4'")
  set(SPHINX_EXTRA_OPTIONS "" CACHE STRING "Extra options to pass to sphinx")

  ## this is for extensions in the "sphinx.ext" namespace, and could
  ## be made more generic. the file "conf.py.in" needs to have the
  ## corresponding variables inserted in the "extensions" directive.
  foreach(e "napoleon" "email")
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

  add_custom_target(html
    COMMAND mkdir -p
    	    ${SPHINX_DIR} 
    	    ${SPHINX_DIR}/build 
            ${CMAKE_BINARY_DIR}/docs
            ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/cmake/meta-project-docs/ ${SPHINX_DIR}	   
    COMMAND ln -fsn ${CMAKE_SOURCE_DIR}/docs
            ${SPHINX_DIR}/source/metaproject
    COMMAND ${CMAKE_BINARY_DIR}/env-shell.sh ${SPHINX_BUILD}
            ${SPHINX_EXTRA_OPTIONS} 
    -b html 
    -d ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees 
    -D latex_paper_size=${SPHINX_PAPER_SIZE} 
    ${SPHINX_DIR}/source 
    ${CMAKE_BINARY_DIR}/docs
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating docs to ${CMAKE_BINARY_DIR}/docs")

  add_custom_target(latex
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/latex ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees
    COMMAND ${CMAKE_BINARY_DIR}/env-shell.sh ${SPHINX_BUILD} 
    ${SPHINX_EXTRA_OPTIONS} 
    -b latex 
    -d ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees 
    -D latex_paper_size=${SPHINX_PAPER_SIZE} 
    ${SPHINX_DIR}/source 
    ${CMAKE_BINARY_DIR}/latex
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating latex to ${CMAKE_BINARY_DIR}/latex")

  add_custom_target(post
    COMMAND rsync -va --delete ${CMAKE_BINARY_DIR}/docs/ pub.icecube.wisc.edu:/net/software/${META_PROJECT}/
    COMMENT "Deploying to http://software.icecube.wisc.edu/${META_PROJECT}"
    )

execute_process(COMMAND svn pg svn:externals ${CMAKE_SOURCE_DIR}
  OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/source/svn-externals.txt)

  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/meta-project-docs/metaproject_metainfo.rst.in
    ${SPHINX_DIR}/source/metaproject_metainfo.rst
    @ONLY)

  configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/meta-project-docs/conf.py.in
    ${SPHINX_DIR}/source/conf.py
    @ONLY)
    
elseif(NOT SPHINX_BUILD)
  colormsg(CYAN "+-- sphinx-build *not* found, disabling build of sphinx documentation.
       See docs at http://software.icecube.wisc.edu/offline_trunk/
       for details.")
else()
  colormsg(CYAN "+-- cmake docs source *not* writable, disabling build of sphinx documentation.
       See docs at http://software.icecube.wisc.edu/offline_trunk/
       for details.")
endif()
