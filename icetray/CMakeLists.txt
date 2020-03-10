#
#  Copyright (C) 2007-2009
#  Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#
#

# Check for libarchive

if(LIBARCHIVE_FOUND)
	add_definitions(-DI3_WITH_LIBARCHIVE)
	include_directories(${LIBARCHIVE_INCLUDE_DIR})
else(LIBARCHIVE_FOUND)
	colormsg(CYAN "+-- libarchive *not* found, omitting optional tarfile support")
	set(LIBARCHIVE_LIBRARIES "")
endif(LIBARCHIVE_FOUND)

if(ZSTD_FOUND)
	add_definitions(-DI3_WITH_ZSTD)
	include_directories(${ZSTD_INCLUDE_DIR})
else(ZSTD_FOUND)
	colormsg(CYAN "+-- zstd *not* found.")
	set(ZSTD_LIBRARIES "")
endif(ZSTD_FOUND)

i3_project(icetray
  DOCS_DIR resources/docs
  PYTHON_DIR python)

set(ICETRAY_PYTHON_SRC
  private/pybindings/module.cxx
  private/pybindings/OMKey.cxx
  private/pybindings/I3Bool.cxx
  private/pybindings/I3Int.cxx
  private/pybindings/I3PhysicsUsage.cxx
  private/pybindings/I3RUsage.cxx
  private/pybindings/I3Tray.cxx
  private/pybindings/I3Units.cxx
  private/pybindings/I3Frame.cxx
  private/pybindings/I3Logging.cxx
  private/pybindings/I3Module.cxx
  private/pybindings/I3Context.cxx
  private/pybindings/I3Configuration.cxx
  private/pybindings/I3ModuleFactory.cxx
  private/pybindings/I3FrameObject.cxx
  private/pybindings/I3FrameMixing.cxx
  private/pybindings/I3TrayInfo.cxx
  private/pybindings/MyService.cxx
  private/pybindings/std_cont_pod/std_cont_pod_char.cxx
  private/pybindings/std_cont_pod/std_cont_pod_double.cxx
  private/pybindings/std_cont_pod/std_cont_pod_I3Frame_Stream.cxx
  private/pybindings/std_cont_pod/std_cont_pod_int.cxx
  private/pybindings/std_cont_pod/std_cont_pod_map_int_int.cxx
  private/pybindings/std_cont_pod/std_cont_pod_map_omkey_int.cxx
  private/pybindings/std_cont_pod/std_cont_pod_omkey.cxx
  private/pybindings/std_cont_pod/std_cont_pod_pairs.cxx
  private/pybindings/std_cont_pod/std_cont_pod_sort.cxx
  private/pybindings/std_cont_pod/std_cont_pod_string.cxx
  private/pybindings/std_cont_pod/std_cont_pod_ulong.cxx
  private/pybindings/std_cont_pod/std_cont_pod_unsigned.cxx
  private/pybindings/std_cont_pod/std_cont_pod_vector_int.cxx
  private/pybindings/std_cont_pod/std_cont_pod_vector_string.cxx
  )

if(ZSTD_FOUND)
  LIST(APPEND OPTIONAL_TOOLS zstd)
endif(ZSTD_FOUND)

i3_add_library(icetray
  private/icetray/I3Tray.cxx
  private/icetray/I3Frame.cxx
  private/icetray/I3FrameObject.cxx
  private/icetray/I3FrameMixing.cxx
  private/icetray/I3Configuration.cxx
  private/icetray/I3ConfigurationImpl.cxx
  private/icetray/I3Context.cxx
  private/icetray/I3Parameter.cxx
  private/icetray/I3Module.cxx
  private/icetray/I3ConditionalModule.cxx
  private/icetray/I3PacketModule.cxx  
  private/icetray/I3ServiceFactory.cxx
  private/icetray/I3TrayInfo.cxx
  private/icetray/I3TrayInfoService.cxx
  private/icetray/I3IcePick.cxx
  private/icetray/I3PrintfLogger.cxx
  private/icetray/I3SyslogLogger.cxx
  private/icetray/I3FileLogger.cxx
  private/icetray/I3Logging.cxx
  private/icetray/PythonFunction.cxx
  private/icetray/FunctionModule.cxx
  private/icetray/PythonModule.cxx
  private/icetray/OMKey.cxx
  private/icetray/I3Int.cxx
  private/icetray/I3Bool.cxx
  private/icetray/I3PhysicsTimer.cxx
  private/icetray/I3PhysicsUsage.cxx
  private/icetray/Utility.cxx
  private/icetray/init.cxx
  private/icetray/crc-ccitt.c
  private/icetray/crc32.c
  private/icetray/load_project.cxx
  private/icetray/name_of.cxx
  private/icetray/memory.cxx
  private/icetray/scratch.cxx
  private/open/open.cxx
  private/open/http_source.cpp

  #
  #  Modules:  testing and example
  #
  private/modules/AddNulls.cxx
  private/modules/PhysicsBuffer.cxx
  private/modules/BottomlessSource.cxx
  private/modules/ManyStreamsSource.cxx
  private/modules/Rename.cxx
  private/modules/CountFrames.cxx
  private/modules/CountObject.cxx
  private/modules/TrashCan.cxx
  private/modules/Delete.cxx
  private/modules/Copy.cxx
  private/modules/Dump.cxx
  private/modules/Fork.cxx
  private/modules/IntGenerator.cxx
  private/modules/Keep.cxx
  private/modules/Get.cxx
  private/modules/FrameCheck.cxx
  private/modules/AllParametersModule.cxx
  private/modules/ContextFreeServiceTestModule.cxx
  private/modules/MaintainInitialValuesModule.cxx
  private/modules/CreateBlobs.cxx

  ${ICETRAY_PYTHON_SRC}

  IWYU
  USE_PROJECTS serialization
  USE_TOOLS boost python ${OPTIONAL_TOOLS}
  LINK_LIBRARIES ${BOOST_PYTHON} ${LIBARCHIVE_LIBRARIES}
  )
#
#  Configure/create directory containing inneresting workspace-wide #defines
#
configure_file(resources/workspace_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/workspace_config.h)

set_source_files_properties(private/icetray/I3TrayInfoService.cxx
  PROPERTIES
  COMPILE_FLAGS "-include ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/workspace_config.h")

#
# Configure memory tracking
#
string( TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER )
if ((${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug") AND (NOT DEFINED MEMORY_TRACKING))
  set(MEMORY_TRACKING ON)
endif ((${CMAKE_BUILD_TYPE_LOWER} STREQUAL "debug") AND (NOT DEFINED MEMORY_TRACKING))
if (${MEMORY_TRACKING})
  set_source_files_properties(
    private/icetray/memory.cxx
    private/icetray/I3Tray.cxx
    private/icetray/I3Module.cxx
    PROPERTIES
    COMPILE_FLAGS "-DMEMORY_TRACKING")
  colormsg(GREEN "+-- memory tracking enabled")
else (${MEMORY_TRACKING})
  colormsg(CYAN "+-- memory tracking disabled")
endif (${MEMORY_TRACKING})

set_source_files_properties(${ICETRAY_PYTHON_SRC}
  PROPERTIES
  COMPILE_FLAGS -DI3_PYBINDINGS_MODULE)

i3_test_executable(test
  private/test/I3BoolTest.cxx
  private/test/I3IntTest.cxx
  private/test/CleanConstructorTest.cxx
  private/test/I3LoggingObjectFirst.cxx
  private/test/ServicesAtDestruction.cxx
  private/test/ConfigureServiceTest.cxx
  private/test/I3LoggingTest.cxx
  private/test/SourceOrderTest.cxx
  private/test/I3LoggingTest2.cxx
  private/test/TestModule.cxx
  private/test/DefaultFrameNameTest.cxx
  private/test/TestServiceFactory.cxx
  private/test/InstallService.cxx
  private/test/EvensAndOdds.cxx
  private/test/I3PhysicsTimerTest.cxx
  private/test/TestServiceFactoryModule.cxx
  private/test/FailingModuleTest.cxx
  private/test/I3TrayTest.cxx
  private/test/Utility.cxx
  private/test/LexicalCasts.cxx
  private/test/FrameAccess.cxx
  private/test/VectorParameters.cxx
  private/test/FrameFlow.cxx
  private/test/ModuleGetService.cxx
  private/test/ModuleParameterConversions.cxx
  private/test/main.cxx
  private/test/I3ContextTest.cxx
  private/test/NonUniqueNameTest.cxx
  private/test/shared-ptr-constness.cxx
  private/test/I3FrameTest.cxx
  private/test/I3FrameMixing.cxx
  private/test/test-throws-not-caught.cxx
  private/test/PhysicsBuffering.cxx
  private/test/typesizes.cxx
  private/test/I3ConditionalModuleTest.cxx
  private/test/iostreams.cxx
  private/test/I3ModulePlumbing.cxx
  private/test/CompressedIO.cxx
  private/test/I3FileLogging.cxx
  private/test/ScratchDir.cxx
  private/test/I3Parameter.cxx

  USE_PROJECTS icetray)

#i3_test_compile(main private/test/main.cxx)

i3_test_scripts(resources/test/*.py)

#
#  Special tests: each header file compiles on its own
#
no_dotfile_glob(ALL_HEADERS public/icetray/*.h)
# ALL_HEADERS will be absolute paths
foreach(HEADER ${ALL_HEADERS})
  get_filename_component(basename ${HEADER} NAME_WE)
  file(RELATIVE_PATH REL_HEADER ${CMAKE_CURRENT_SOURCE_DIR} ${HEADER})
  #i3_test_compile(${basename} ${REL_HEADER})
endforeach(HEADER ${ALL_HEADERS})

#
# Python bindings
#
set(TMP_LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH})

i3_add_pybindings(icetray
  private/pybindings/module.cxx
  USE_TOOLS boost python
  LINK_LIBRARIES ${LIBARCHIVE_LIBRARIES}
  USE_PROJECTS icetray
  )

#
# This is special, goes in the toplevel 'lib' directory for
# backwards compatibility
#
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/private/pybindings/I3Tray.py
  DESTINATION lib)

add_custom_command(OUTPUT ${TMP_LIBRARY_OUTPUT_PATH}/I3Tray.py
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/private/pybindings/I3Tray.py
  COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/private/pybindings/I3Tray.py ${TMP_LIBRARY_OUTPUT_PATH}
  )

add_custom_target(I3Tray.py
  DEPENDS ${TMP_LIBRARY_OUTPUT_PATH}/I3Tray.py
  )

add_dependencies(icetray-pybindings I3Tray.py)

# Install icetray-inspect
i3_executable_script(inspect resources/icetray-inspect.py)
