#
#  $Id$
#  
#  Copyright (C) 2007
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
  private/pybindings/I3Frame.cxx
  private/pybindings/I3Module.cxx
  private/pybindings/I3Context.cxx
  private/pybindings/I3FrameObject.cxx
  private/pybindings/I3TrayInfo.cxx
  private/pybindings/std_cont_pod.cxx)

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

  ${ICETRAY_PYTHON_SRC}

  USE_TOOLS boost python log4cplus root
  LINK_LIBRARIES ${BOOST_PYTHON}
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

i3_executable(inspect 
  private/inspect/*.cxx
  USE_PROJECTS icetray
  USE_TOOLS log4cplus boost root)

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

  USE_TOOLS root
  USE_PROJECTS icetray)

i3_test_compile(main private/test/main.cxx)

i3_test_scripts(resources/scripts/*.py)

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

