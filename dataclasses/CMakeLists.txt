
i3_project(dataclasses
  PYTHON_DIR python
  DOCS_DIR resources/docs)

i3_add_library(dataclasses
  private/dataclasses/*.cxx
  private/dataclasses/*.c
  private/dataclasses/*/*.cxx
  private/dataclasses/*/*/*.cxx
  private/dataclasses/stream_overloads.cpp
  private/test_module/TestConstCache.cxx

  USE_TOOLS boost python

  USE_PROJECTS serialization icetray
  )


i3_test_executable(test 
  private/test/*.cxx
  USE_PROJECTS serialization dataclasses dataio
  USE_PYBINDINGS dataio
  )

i3_add_pybindings(dataclasses
  private/pybindings/I3Calibration/*.cxx
  private/pybindings/I3Constants.cxx
  private/pybindings/I3DOMLaunch/*.cxx
  private/pybindings/I3DetectorStatus/*.cxx
  private/pybindings/I3DOMFunctions.cxx
  private/pybindings/I3Direction.cxx
  private/pybindings/I3Double.cxx
  private/pybindings/I3PODHolder/I3UInt64.cxx
  private/pybindings/I3EventHeader.cxx
  private/pybindings/I3FlasherInfo.cxx
  private/pybindings/I3FilterResult.cxx
  private/pybindings/I3Geometry.cxx
  private/pybindings/I3OMGeo.cxx
  private/pybindings/I3TankGeo.cxx
  private/pybindings/I3MCHit/*.cxx
  private/pybindings/I3MCTree.cxx
  private/pybindings/I3LinearizedMCTree.cxx
  private/pybindings/I3ParticleID.cxx
  private/pybindings/I3Particle.cxx
  private/pybindings/I3Position.cxx
  private/pybindings/I3RecoHit.cxx
  private/pybindings/I3RecoPulse.cxx
  private/pybindings/I3String.cxx
  private/pybindings/I3Matrix.cxx
  private/pybindings/I3Time.cxx
  private/pybindings/I3TimeWindow.cxx
  private/pybindings/I3Trigger.cxx
  private/pybindings/I3TriggerHierarchy.cxx
  private/pybindings/I3Waveform/*.cxx
  private/pybindings/I3Vectors/*.cxx
  private/pybindings/I3Maps/*.cxx
  private/pybindings/I3Orientation.cxx
  private/pybindings/I3SuperDST.cxx
  private/pybindings/I3ModuleGeo.cxx
  private/pybindings/I3MCTreePhysicsLibrary.cxx
  private/pybindings/ModuleKey.cxx
  private/pybindings/module.cxx
  private/pybindings/TankKey.cxx
  private/pybindings/TriggerKey.cxx
  private/pybindings/test_utils/I3VectorUtils.cxx
  USE_TOOLS boost python
  USE_PROJECTS serialization icetray dataclasses
  )

i3_test_scripts(resources/test/*.py)
set_tests_properties(dataclasses::test PROPERTIES LABELS RUN_LONG)
