# PAPER         =
# 
# # Internal variables.
# PAPEROPT_a4     = -D latex_paper_size=a4
# PAPEROPT_letter = -D latex_paper_size=letter
# ALLSPHINXOPTS   = -d $(I3_BUILD)/docs/build/doctrees $(PAPEROPT_$(PAPER)) $(SPHINXOPTS) source
# 

set(SPHINX_EXTRA_OPTIONS "-N;-a;-E" CACHE STRING "Sphinx options")

find_program(SPHINX_BUILD sphinx-build)

if(SPHINX_BUILD)
  colormsg(GREEN "+-- sphinx-build found, building sphinx documentation")
  set(SPHINX_PAPER_SIZE "letter" CACHE STRING "Size of paper when generating latex/pdf of documentation.  Options are 'letter' or 'a4'")
  set(SPHINX_EXTRA_OPTIONS "" CACHE STRING "Extra options to pass to sphinx")

  add_custom_target(html
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees 
    COMMAND ln -fsn ${CMAKE_SOURCE_DIR}/docs ${CMAKE_CURRENT_SOURCE_DIR}/source/metaproject
    COMMAND ln -fsn ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/project-doc-links ${CMAKE_CURRENT_SOURCE_DIR}/source/projects
    COMMAND 
    ${CMAKE_BINARY_DIR}/env-shell.sh ${SPHINX_BUILD} ${SPHINX_EXTRA_OPTIONS} 
    -b html 
    -d ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees 
    -D latex_paper_size=${SPHINX_PAPER_SIZE} 
    source
    ${CMAKE_BINARY_DIR}/docs
    DEPENDS icetray-pybindings
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating docs to ${CMAKE_BINARY_DIR}/docs")

  add_custom_target(latex
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/latex ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees
    COMMAND ${CMAKE_BINARY_DIR}/env-shell.sh ${SPHINX_BUILD} 
    ${SPHINX_EXTRA_OPTIONS} 
    -b latex 
    -d ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/doctrees 
    -D latex_paper_size=${SPHINX_PAPER_SIZE} 
    source
    ${CMAKE_BINARY_DIR}/latex
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating latex to ${CMAKE_BINARY_DIR}/latex")

  add_custom_target(post
    COMMAND rsync -va --delete ${CMAKE_BINARY_DIR}/docs/ pub.icecube.wisc.edu:/net/software/${META_PROJECT}/
    COMMENT "Deploying to http://software.icecube.wisc.edu/${META_PROJECT}"
    )

execute_process(COMMAND svn pg svn:externals ${CMAKE_SOURCE_DIR}
  OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/source/svn-externals.txt)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/metaproject_metainfo.rst.in ${CMAKE_CURRENT_SOURCE_DIR}/source/metaproject_metainfo.rst
    @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in ${CMAKE_CURRENT_SOURCE_DIR}/source/conf.py
    @ONLY)
else()
  colormsg(CYAN "+-- sphinx-build *not* found, disabling build of sphinx documentation.
       See docs at http://software.icecube.wisc.edu/offline_trunk/
       for details.")
endif()
