# -*- mode: cmake; cmake-tab-width: 4 -*- vim: ft=cmake

# This cmake file contains the "setup and configuration" for testing
# and data. Primarily it sets two cmake variables from provided
# environment variables, and defines targets to rsync testing and
# production data.

# It is best to set I3_TESTDATA and I3_PRODDATA as environment
# variables in the "cmake step" when building your workspace. Using a
# CMake flag may not do what you expect.

# CMake's `enable_test()` MUST NOT BE CALLED HERE. It must be called
# in the top level CMakeLists (ie our symlinked topleve.cmake) in
# order for cmake to properly setup testing. If you call
# `enable_testing()` from here, CMake will get confused and only setup
# testing under our 'cmake/' directory.

colormsg("")
colormsg(_HIBLUE_ "Configuring data and tests...")
colormsg("")

set(TESTDATA_VERSION "trunk")
set(PRODDATA_VERSION "trunk")

## set the value of I3_TESTDATA
if(DEFINED ENV{I3_TESTDATA})
    set(p $ENV{I3_TESTDATA})
    if(IS_ABSOLUTE ${p})
        set(I3_TESTDATA "${p}")
    else()
        get_filename_component(I3_TESTDATA "${CMAKE_BINARY_DIR}/${p}" ABSOLUTE)
    endif()
    set(I3_TESTDATA "${I3_TESTDATA}" CACHE PATH "Path to your icetray test-data" FORCE)
else()
    set(I3_TESTDATA "${CMAKE_BINARY_DIR}/test-data" CACHE PATH "Normalized path to your icetray test-data")
    colormsg(YELLOW "*** I3_TESTDATA was not set via an environment variable. Using the default value of ${I3_TESTDATA}")
endif()
boost_report_value(I3_TESTDATA)

## setup make/ninja target for rsync'ing test-data
set(TESTDATA_URL "code.icecube.wisc.edu::Offline/test-data/${TESTDATA_VERSION}/")
add_custom_target(rsync
    COMMAND test -n "${I3_TESTDATA}"
    COMMAND mkdir -p "${I3_TESTDATA}"
    COMMAND rsync -vrlpt --delete ${TESTDATA_URL} ${I3_TESTDATA}/
    COMMENT "Rsyncing test-data to ${I3_TESTDATA} (I3_TESTDATA)"
    )

## set the value of I3_PRODDATA
if(DEFINED ENV{I3_PRODDATA})
    set(p $ENV{I3_PRODDATA})
    if(IS_ABSOLUTE ${p})
        set(I3_PRODDATA "${p}")
    else()
        get_filename_component(I3_PRODDATA "${CMAKE_BINARY_DIR}/${p}" ABSOLUTE)
    endif()
    set(I3_PRODDATA "${I3_PRODDATA}" CACHE PATH "Path to your icetray test-data" FORCE)
else()
    set(I3_PRODDATA "${CMAKE_BINARY_DIR}/prod-data" CACHE PATH "Normalized path to your icetray production-data")
    colormsg(YELLOW "*** I3_PRODDATA was not set via an environment variable.")
    colormsg(YELLOW "***   Using the default value of ${I3_PRODDATA}")
endif()
boost_report_value(I3_PRODDATA)

## setup make/ninja target for rsync'ing production data
set(PRODDATA_URL "code.icecube.wisc.edu::Offline/prod-data/${PRODDATA_VERSION}/")
add_custom_target(rsync-prod-data
    COMMAND test -n "${I3_PRODDATA}"
    COMMAND mkdir -p "${I3_PRODDATA}"
    COMMAND rsync -vrlpt --delete ${PRODDATA_URL} ${I3_PRODDATA}/
    COMMENT "Rsyncing test-data to ${I3_PRODDATA} (I3_PRODDATA)"
    )

## DO NOT add `enable_testing()` here. See commentary above.
