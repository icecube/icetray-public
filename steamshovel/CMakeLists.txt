# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# Note: We use globbing here to automatically add new files to the build.
# You still need to "touch" the CMakeLists.txt to re-generate the Makefile.

i3_project(steamshovel
  PYTHON_DIR python
  DOCS_DIR resources/docs)

if(BUILD_STEAMSHOVEL)

  set(CMAKE_AUTOMOC True)

  # look for homebrew, and its qt5 paths
  if(BREW)
    execute_process(COMMAND ${BREW} --prefix qt@5 OUTPUT_VARIABLE QT5_BREW_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
    find_package(Qt5 COMPONENTS OpenGL
      HINTS ${QT5_BREW_PATH})
  endif()

  # non-homebrew search
  find_package(Qt5 COMPONENTS OpenGL)
  find_package(OpenGL QUIET)
  find_package(GLUT QUIET)

  if(Qt5_FOUND AND Qt5OpenGL_FOUND AND OPENGL_FOUND AND OPENGL_GLU_FOUND)
    ## use GIT_REVISION to derive SHOVEL_VERSION
    set(SHOVEL_VERSION "${GIT_REVISION}")

    if(SHOVEL_REVISION_ERROR OR NOT SHOVEL_REVISION)
      set(SHOVEL_REVISION 0)
    endif()

    # prevent Q_FOREACH from stepping on boost::foreach
    add_definitions(-DQT_NO_KEYWORDS -DSHOVEL_REVISION=${SHOVEL_REVISION})
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
      add_definitions(-DQT_DEPRECATED_WARNINGS)
      add_definitions(-Wdeprecated)
    endif()
    # prevent OpenGL deprecation warnings on macOS
    if(APPLE)
      add_definitions(-DGL_SILENCE_DEPRECATION)
    endif()

    # extra hints for qt5 and automoc
    include_directories(BEFORE SYSTEM ${Qt5OpenGL_INCLUDE_DIRS})
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

    configure_file("private/steamshovel/SteamShovelConf.h.in" "SteamShovelConf.h")

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
      NOUNDERSCORE
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
      NOUNDERSCORE
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
    # test_shovelscripts.py is currently disabled.
    # See ticket #2430: https://github.com/icecube/icetray/issues/2430
    # The plan is to get this back in shape before the fall 2020 release.
    i3_test_scripts(resources/test/test_shovelio_raw_File.py
      resources/test/test_shovelio_raw_FileGroup.py
      resources/test/test_shovelio_against_I3Reader.py
      resources/test/test_setting.py
      resources/test/test_shovelio_I3FrameSequence.py
      resources/test/test_shovelscripts.py
      python/util/colormaps.py
    )
    set_tests_properties(steamshovel::test_shovelscripts.py PROPERTIES
      DISABLED TRUE
      SKIP_REGULAR_EXPRESSION " \.\.\. skipped ")


    if(APPLE)

      # Give steamshovel an extra link dependency on QtSvg, which is used by PyQt4;
      # this ensures that QtSvg will wind up in Steamshovel.app.
      find_package(Qt5 COMPONENTS Svg
        HINTS ${Qt5Core_INCLUDE_DIRS})
      target_link_libraries( steamshovel Qt5::Svg )

      add_custom_target(mac-steamshovel-app
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/resources/Steamshovelapp/macdeployapp.py ${CMAKE_BINARY_DIR} ${SHOVEL_REVISION}
      )
      add_dependencies(mac-steamshovel-app steamshovel)

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
      COMMENT "\n>>>\n>>> ${PROJECT_NAME} build skipped because Qt5 was not found
or there was a config error\n>>>"
    )
  endif()

endif()

# always symlink the icetopviewer
execute_process(COMMAND ln -sf ${CMAKE_CURRENT_SOURCE_DIR}/resources/scripts/icetopviewer/icetopviewer.py ${CMAKE_BINARY_DIR}/bin/icetopviewer)
