if(POLICY CMP0002)
  cmake_policy(SET CMP0002 NEW)
endif()
if(POLICY CMP0053)
  cmake_policy(SET CMP0053 OLD)
endif()

set(CMAKE_AUTOMOC True)

# look for homebrew, and its qt5 paths
if(BREW)
  execute_process(COMMAND ${BREW} --prefix qt5 OUTPUT_VARIABLE QT5_BREW_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
  find_package(Qt5 COMPONENTS OpenGL
    HINTS ${QT5_BREW_PATH}
    PATH /usr/local/Cellar/qt5/5.7.0)
endif()

# non-homebrew search
find_package(Qt5 COMPONENTS OpenGL)
find_package(OpenGL)
find_package(GLUT)

if(Qt5_FOUND AND Qt5OpenGL_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
  # Use the version of Steamshovel itself, rather than the last commit to the
  # entire IceCube svn repository
  set(OLD_LANG ENV{LANG})
  set(ENV{LANG} "en_US.UTF-8")
  execute_process(COMMAND ${SVN_EXECUTABLE} info ${CMAKE_SOURCE_DIR}/steamshovel
    COMMAND awk "/^Last Changed Rev:/ { printf $4 }"
    OUTPUT_VARIABLE SHOVEL_REVISION
    ERROR_VARIABLE SHOVEL_REVISION_ERROR)
  set(ENV{LANG} OLD_LANG)

  if(SHOVEL_REVISION_ERROR OR NOT SHOVEL_REVISION)
    set(SHOVEL_REVISION 0)
  endif()

  # prevent Q_FOREACH from stepping on boost::foreach
  add_definitions(-DQT_NO_KEYWORDS -DSHOVEL_REVISION=${SHOVEL_REVISION})

  # extra hints for qt5 and automoc
  include_directories(BEFORE ${Qt5OpenGL_INCLUDE_DIRS})
  include_directories(BEFORE /usr/local/opt/qt5/include) ## necessary if qt4 and qt5 are installed via homebrew (even if qt4 is unlinked!)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  qt5_wrap_ui(steamshovel_UIS_H
    private/ui/config_dialog.ui
    private/ui/shovelmainwindow.ui
    private/ui/timeline.ui
    private/ui/scenario.ui
    private/ui/fullscreen_message.ui
    private/ui/tvmode_message.ui
    private/ui/screenshot.ui
    private/ui/movie.ui
    private/ui/movie_progress.ui
    private/ui/update_dialog.ui
    )

  qt5_add_resources(RCC_SRCS
    resources/steamshovel.qrc
    )
  
  ## duplicate part of qt5_add_resources(), and remove the dir it creates
  ## if packaging breaks, try deleting this block
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources/steamshovel.qrc"
          "${CMAKE_CURRENT_SOURCE_DIR}/resources/steamshovel.qrc.depends"
          COPYONLY)
  file(REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/resources")

  set(CONFIG_SRCS
    "python/artists/mplart/QtMPLArtist.py"
    "python/embed/console.py"
    "python/embed/console_widget.py"
    "python/util/help.py"
    "resources/test/test_shovelscripts.py")

  set(_pyqtver "5")
  foreach(_f IN LISTS CONFIG_SRCS)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${_f}.in"
      "${CMAKE_CURRENT_SOURCE_DIR}/${_f}"
      @ONLY)
  endforeach()

  i3_add_library( shovelart
    private/shovelart/artists/*.cpp
    private/shovelart/*.cpp
    private/shovelart/pybindings/Settings.cpp
    private/scripting/pycompat.cpp
    private/scripting/pyerr.cpp
    private/scripting/qpixmap_from_python.cpp
    USE_TOOLS boost python opengl
    USE_PROJECTS icetray dataclasses
    LINK_LIBRARIES ${BOOST_PYTHON} Qt5::OpenGL
    )

  i3_add_pybindings( shovelart
    private/scripting/*.cpp
    private/shovelart/pybindings/*.cpp
    USE_TOOLS boost python opengl
    USE_PROJECTS icetray dataclasses
    LINK_LIBRARIES ${BOOST_PYTHON} Qt5::OpenGL
    )

  ## the following files trigger warnings in expanded macros
  set_source_files_properties(private/shovelart/pybindings/Settings.cpp
                              PROPERTIES COMPILE_FLAGS "-Wno-parentheses-equality")
  set_source_files_properties(private/shovelart/pybindings/Scenario.cpp
                              private/shovelart/pybindings/PyArtist.cpp
                              PROPERTIES COMPILE_FLAGS "-Wno-unused-function")

  target_link_libraries( shovelart-pybindings shovelart )

  i3_add_library( shovelio
    private/shovelio/*.cpp
    USE_TOOLS boost python
    USE_PROJECTS serialization icetray dataclasses
    LINK_LIBRARIES ${BOOST_PYTHON} ${BOOST_THREAD}
    )

  i3_add_pybindings( shovelio
    private/shovelio/pybindings/*.cpp
    USE_TOOLS boost python
    USE_PROJECTS icetray dataclasses
    LINK_LIBRARIES ${BOOST_PYTHON}
    )

  target_link_libraries( shovelio-pybindings shovelio )

  i3_executable( logviewer
    private/logviewer/*.cpp
    USE_PROJECTS icetray
    LINK_LIBRARIES Qt5::OpenGL
    )

  i3_executable( steamshovel
    private/steamshovel/*.cpp
    private/scripting/*.cpp
    ${steamshovel_UIS_H}
    ${RCC_SRCS}
    USE_TOOLS boost python opengl
    USE_PROJECTS icetray dataclasses dataio
    LINK_LIBRARIES ${BOOST_PYTHON} Qt5::OpenGL
    NO_PREFIX
    )
  target_link_libraries( steamshovel shovelart shovelio )
  add_dependencies( steamshovel steamshovel-logviewer shovelart shovelio
                    shovelart-pybindings shovelio-pybindings
                    dataio-pybindings dataclasses-pybindings icetray-pybindings )

  # tests
  i3_test_scripts(resources/test/*.py)


  if(APPLE)

    # Give steamshovel an extra link dependency on QtSvg, which is used by PyQt4;
    # this ensures that QtSvg will wind up in Steamshovel.app.
    find_package(Qt5 COMPONENTS Svg
      HINTS ${Qt5Core_INCLUDE_DIRS})
    target_link_libraries( steamshovel Qt5::Svg )

    # Ensure install_name_tool and macdeployqt don't cause problems when rewriting shared lib paths
    SET_TARGET_PROPERTIES( steamshovel
        PROPERTIES
        LINK_FLAGS "-headerpad_max_install_names"
    )

    add_custom_target(mac-steamshovel-app
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/resources/Steamshovelapp/macdeployapp.py ${CMAKE_BINARY_DIR} ${SHOVEL_REVISION}
    )
    add_dependencies(mac-steamshovel-app steamshovel html)

    set( STEAMSHOVEL_DMG ${CMAKE_BINARY_DIR}/Steamshovel.r${SHOVEL_REVISION}.dmg )
    add_custom_target(mac-steamshovel-dmg
        COMMAND rm -f ${STEAMSHOVEL_DMG}
        COMMAND hdiutil create -volname Steamshovel -srcfolder ${CMAKE_BINARY_DIR}/Steamshovel.app -ov -format UDBZ ${STEAMSHOVEL_DMG}
    )
    add_dependencies(mac-steamshovel-dmg mac-steamshovel-app)

  endif(APPLE)

else()

  if(NOT OPENGL_FOUND)
    colormsg(CYAN "+-- Opengl not found.")
  endif()
  if(NOT OPENGL_GLU_FOUND)
    colormsg(CYAN "+-- Opengl GLU not found.")
  endif()
  if(NOT Qt5_FOUND)
    colormsg(CYAN "+-- Qt5 Not found.")
  endif()
  if(QT5_CONFIG_ERROR)
    colormsg(CYAN "+-- Qt5 Configuration error.")
  endif()
  if(NOT QT_QTOPENGL_FOUND)
    colormsg(CYAN "+-- Qt5 did not find opengl.")
  endif()
  colormsg(CYAN "+-- Disabling build of ${PROJECT_NAME}.")

  add_custom_target(steamshovel
    COMMENT "\n>>>\n>>> ${PROJECT_NAME} build skipped becase Qt5 was not found
or there was a config error\n>>>"
    )
endif()
  
