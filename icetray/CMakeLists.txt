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

option(USE_I3_FRAMEWORK_LOGGING 
  "Turn on low level icetray logging (for debugging icetray itself)" 
  OFF)

if(USE_I3_FRAMEWORK_LOGGING)
  add_definitions(-DI3_FRAMEWORK_LOGGING)
endif()

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
  private/pybindings/I3Module.cxx
  private/pybindings/I3Context.cxx
  private/pybindings/I3Configuration.cxx
  private/pybindings/I3ModuleFactory.cxx
  private/pybindings/I3FrameObject.cxx
  private/pybindings/I3TrayInfo.cxx
  private/pybindings/std_cont_pod.cxx
  private/pybindings/MyService.cxx)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
  # On FreeBSD, the backtrace facility is in a separate library.
  set(I3_BACKTRACE_CXX private/icetray/Backtrace.cxx)
endif (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")

i3_add_library(icetray
  private/icetray/I3Frame.cxx 
  private/icetray/open.cxx
  private/icetray/load_project.cxx
  private/icetray/I3FrameObject.cxx      
  private/icetray/portable_binary_iarchive.cxx
  private/icetray/portable_binary_oarchive.cxx
  private/icetray/I3Logging.cxx        
  private/icetray/PythonLogging.cxx            
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
  private/icetray/Utility.cxx
  private/icetray/I3ServiceFactory.cxx   
  private/icetray/I3TrayInfo.cxx               
  private/icetray/I3TrayInfoService.cxx  
  private/icetray/I3IcePick.cxx        
  private/icetray/I3PacketModule.cxx  
  ${I3_BACKTRACE_CXX}
  private/icetray/PythonInterpreter.cxx        
  private/icetray/PythonFunction.cxx           
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

  USE_TOOLS boost python log4cplus root
  LINK_LIBRARIES ${BOOST_PYTHON} ${DCAP_LIBRARIES} ${LIBARCHIVE_LIBRARIES}
  ROOTCINT public/icetray/I3FrameObject.h)

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
  private/test/CleanConstructorTest.cxx  
  private/test/I3LoggingObjectFirst.cxx     
  private/test/ServicesAtDestruction.cxx
  private/test/ConfigureServiceTest.cxx  
  private/test/I3LoggingTest.cxx                    
  private/test/SourceOrderTest.cxx
  private/test/ContextSwitching.cxx      
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
  private/test/OutOfMemoryModuleTest.cxx            
  private/test/test-throws-not-caught.cxx
  private/test/PhysicsBuffering.cxx         
  private/test/typesizes.cxx
  private/test/OMKey.cxx
  private/test/I3ConditionalModuleTest.cxx
  private/test/NoOutboxModule.cxx
  private/test/iostreams.cxx
 
  USE_PROJECTS icetray)

i3_test_compile(main private/test/main.cxx)

i3_test_scripts(resources/test/*.py)

add_subdirectory(private/compile-fail)
add_subdirectory(private/compile)

#add_subdirectory(private/test-lib)

#
#  Special tests: each header file compiles on its own
#
no_dotfile_glob(ALL_HEADERS public/icetray/*.h)
# ALL_HEADERS will be absolute paths
foreach(HEADER ${ALL_HEADERS})
  get_filename_component(basename ${HEADER} NAME_WE)
  file(RELATIVE_PATH REL_HEADER ${CMAKE_CURRENT_SOURCE_DIR} ${HEADER})
  i3_test_compile(${basename} ${REL_HEADER})
endforeach(HEADER ${ALL_HEADERS})

#
# Python bindings are in there
#
add_subdirectory(private/pybindings)
add_subdirectory(private/test-pybindings)

# Install icetray-inspect
exec_program(ln ARGS -snf ${CMAKE_SOURCE_DIR}/icetray/resources/icetray-inspect.py ${EXECUTABLE_OUTPUT_PATH}/icetray-inspect
    OUTPUT_VARIABLE DEV_NULL)

