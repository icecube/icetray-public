
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

set(DATACLASSES_PYTHON_TEST_SCRIPTS
  resources/test/workout_pybindings.py
  resources/test/filtermasks_pybindings.py
  resources/test/i3geometry_version_test.py
  resources/test/std_map_indexing_suite_dict_interface_test.py
  resources/test/test_I3DetectorStatus.py
  resources/test/test_I3FlasherInfo.py
  resources/test/test_I3Orientation_setOrientation.py
  resources/test/test_I3TankGeo.py
  resources/test/test_I3Vectors.py
  resources/test/test_particle_masses.py  
  resources/test/i3calibration_version_test.py
  resources/test/i3matrix_test.py
  resources/test/std_map_indexing_suite_test.py
  resources/test/test_I3DOMCalibration.py
  resources/test/test_I3Geometry.py
  resources/test/test_i3particlevect.py
  resources/test/test_I3Time.py
  resources/test/test_I3VEMCalibration.py
  resources/test/test_shapeless_primary.py
  resources/test/i3detectorstatus_version_test.py
  resources/test/i3string_compare.py
  resources/test/test_I3DOMFunctions.py
  resources/test/test_I3MCHit.py
  resources/test/test_I3RecoHit.py
  resources/test/test_I3WaveformIsEqualOperator.py
  resources/test/test_tree_pybindings.py
  resources/test/i3double_compare.py
  resources/test/leap_second_test.py
  resources/test/test_I3DOMStatus.py
  resources/test/test_I3MCTreeUtils.py
  resources/test/test_I3RecoPulseSeriesMapApplySPECorrection.py
  resources/test/test_I3Trigger.py
  resources/test/test_I3Waveform.py
  resources/test/test_TriggerKey.py
  resources/test/i3eventheader_version_test.py
  resources/test/loads.py
  resources/test/test_get_most_energetic_cascade.py
  resources/test/test_I3EventHeader.py
  resources/test/test_I3ModuleGeo.py
  resources/test/test_I3RecoPulseSeriesMapMask_pybindings.py
  resources/test/test_I3TriggerStatus.py
  resources/test/test_linear_tree.py
  resources/test/i3frame_subterfuge.py
  resources/test/readwrite_i3vectors.py
  resources/test/test_I3Calibration.py
  resources/test/test_I3FilterResult.py
  resources/test/test_I3OMGeo.py
  resources/test/test_I3SuperDST_merge.py
  resources/test/test_I3UInt64.py
  resources/test/test_mctree_physics_library.py
  resources/test/test_disabling_of_python_dynamism.py
  )

if(Python_VERSION_MAJOR VERSION_GREATER_EQUAL 3)
  # this script throws a SyntaxError, which you can't catch
  # in Python, so we're only enabling it for Python3.
  list(APPEND DATACLASSES_PYTHON_TEST_SCRIPTS resources/test/test_I3Time_unixtime.py)
endif()


i3_test_scripts(${DATACLASSES_PYTHON_TEST_SCRIPTS})

set_tests_properties(dataclasses::test PROPERTIES LABELS RUN_LONG)
