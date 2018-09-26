# Note: We use globbing here to automatically add new files to the build.
# You still need to "touch" the CMakeLists.txt to re-generate the Makefile.

i3_project(steamshovel
  PYTHON_DIR python
  DOCS_DIR resources/docs
  )

option(USE_QT4 "Use Qt4/PyQt4 instead of the default of Qt5/PyQt5" OFF)

if (NOT USE_QT4)
  include(${CMAKE_CURRENT_SOURCE_DIR}/config_with_qt5.cmake)
  return()
endif()

colormsg(HIYELLOW "*** You've opted to use Qt4. Qt4 has reached its end-of-life,")
colormsg(HIYELLOW "*** and its use in steamshovel is deprecated. Please the default Qt5 instead.")

if ((PYTHON_NUMERIC_VERSION GREATER 20702) AND (BOOST_PORTS_VERSION MATCHES "1.38.0"))
    colormsg (YELLOW "*** You are using boost<=1.38 and Python>=2.7.3.  Steamshovel will do its best to avert crashes and catastrophe, but no promises.")
endif ((PYTHON_NUMERIC_VERSION GREATER 20702) AND (BOOST_PORTS_VERSION MATCHES "1.38.0"))
if (PYTHON_NUMERIC_VERSION LESS 20600)
    colormsg (YELLOW "*** python v2.6 or greater is required... skipping")
else (PYTHON_NUMERIC_VERSION LESS 20600)


if(QT4_FOUND AND NOT QT_CONFIG_ERROR AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND)
  # Use the version of Steamshovel itself, rather than the last commit to the
  # entire IceCube svn repository
  set(OLD_LANG ENV{LANG})
  set(ENV{LANG} "en_US.UTF-8")
  execute_process(COMMAND ${SVN_EXECUTABLE} info ${CMAKE_SOURCE_DIR}/steamshovel
    COMMAND awk "/^Last Changed Rev:/ { printf $4 }"
    OUTPUT_VARIABLE SHOVEL_REVISION
    ERROR_VARIABLE SHOVEL_REVISION_ERROR)
  set(ENV{LANG} OLD_LANG)

  if(SHOVEL_REVISION_ERROR)
    set(SHOVEL_REVISION 0)
  endif()

  # prevent Q_FOREACH from stepping on boost::foreach
  add_definitions(-DQT_NO_KEYWORDS -DSHOVEL_REVISION=${SHOVEL_REVISION})

  include(${QT_USE_FILE})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/private)

  qt4_i3_automoc(
    private/steamshovel/ShovelMainWindow.cpp
    private/steamshovel/SessionMenu.cpp
    private/steamshovel/I3GLWidget.cpp
    private/steamshovel/FramePicker.cpp
    private/steamshovel/FileService.cpp
    private/steamshovel/FilterWidget.cpp
    private/steamshovel/TimeSpinBox.cpp
    private/steamshovel/SteamshovelApp.cpp
    private/steamshovel/TimelineWidget.cpp
    private/steamshovel/I3FrameTableModel.cpp
    private/steamshovel/ScenarioModel.cpp
    private/steamshovel/ScenarioFrameModel.cpp
    private/steamshovel/ShellWidget.cpp
    private/steamshovel/ArtistDelegate.cpp
    private/steamshovel/ArtistSettings.cpp
    private/steamshovel/ScenarioWidget.cpp
    private/steamshovel/ProjectionDialog.cpp
    private/steamshovel/MovieDialog.cpp
    private/steamshovel/MovieEngine.cpp
    private/steamshovel/LogDialog.cpp
    private/steamshovel/SaveFrameDialog.cpp
    private/steamshovel/ConfigDialog.cpp
    private/steamshovel/ShovelSlider.cpp
    private/shovelart/BindSignal.cpp
    private/shovelart/Scenario.cpp
    private/shovelart/ArtistRegistry.cpp
    private/shovelart/artists/Pong.cpp
    private/shovelart/artists/IceTop.cpp
    private/scripting/qmeta_args.cpp
    )

  qt4_wrap_ui(steamshovel_UIS_H
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

  qt4_add_resources(RCC_SRCS
    resources/steamshovel.qrc
    )
  
  ## duplicate part of qt4_add_resources(), and remove the dir it creates
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

  set(_pyqtver "4")
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
    USE_TOOLS boost python opengl qt4
    USE_PROJECTS icetray dataclasses
    LINK_LIBRARIES ${BOOST_PYTHON} ${QT_LIBRARIES} ${OPENGL_LIBRARIES}
    )

  i3_add_pybindings( shovelart
    private/scripting/*.cpp
    private/shovelart/pybindings/*.cpp
    USE_TOOLS boost python opengl qt4
    USE_PROJECTS icetray dataclasses
    LINK_LIBRARIES ${BOOST_PYTHON} ${QT_LIBRARIES} ${OPENGL_LIBRARIES}
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

  # logviewer provides access to error messages on MacOS
  qt4_i3_automoc(
    private/logviewer/LogViewer.cpp
    )

  i3_executable( logviewer
    private/logviewer/*.cpp
    USE_TOOLS qt4
    USE_PROJECTS icetray
    LINK_LIBRARIES ${QT_LIBRARIES}
    )

  i3_executable( steamshovel
    private/steamshovel/*.cpp
    private/scripting/*.cpp
    ${steamshovel_UIS_H}
    ${RCC_SRCS}
    USE_TOOLS boost python opengl qt4
    USE_PROJECTS icetray dataclasses dataio
    LINK_LIBRARIES ${BOOST_PYTHON} ${QT_LIBRARIES} ${OPENGL_LIBRARIES}
    NO_PREFIX
    )
  target_link_libraries( steamshovel shovelart shovelio )
  add_dependencies( steamshovel steamshovel-logviewer shovelart shovelio
                    shovelart-pybindings shovelio-pybindings
                    dataio-pybindings dataclasses-pybindings icetray-pybindings )

  # tests
  i3_test_scripts(resources/test/*.py)


  if(APPLE)
    add_definitions( -DQT_WS_MAC=1 )

    # Give steamshovel an extra link dependency on QtSvg, which is used by PyQt4;
    # this ensures that QtSvg will wind up in Steamshovel.app.
    target_link_libraries( steamshovel ${QT_QTSVG_LIBRARY} )

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

else(QT4_FOUND AND NOT QT_CONFIG_ERROR AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND)

  if(NOT OPENGL_FOUND)
    colormsg(CYAN "+-- Opengl not found.")
  endif()
  if(NOT OPENGL_GLU_FOUND)
    colormsg(CYAN "+-- Opengl GLU not found.")
  endif()
  if(NOT QT4_FOUND)
    colormsg(CYAN "+-- Qt4 Not found.")
  endif()
  if(QT4_CONFIG_ERROR)
    colormsg(CYAN "+-- Qt4 Configuration error.")
  endif()
  if(NOT QT_QTOPENGL_FOUND)
    colormsg(CYAN "+-- Qt4 did not find opengl.")
  endif()
  colormsg(CYAN "+-- Disabling build of ${PROJECT_NAME}.")

  add_custom_target(steamshovel
    COMMENT "\n>>>\n>>> ${PROJECT_NAME} build skipped becase Qt4 was not found
or there was a config error\n>>>"
    )
endif(QT4_FOUND AND NOT QT_CONFIG_ERROR AND OPENGL_FOUND AND OPENGL_GLU_FOUND AND QT_QTOPENGL_FOUND)

endif(PYTHON_NUMERIC_VERSION LESS 20600)

