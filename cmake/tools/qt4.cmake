# we don't use the tooldef() macro, so we have to fudge pretty-printing
colormsg("")
colormsg(HICYAN "qt4")

include(CheckSymbolExists)
include(MacroAddFileDependencies)

function(_QT4_QUERY_QMAKE VAR RESULT)
  execute_process(COMMAND "${QT_QMAKE_EXECUTABLE}" -query ${VAR}
    RESULT_VARIABLE return_code
    OUTPUT_VARIABLE output ERROR_VARIABLE output
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE)
  if(NOT return_code)
    file(TO_CMAKE_PATH "${output}" output)
    set(${RESULT} ${output} PARENT_SCOPE)
  endif(NOT return_code)
endfunction(_QT4_QUERY_QMAKE)

set(Qt4_FIND_QUIETLY FALSE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_FILE ${CMAKE_ROOT}/Modules/UseQt4.cmake)
set(QT_DEFINITIONS "-qt=qt4")

if(NOT QT_QMAKE_EXECUTABLE)
  # check for qmake
  # first, try various paths one by one, looking for one that is recent enough
  set(_test_paths
    /usr/lib/x86_64-linux-gnu/qt4/bin
    ${I3_PORTS}/qt-4.8.6/bin)
  set(QTVERSION 0)
  foreach(_test_path ${_test_paths})
    find_program(QT_QMAKE_EXECUTABLE NAMES qmake-qt4 qmake
      PATHS
      ${_test_path}
      DOC "Qmake executable.  Use this to set the version of Qt that you use."
      NO_DEFAULT_PATH
      )
    _qt4_query_qmake(QT_VERSION QTVERSION)
    if (${QTVERSION} VERSION_LESS "4.8")
      unset(QT_QMAKE_EXECUTABLE CACHE)
    else()
      break()
    endif()
  endforeach()
  # if all else fails, fall back to default search paths
  if(NOT QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE NAMES qmake-qt4 qmake
      DOC "Qmake executable.  Use this to set the version of Qt that you use."
      ${TOOL_SYSTEM_PATH}
      )
  endif(NOT QT_QMAKE_EXECUTABLE)
endif(NOT QT_QMAKE_EXECUTABLE)

find_package(OpenGL)
find_package(GLUT)
find_package(Qt4 4.8)

#
# Helper Macro for building Qt files
#
macro (QT4_GET_MOC_INC_DIRS _moc_INC_DIRS)
  set(${_moc_INC_DIRS})
  GET_DIRECTORY_PROPERTY(_inc_DIRS INCLUDE_DIRECTORIES)
  
  foreach(_current ${_inc_DIRS})
    set(${_moc_INC_DIRS} ${${_moc_INC_DIRS}} "-I" ${_current})
  endforeach(_current ${_inc_DIRS})
ENDMACRO(QT4_GET_MOC_INC_DIRS)

#
# Helper macro that runs moc on qt interface files
#
macro(QT4_I3_AUTOMOC)
  qt4_get_moc_inc_dirs(_moc_INCS)

  set(_matching_FILES )
  foreach (_current_FILE ${ARGN})
    
    get_filename_component(_abs_FILE ${_current_FILE} ABSOLUTE)
    # if "SKIP_AUTOMOC" is set to true, we will not handle this file here.
    # here. this is required to make bouic work correctly:
    # we need to add generated .cpp files to the sources (to compile them),
    # but we cannot let automoc handle them, as the .cpp files don't exist yet when
    # cmake is run for the very first time on them -> however the .cpp files might
    # exist at a later run. at that time we need to skip them, so that we don't add two
    # different rules for the same moc file
    get_source_file_property(_skip ${_abs_FILE} SKIP_AUTOMOC)
    
    if (NOT _skip AND EXISTS ${_abs_FILE})
      
      file(READ ${_abs_FILE} _contents)
      
      get_filename_component(_abs_PATH ${_abs_FILE} PATH)
      
      string(REGEX MATCHALL "#include +[^ ]+\\.moc[\">]" _match "${_contents}")
      if(_match)
        foreach (_current_MOC_INC ${_match})
          string(REGEX MATCH "[^ <\"]+\\.moc" _current_MOC "${_current_MOC_INC}")
          
          get_filename_component(_basename ${_current_MOC} NAME_WE)

          # Since the common default header path is in public/${PROJECT_NAME}, check there first
          if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/public/${PROJECT_NAME}/${_basename}.h)
            set(_header ${CMAKE_CURRENT_SOURCE_DIR}/public/${PROJECT_NAME}/${_basename}.h)
          # Otherwise check the default private location
          elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/private/${PROJECT_NAME}/${_basename}.h)
            set(_header ${CMAKE_CURRENT_SOURCE_DIR}/private/${PROJECT_NAME}/${_basename}.h)
          # Otherwise check the same directory as the source file
          elseif(EXISTS ${_abs_PATH}/${_basename}.h )
            set(_header ${_abs_PATH}/${_basename}.h )
          else()
            message( FATAL_ERROR "Cannot find header for automoc: ${_basename}.h" )
          endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/public/${PROJECT_NAME}/${_basename}.h)
          
          set(_moc    ${CMAKE_CURRENT_BINARY_DIR}/${_current_MOC})
          add_custom_command(OUTPUT ${_moc}
            COMMAND ${QT_MOC_EXECUTABLE}
            ARGS ${_moc_INCS} -DQT_NO_KEYWORDS ${_header} -o ${_moc}
            DEPENDS ${_header}
            )
          
          macro_add_file_dependencies(${_abs_FILE} ${_moc})
        endforeach (_current_MOC_INC)
      endif(_match)
    endif ( NOT _skip AND EXISTS ${_abs_FILE} )
  endforeach (_current_FILE)
endmacro(QT4_I3_AUTOMOC)
