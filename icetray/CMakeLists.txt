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

# 
#  Check for dcap
#  
find_library(DCAP_LIBRARIES dcap HINTS ${DCAP_INSTALL_PREFIX}/lib ${DCAP_INSTALL_PREFIX}/lib64)
find_file(FIND_DCAP_H dcap.h HINTS ${DCAP_INSTALL_PREFIX}/include)

get_filename_component(DCAP_INCLUDE_DIR ${FIND_DCAP_H} PATH)

if (DCAP_LIBRARIES AND DCAP_INCLUDE_DIR)
  colormsg(GREEN "+-- Found libdcap, enabling dcap support")
  add_definitions(-DI3_WITH_DCAP)
  include_directories(${DCAP_INCLUDE_DIR})
else()
  colormsg(CYAN "+-- libdcap *not* found, omitting optional dcap support")
  set(DCAP_LIBRARIES "")  
endif()  

# Check for libarchive

if(LIBARCHIVE_FOUND)
	add_definitions(-DI3_WITH_LIBARCHIVE)
	include_directories(${LIBARCHIVE_INCLUDE_DIR})
else(LIBARCHIVE_FOUND)
	colormsg(CYAN "+-- libarchive *not* found, omitting optional tarfile support")
	set(LIBARCHIVE_LIBRARIES "")
endif(LIBARCHIVE_FOUND)

# ROOT

if(USE_ROOT)
	set(OPTIONAL_TOOLS root)
endif(USE_ROOT)

if(USE_CINT)
    colormsg(HIYELLOW "*** CINT dictionaries are deprecated and will disappear November 1, 2015")
    set(ROOTCINT "ROOTCINT public/icetray/I3FrameObject.h")
endif(USE_CINT)

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
  private/pybindings/std_cont_pod/std_cont_pod_vector_string.cxx)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
  # On FreeBSD, the backtrace facility is in a separate library.
  set(I3_BACKTRACE_CXX private/icetray/Backtrace.cxx)
endif (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")

i3_add_library(icetray
  private/icetray/I3Frame.cxx 
  private/icetray/crc-ccitt.c
  private/icetray/crc32.c
  private/icetray/open.cxx
  private/icetray/load_project.cxx
  private/icetray/I3FrameObject.cxx      
  private/icetray/I3Logging.cxx        
  private/icetray/I3Configuration.cxx
  private/icetray/I3ConfigurationImpl.cxx
  private/icetray/I3Deprecation.cxx
  private/icetray/I3Parameter.cxx
  private/icetray/OMKey.cxx            
  private/icetray/I3Tray.cxx           
  private/icetray/I3Module.cxx         
  private/icetray/I3ConditionalModule.cxx  
  private/icetray/I3Context.cxx        
  private/icetray/I3DSORegistry.cxx      
  private/icetray/I3Int.cxx            
  private/icetray/I3Bool.cxx
  private/icetray/I3PhysicsTimer.cxx     
  private/icetray/I3PhysicsUsage.cxx     
  private/icetray/I3PrintfLogger.cxx
  private/icetray/I3SyslogLogger.cxx
  private/icetray/Utility.cxx
  private/icetray/I3ServiceFactory.cxx   
  private/icetray/I3TrayInfo.cxx               
  private/icetray/I3TrayInfoService.cxx  
  private/icetray/I3IcePick.cxx        
  private/icetray/I3PacketModule.cxx  
  private/icetray/serialization.cxx  
  ${I3_BACKTRACE_CXX}
  private/icetray/init.cxx
  private/icetray/PythonFunction.cxx
  private/icetray/FunctionModule.cxx
  private/icetray/PythonModule.cxx
  private/icetray/name_of.cxx

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
  private/modules/DeleteUnregistered.cxx               
  private/modules/Get.cxx              
  private/modules/FrameCheck.cxx               
  private/modules/AllParametersModule.cxx
  private/modules/ContextFreeServiceTestModule.cxx
  private/modules/MaintainInitialValuesModule.cxx

  ${ICETRAY_PYTHON_SRC}

  USE_TOOLS boost python ${OPTIONAL_TOOLS}
  LINK_LIBRARIES ${BOOST_PYTHON} ${DCAP_LIBRARIES} ${LIBARCHIVE_LIBRARIES}
  ${ROOTCINT})
#
#  Configure/create directory containing inneresting workspace-wide #defines
#
configure_file(resources/workspace_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/workspace_config.h)

set_source_files_properties(private/icetray/I3TrayInfoService.cxx
  PROPERTIES
  COMPILE_FLAGS "-include ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/workspace_config.h")

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
  private/test/test-throws-not-caught.cxx
  private/test/PhysicsBuffering.cxx         
  private/test/typesizes.cxx
  private/test/I3ConditionalModuleTest.cxx
  private/test/NoOutboxModule.cxx
  private/test/iostreams.cxx
  private/test/I3ModulePlumbing.cxx
 
  USE_PROJECTS icetray)

#i3_test_compile(main private/test/main.cxx)

# Reunify these directories once python < 2.6 support dropped
if (PYTHON_NUMERIC_VERSION LESS 20600)
	i3_test_scripts(resources/test/*.py)
else (PYTHON_NUMERIC_VERSION LESS 20600)
	i3_test_scripts(resources/test/*.py resources/py26-tests/*.py)
endif (PYTHON_NUMERIC_VERSION LESS 20600)

#add_subdirectory(private/test-lib)

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
# Python bindings are in there
#
add_subdirectory(private/pybindings)
add_subdirectory(private/test-pybindings)

# Install icetray-inspect
i3_executable_script(inspect resources/icetray-inspect.py)

