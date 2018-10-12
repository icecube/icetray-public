#
# $Id$
#
i3_project(phys-services
  PYTHON_DIR python
  PYTHON_DEST icecube/phys_services
  DOCS_DIR resources/docs)

if (SPRNG_FOUND)
  colormsg(GREEN "+-- sprng found, adding SPRNGRandomService")
  set(SPRNG_SOURCES
    private/phys-services/I3SPRNGRandomService.cxx
    private/phys-services/I3SPRNGRandomServiceFactory.cxx)
  set(OPTIONAL_TOOLS ${OPTIONAL_TOOLS} sprng)
  ADD_DEFINITIONS(-DI3_USE_SPRNG)
else(SPRNG_FOUND)
  colormsg(CYAN "+-- sprng *not* found, omitting optional SPRNGRandomService")
endif(SPRNG_FOUND)

i3_add_library(phys-services 
  private/converter/I3EventInfoConverter.cxx
  private/converter/I3RecoInfoConverter.cxx
  private/converter/I3CutValuesConverter.cxx
  private/geo-selector/GeoSelUtils.cxx
  private/geo-selector/I3GeoTrimmers.cxx
  private/geo-selector/I3GeometrySelectorService.cxx
  private/geo-selector/I3GeometrySelectorServiceFactory.cxx
  private/geo-selector/I3StringAdderService.cxx
  private/geo-selector/I3StringAdderServiceFactory.cxx
  private/phys-services/I3BadDOMAuditor.cxx
  private/phys-services/I3Calculator.cxx
  private/phys-services/I3CascadeCutValues.cxx
  private/phys-services/I3Cuts.cxx
  private/phys-services/I3CrossSection.cxx
  private/phys-services/I3CutsModule.cxx
  private/phys-services/I3EventCounter.cxx
  private/phys-services/I3FileOMKey2MBID.cxx
  private/phys-services/I3FileOMKey2MBIDFactory.cxx
  private/phys-services/I3GCDAuditor.cxx
  private/phys-services/I3RandomService.cxx
  private/phys-services/I3GSLRandomService.cxx
  private/phys-services/I3GSLRandomServiceFactory.cxx
  private/phys-services/I3MT19937.cxx
  private/phys-services/I3GeometryDecomposer.cxx
  private/phys-services/I3CutValues.cxx
  private/phys-services/I3ScaleCalculator.cxx
  private/phys-services/I3Splitter.cxx
  private/phys-services/I3NullSplitter.cxx
  private/phys-services/I3OrphanQDropper.cxx
  private/phys-services/I3VEMCalManipulator.cxx
  private/phys-services/I3VEMCalManipulatorFactory.cxx
  private/phys-services/I3XMLOMKey2MBID.cxx
  private/phys-services/I3XMLOMKey2MBIDFactory.cxx
  private/phys-services/I3SplitTriggerSelector.cxx
  private/phys-source/I3MetaSynth.cxx
  private/phys-source/I3GCDFileService.cxx
  private/phys-source/I3GCDFileServiceFactory.cxx
  private/phys-source/I3TextFileGeometryService.cxx
  private/phys-source/I3TextFileGeometryServiceFactory.cxx
  private/surfaces/Surface.cxx
  private/surfaces/SamplingSurface.cxx
  private/surfaces/Cylinder.cxx
  private/surfaces/ExtrudedPolygon.cxx
  private/surfaces/AxialCylinder.cxx
  private/surfaces/Sphere.cxx
  private/surfaces/polygon.cxx
  ${SPRNG_SOURCES}

  USE_PROJECTS serialization icetray dataclasses interfaces tableio photospline

  USE_TOOLS ${OPTIONAL_TOOLS} gsl boost python 
  )

i3_executable(CreateXMLOMKey2MBIDConversionTable
  private/CreateXMLOMKey2MBIDConversionTable/*.cxx
  USE_PROJECTS phys-services
  USE_TOOLS gsl)

if (SPRNG_FOUND)
  set(SPRNG_TESTS private/test/I3RandomServiceStateTest.cxx)
endif (SPRNG_FOUND)

i3_test_executable(test
  private/test/ContainmentSizeTest.cxx
  private/test/GeometrySelectorTests.cxx
  private/test/I3CalculatorTest.cxx	  
  private/test/I3CutsTest.cxx
  private/test/I3GeoSelTestModule.cxx
  private/test/I3RandomServiceTest.cxx	    
  private/test/I3ScaleCalculatorTest.cxx
  private/test/I3XMLOMKey2MBIDTest.cxx
  private/test/OMKey2MBIDTest.cxx
  private/test/OneFrameTest.cxx
  private/test/SmoothnessTest.cxx
  private/test/TestGeoTrimmers.cxx
  private/test/main.cxx
  ${SPRNG_TESTS}
  USE_PROJECTS phys-services dataio)

i3_test_scripts(resources/test/*.py)

i3_add_pybindings(phys_services
  private/pybindings/I3Calculator.cxx
  private/pybindings/I3ScaleCalculator.cxx
  private/pybindings/I3CrossSection.cxx
  private/pybindings/I3CutValues.cxx
  private/pybindings/I3CascadeCutValues.cxx
  private/pybindings/Converter.cxx
  private/pybindings/RandomServices.cxx
  private/pybindings/I3Cuts.cxx
  private/pybindings/I3Splitter.cxx
  private/pybindings/I3XMLOMKey2MBID.cxx
  private/pybindings/I3GCDFileService.cxx
  private/pybindings/Surface.cxx
  private/pybindings/module.cxx
  USE_TOOLS boost python 
  USE_PROJECTS phys-services interfaces
  )
