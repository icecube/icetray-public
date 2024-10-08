#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import sys
from pathlib import Path
from tempfile import TemporaryDirectory
import subprocess
import shutil

try:
    src = Path(os.environ["I3_SRC"])
except:
    sys.stderr.write("I3_SRC is not set, doing nothing\n")
    sys.exit(-1)

tempdir = TemporaryDirectory()

whitelist_files = [
    ".editorconfig",
    ".gitignore",
    ".pre-commit-config.yaml",
    "Brewfile",
    "CODEOWNERS",
    "CONTRIBUTING.rst",
    "DOMLauncher/private/pybindings/module.cxx",
    "MuonGun/private/cubature/README",
    "MuonGun/private/cubature/cubature.c",
    "MuonGun/private/pybindings/MuonGunBackgroundService.cxx",
    "MuonGun/public/cubature/cubature.h",
    "PROPOSAL/cmake/FindROOT_new.cmake",
    "PROPOSAL/public/PROPOSAL/json.hpp",
    "clsim/CMakeLists.txt",
    "clsim/private/clsim/I3CLSimLightSourceToStepConverterFlasher.cxx",
    "clsim/private/clsim/I3CLSimLightSourceToStepConverterPPC.cxx",
    "clsim/private/clsim/I3CLSimLightSourceToStepConverterUtils.cxx",
    "clsim/private/clsim/I3CLSimLightSourceToStepConverterUtils.h",
    "clsim/private/clsim/I3CLSimModule.cxx",
    "clsim/private/clsim/I3CLSimModuleHelper.cxx",
    "clsim/private/clsim/I3CLSimSpectrumTable.cxx",
    "clsim/private/clsim/converter/I3MCHitConverterWithIDs.cxx",
    "clsim/private/clsim/converter/I3MCHitConverterWithIDs.h",
    "clsim/private/clsim/dom/I3PhotonToMCHitConverterForMultiPMT.cxx",
    "clsim/private/clsim/opencl.hpp",
    "clsim/private/clsim/shadow/I3ShadowedPhotonRemoverModule.cxx",
    "clsim/private/clsim/tabulator/Axes.cxx",
    "clsim/private/clsim/tabulator/Axes.h",
    "clsim/private/clsim/tabulator/Axis.cxx",
    "clsim/private/clsim/tabulator/Axis.h",
    "clsim/private/clsim/tabulator/I3CLSimStepToTableConverter.cxx",
    "clsim/private/clsim/tabulator/I3CLSimStepToTableConverter.h",
    "clsim/private/clsim/tabulator/I3CLSimTabulatorModule.cxx",
    "clsim/private/clsim/util/I3MuonSliceRemoverAndPulseRelabeler.cxx",
    "clsim/private/clsim/util/I3MuonSlicer.cxx",
    "clsim/private/clsim/util/I3TauSanitizer.cxx",
    "clsim/private/cuda/I3CLSimStepToPhotonConverterCUDA.cxx",
    "clsim/private/cuda/I3CLSimStepToPhotonConverterCUDA.h",
    "clsim/private/cuda/kernels/CMakeLists.txt",
    "clsim/private/cuda/kernels/dataStructCuda.cuh",
    "clsim/private/cuda/kernels/domGeoData.cuh",
    "clsim/private/cuda/kernels/helper_math.h",
    "clsim/private/cuda/kernels/propagationKernelFunctions.cuh",
    "clsim/private/cuda/kernels/propagationKernelSource.cu",
    "clsim/private/cuda/kernels/propagationKernelSource.cuh",
    "clsim/private/cuda/kernels/rng.cuh",
    "clsim/private/cuda/kernels/scatteringAndAbsorbtionData.cuh",
    "clsim/private/cuda/kernels/settings.cuh",
    "clsim/private/cuda/kernels/utils.cuh",
    "clsim/private/cuda/kernels/wlenBiasSource.cuh",
    "clsim/private/cuda/kernels/wlenGeneration.cuh",
    "clsim/private/cuda/kernels/zOffsetHandling.cuh",
    "clsim/private/geant4/I3CLSimI3ParticleGeantConverter.hh",
    "clsim/private/geant4/I3CLSimLightSourcePropagatorGeant4.cxx",
    "clsim/private/geant4/I3CLSimLightSourcePropagatorGeant4.h",
    "clsim/private/geant4/TrkCerenkov.cxx",
    "clsim/private/geant4/TrkCerenkov.hh",
    "clsim/private/geant4/TrkDetectorConstruction.cxx",
    "clsim/private/geant4/TrkDetectorConstruction.hh",
    "clsim/private/geant4/TrkEventAction.cxx",
    "clsim/private/geant4/TrkEventAction.hh",
    "clsim/private/geant4/TrkOpticalPhysics.cxx",
    "clsim/private/geant4/TrkOpticalPhysics.hh",
    "clsim/private/geant4/TrkPrimaryGeneratorAction.cxx",
    "clsim/private/geant4/TrkPrimaryGeneratorAction.hh",
    "clsim/private/geant4/TrkStackingAction.cxx",
    "clsim/private/geant4/TrkStackingAction.hh",
    "clsim/private/geant4/TrkUISessionToQueue.cxx",
    "clsim/private/geant4/TrkUISessionToQueue.hh",
    "clsim/private/geant4/TrkUserEventInformation.cxx",
    "clsim/private/geant4/TrkUserEventInformation.hh",
    "clsim/private/opencl/I3CLSimHelperGenerateGeometrySource.cxx",
    "clsim/private/opencl/I3CLSimHelperGenerateGeometrySource.h",
    "clsim/private/opencl/I3CLSimHelperGenerateMediumPropertiesSource.cxx",
    "clsim/private/opencl/I3CLSimHelperGenerateMediumPropertiesSource.h",
    "clsim/private/opencl/I3CLSimHelperGenerateMediumPropertiesSource_Optimizers.cxx",
    "clsim/private/opencl/I3CLSimHelperGenerateMediumPropertiesSource_Optimizers.h",
    "clsim/private/opencl/I3CLSimHelperLoadProgramSource.h",
    "clsim/private/opencl/I3CLSimOpenCLDevice.cxx",
    "clsim/private/opencl/I3CLSimStepToPhotonConverterOpenCL.cxx",
    "clsim/private/opencl/ieeehalfprecision.cxx",
    "clsim/private/pybindings/I3CLSimLightSourcePropagatorGeant4.cxx",
    "clsim/private/pybindings/I3CLSimLightSourceToStepConverter.cxx",
    "clsim/private/pybindings/I3CLSimLightSourceToStepConverterUtils.cxx",
    "clsim/private/pybindings/I3CLSimModuleHelper.cxx",
    "clsim/private/pybindings/I3CLSimOpenCLDevice.cxx",
    "clsim/private/pybindings/I3CLSimSpectrumTable.cxx",
    "clsim/private/pybindings/I3CLSimStepToPhotonConverterOpenCL.cxx",
    "clsim/private/pybindings/I3CLSimTester.cxx",
    "clsim/private/pybindings/module.cxx",
    "clsim/private/pybindings/tabulator.cxx",
    "clsim/private/test/I3CLSimFunctionTester.cxx",
    "clsim/private/test/I3CLSimFunctionTester.h",
    "clsim/private/test/I3CLSimMediumPropertiesTester.cxx",
    "clsim/private/test/I3CLSimMediumPropertiesTester.h",
    "clsim/private/test/I3CLSimRandomDistributionTester.cxx",
    "clsim/private/test/I3CLSimRandomDistributionTester.h",
    "clsim/private/test/I3CLSimRandomNumberGeneratorBenchmark.cxx",
    "clsim/private/test/I3CLSimRandomNumberGeneratorBenchmark.h",
    "clsim/private/test/I3CLSimScalarFieldTester.cxx",
    "clsim/private/test/I3CLSimScalarFieldTester.h",
    "clsim/private/test/I3CLSimTesterBase.cxx",
    "clsim/private/test/I3CLSimTesterBase.h",
    "clsim/private/test/I3CLSimVectorTransformTester.cxx",
    "clsim/private/test/I3CLSimVectorTransformTester.h",
    "clsim/public/clsim/I3CLSimLightSourceToStepConverterFlasher.h",
    "clsim/public/clsim/I3CLSimLightSourceToStepConverterPPC.h",
    "clsim/public/clsim/I3CLSimModule.h",
    "clsim/public/clsim/I3CLSimModuleHelper.h",
    "clsim/public/clsim/I3CLSimOpenCLDevice.h",
    "clsim/public/clsim/I3CLSimSpectrumTable.h",
    "clsim/public/clsim/I3CLSimStepToPhotonConverterOpenCL.h",
    "clsim/public/clsim/dom/I3PhotonToMCHitConverterForMultiPMT.h",
    "clsim/public/clsim/shadow/I3ShadowedPhotonRemoverModule.h",
    "clsim/public/clsim/util/I3MuonSlicer.h",
    "clsim/python/AsyncTap.py",
    "clsim/python/FakeFlasherInfoGenerator.py",
    "clsim/python/FlasherInfoVectToFlasherPulseSeriesConverter.py",
    "clsim/python/GetAntaresOMAcceptance.py",
    "clsim/python/GetAntaresOMAngularSensitivity.py",
    "clsim/python/GetDefaultParameterizationList.py",
    "clsim/python/GetFlasherParameterizationList.py",
    "clsim/python/GetHybridParameterizationList.py",
    "clsim/python/GetIceCubeDOMAcceptance.py",
    "clsim/python/GetIceCubeDOMAngularSensitivity.py",
    "clsim/python/GetIceCubeFlasherSpectrum.py",
    "clsim/python/GetKM3NeTDOMAcceptance.py",
    "clsim/python/I3CLSimRandomValueIceCubeFlasherTimeProfile.py",
    "clsim/python/MakeAntaresMediumProperties.py",
    "clsim/python/MakeIceCubeMediumProperties.py",
    "clsim/python/MakeIceCubeMediumPropertiesPhotonics.py",
    "clsim/python/StandardCandleFlasherPulseSeriesGenerator.py",
    "clsim/python/__init__.py",
    "clsim/python/tablemaker/tabulator.py",
    "clsim/python/traysegments/I3CLSimMakeHits.py",
    "clsim/python/traysegments/I3CLSimMakeHitsFromPhotons.py",
    "clsim/python/traysegments/I3CLSimMakePhotons.py",
    "clsim/python/traysegments/__init__.py",
    "clsim/python/util/GetAnisotropyTransforms.py",
    "clsim/python/util/GetIceTiltZShift.py",
    "clsim/python/util/GetRefractiveIndexRange.py",
    "clsim/python/util/__init__.py",
    "clsim/python/util/interpolate.py",
    "clsim/resources/docs/faq.rst",
    "clsim/resources/docs/index.rst",
    "clsim/resources/docs/overview.rst",
    "clsim/resources/docs/tabulator.rst",
    "clsim/resources/kernels/Random123/LICENSE",
    "clsim/resources/kernels/Random123/include/Random123/MicroURNG.hpp",
    "clsim/resources/kernels/Random123/include/Random123/ReinterpretCtr.hpp",
    "clsim/resources/kernels/Random123/include/Random123/aes.h",
    "clsim/resources/kernels/Random123/include/Random123/array.h",
    "clsim/resources/kernels/Random123/include/Random123/ars.h",
    "clsim/resources/kernels/Random123/include/Random123/conventional/Engine.hpp",
    "clsim/resources/kernels/Random123/include/Random123/conventional/gsl_cbrng.h",
    "clsim/resources/kernels/Random123/include/Random123/features/clangfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/compilerfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/gccfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/iccfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/msvcfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/nvccfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/open64features.h",
    "clsim/resources/kernels/Random123/include/Random123/features/openclfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/pgccfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/sse.h",
    "clsim/resources/kernels/Random123/include/Random123/features/sunprofeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/features/xlcfeatures.h",
    "clsim/resources/kernels/Random123/include/Random123/gsl_microrng.h",
    "clsim/resources/kernels/Random123/include/Random123/philox.h",
    "clsim/resources/kernels/Random123/include/Random123/threefry.h",
    "clsim/resources/kernels/cylindrical_coordinates.c.cl",
    "clsim/resources/kernels/medium_properties_test_kernel.c.cl",
    "clsim/resources/kernels/medium_properties_test_kernel.h.cl",
    "clsim/resources/kernels/propagation_kernel.c.cl",
    "clsim/resources/kernels/propagation_kernel.h.cl",
    "clsim/resources/kernels/scalar_field_test_kernel.c.cl",
    "clsim/resources/kernels/scalar_field_test_kernel.h.cl",
    "clsim/resources/kernels/sparse_collision_kernel.c.cl",
    "clsim/resources/kernels/sparse_collision_kernel.h.cl",
    "clsim/resources/kernels/spherical_coordinates.c.cl",
    "clsim/resources/kernels/vector_transform_test_kernel.c.cl",
    "clsim/resources/kernels/vector_transform_test_kernel.h.cl",
    "clsim/resources/kernels/wlen_dep_val_test_kernel.c.cl",
    "clsim/resources/kernels/wlen_dep_val_test_kernel.h.cl",
    "clsim/resources/tablemaker/photomc.py",
    "cmc/private/pybindings/I3CascadeMCService.cxx",
    "cmc/private/pybindings/module.cxx",
    "cmc/resources/docs/cmc_code.rst",
    "cmc/resources/docs/index.rst",
    "cmc/resources/docs/physics_overview.rst",
    "earthmodel-service/private/pybindings/EarthModelCalculator.cxx",
    "earthmodel-service/private/pybindings/EarthModelService.cxx",
    "earthmodel-service/private/pybindings/module.cxx",
    "g4-tankresponse/private/g4-tankresponse/triangle/README",
    "g4-tankresponse/private/g4-tankresponse/triangle/triangle.c",
    "g4-tankresponse/private/g4-tankresponse/triangle/triangle.h",
    "genie-reader/resources/flux_root_files/generate_flux_hist_root.py",
    "ice-models/python/icewave/FourierToolset.py",
    "ice-models/python/icewave/PlusModeParametrization.py",
    "neutrino-generator/private/neutrino-generator/Step.h",
    "neutrino-generator/private/pybindings/Interaction.cxx",
    "neutrino-generator/private/pybindings/Steering.cxx",
    "neutrino-generator/private/pybindings/TypeDefs.cxx",
    "neutrino-generator/private/pybindings/ZenithSampler.cxx",
    "polyplopia/private/pybindings/CoincidentEventServices.cxx",
    "polyplopia/private/pybindings/I3MapString.cxx",
    "polyplopia/private/pybindings/PolyplopiaUtils.cxx",
    "pyproject.toml",
    "requirements.txt",
    "sim-services/private/pybindings/I3GeneratorService.cxx",
    "sim-services/private/pybindings/I3LightSourceParameterization.cxx",
    "sim-services/private/pybindings/I3LightSourcePropagator.cxx",
    "sim-services/private/pybindings/I3LightSourceToStepConverter.cxx",
    "sim-services/private/pybindings/I3PhotonPropagationServer.cxx",
    "sim-services/private/pybindings/I3SimConstants.cxx",
    "sim-services/private/pybindings/I3StepToPhotonConverter.cxx",
    "sim-services/private/pybindings/I3SumGenerator.cxx",
    "sim-services/private/sim-services/I3CombineMCPE.cxx",
    "sim-services/private/sim-services/I3DownsampleMCPE.cxx",
    "sim-services/private/sim-services/I3LightSourceParameterization.cxx",
    "sim-services/private/sim-services/I3LightSourceToStepConverter.cxx",
    "sim-services/private/sim-services/I3LightSourceToStepConverterAsync.cxx",
    "sim-services/private/sim-services/I3LightSourceToStepConverterPotemkin.h",
    "sim-services/private/sim-services/I3MCPEtoI3MCHitConverter.cxx",
    "sim-services/private/sim-services/I3PhotonPropagationClientModule.cxx",
    "sim-services/private/sim-services/I3PhotonToMCPEConverter.cxx",
    "sim-services/private/sim-services/I3PrimaryPulseMapper.cxx",
    "sim-services/private/sim-services/I3PropagatorModule.cxx",
    "sim-services/private/sim-services/I3RemoveLargeDT.cxx",
    "sim-services/private/sim-services/I3StepToPhotonConverterPotemkin.h",
    "sim-services/private/sim-services/OnboardNoiseSuppression.cxx",
    "sim-services/private/vendor/cppzmq/zmq.hpp",
    "sim-services/private/vendor/cppzmq/zmq_addon.hpp",
    "sim-services/public/sim-services/I3LightSourceParameterization.h",
    "sim-services/public/sim-services/I3LightSourceToStepConverter.h",
    "sim-services/public/sim-services/I3LightSourceToStepConverterAsync.h",
    "sim-services/public/sim-services/I3PhotonPropagationClientModule.h",
    "sim-services/public/sim-services/I3PhotonToMCPEConverter.h",
    "sim-services/public/sim-services/I3StepToPhotonConverter.h",
    "sim-services/public/sim-services/OnboardNoiseSuppression.h",
    "sim-services/resources/tests/test_mdom_onboard_noise_algorithm.py",
    "simclasses/private/pybindings/I3CLSimEventStatistics.cxx",
    "simclasses/private/pybindings/I3CLSimFunction.cxx",
    "simclasses/private/pybindings/I3CLSimMediumProperties.cxx",
    "simclasses/private/pybindings/I3CLSimPhoton.cxx",
    "simclasses/private/pybindings/I3CLSimPhotonHistory.cxx",
    "simclasses/private/pybindings/I3CLSimRandomValue.cxx",
    "simclasses/private/pybindings/I3CLSimScalarField.cxx",
    "simclasses/private/pybindings/I3CLSimVectorTransform.cxx",
    "simclasses/private/pybindings/I3CompressedPhoton.cxx",
    "simclasses/private/pybindings/I3Converters.cxx",
    "simclasses/private/pybindings/I3ExtraGeometryItem.cxx",
    "simclasses/private/pybindings/I3FlasherPulse.cxx",
    "simclasses/private/pybindings/I3LightSource.cxx",
    "simclasses/private/pybindings/I3MMCTrack.cxx",
    "simclasses/private/pybindings/I3Photon.cxx",
    "simclasses/private/pybindings/I3SimStep.cxx",
    "simclasses/private/pybindings/I3SimpleGeometry.cxx",
    "simclasses/private/pybindings/module.cxx",
    "simclasses/private/simclasses/I3CLSimEventStatistics.cxx",
    "simclasses/private/simclasses/I3CLSimMediumProperties.cxx",
    "simclasses/private/simclasses/I3CLSimPhoton.cxx",
    "simclasses/private/simclasses/I3CLSimPhotonHistory.cxx",
    "simclasses/private/simclasses/I3CompressedPhoton.cxx",
    "simclasses/private/simclasses/I3ExtraGeometryItem.cxx",
    "simclasses/private/simclasses/I3ExtraGeometryItemCylinder.cxx",
    "simclasses/private/simclasses/I3ExtraGeometryItemMove.cxx",
    "simclasses/private/simclasses/I3ExtraGeometryItemUnion.cxx",
    "simclasses/private/simclasses/I3FlasherPulse.cxx",
    "simclasses/private/simclasses/I3GENIEResultDict.cxx",
    "simclasses/private/simclasses/I3LightSource.cxx",
    "simclasses/private/simclasses/I3Photon.cxx",
    "simclasses/private/simclasses/I3SimStep.cxx",
    "simclasses/private/simclasses/I3SimpleGeometryFromI3Geometry.cxx",
    "simclasses/private/simclasses/I3SimpleGeometryTextFile.cxx",
    "simclasses/private/simclasses/converter/I3MCPEConverterWithIDs.cxx",
    "simclasses/private/simclasses/converter/I3MCPEConverterWithIDs.h",
    "simclasses/private/simclasses/converter/I3PhotonConverter.cxx",
    "simclasses/private/simclasses/converter/I3PhotonConverter.h",
    "simclasses/private/simclasses/function/I3CLSimFunction.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionAbsLenIceCube.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionConstant.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionDeltaPeak.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionFromTable.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionPolynomial.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionRefIndexIceCube.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionRefIndexQuanFry.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionScatLenIceCube.cxx",
    "simclasses/private/simclasses/function/I3CLSimFunctionScatLenPartic.cxx",
    "simclasses/private/simclasses/function/I3CLSimScalarField.cxx",
    "simclasses/private/simclasses/function/I3CLSimScalarFieldAnisotropyAbsLenScaling.cxx",
    "simclasses/private/simclasses/function/I3CLSimScalarFieldConstant.cxx",
    "simclasses/private/simclasses/function/I3CLSimScalarFieldIceTiltZShift.cxx",
    "simclasses/private/simclasses/function/I3CLSimVectorTransform.cxx",
    "simclasses/private/simclasses/function/I3CLSimVectorTransformConstant.cxx",
    "simclasses/private/simclasses/function/I3CLSimVectorTransformMatrix.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValue.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueApplyFunction.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueConstant.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueFixParameter.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueHenyeyGreenstein.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueInterpolatedDistribution.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueMixed.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueNormalDistribution.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueRayleighScatteringCosAngle.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueSimplifiedLiu.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueUniform.cxx",
    "simclasses/private/simclasses/random_value/I3CLSimRandomValueWlenCherenkovNoDispersion.cxx",
    "simclasses/public/simclasses/I3CLSimEventStatistics.h",
    "simclasses/public/simclasses/I3CLSimMediumProperties.h",
    "simclasses/public/simclasses/I3CLSimPhoton.h",
    "simclasses/public/simclasses/I3CLSimPhotonHistory.h",
    "simclasses/public/simclasses/I3CLSimQueue.h",
    "simclasses/public/simclasses/I3CompressedPhoton.h",
    "simclasses/public/simclasses/I3ExtraGeometryItem.h",
    "simclasses/public/simclasses/I3ExtraGeometryItemCylinder.h",
    "simclasses/public/simclasses/I3ExtraGeometryItemMove.h",
    "simclasses/public/simclasses/I3ExtraGeometryItemUnion.h",
    "simclasses/public/simclasses/I3FlasherPulse.h",
    "simclasses/public/simclasses/I3GENIEResultDict.h",
    "simclasses/public/simclasses/I3LightSource.h",
    "simclasses/public/simclasses/I3Photon.h",
    "simclasses/public/simclasses/I3SimStep.h",
    "simclasses/public/simclasses/I3SimStepStore.h",
    "simclasses/public/simclasses/I3SimpleGeometry.h",
    "simclasses/public/simclasses/I3SimpleGeometryFromI3Geometry.h",
    "simclasses/public/simclasses/I3SimpleGeometryTextFile.h",
    "simclasses/public/simclasses/function/I3CLSimFunction.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionAbsLenIceCube.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionConstant.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionDeltaPeak.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionFromTable.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionPolynomial.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionRefIndexIceCube.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionRefIndexQuanFry.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionScatLenIceCube.h",
    "simclasses/public/simclasses/function/I3CLSimFunctionScatLenPartic.h",
    "simclasses/public/simclasses/function/I3CLSimScalarField.h",
    "simclasses/public/simclasses/function/I3CLSimScalarFieldAnisotropyAbsLenScaling.h",
    "simclasses/public/simclasses/function/I3CLSimScalarFieldConstant.h",
    "simclasses/public/simclasses/function/I3CLSimScalarFieldIceTiltZShift.h",
    "simclasses/public/simclasses/function/I3CLSimVectorTransform.h",
    "simclasses/public/simclasses/function/I3CLSimVectorTransformConstant.h",
    "simclasses/public/simclasses/function/I3CLSimVectorTransformMatrix.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValue.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueApplyFunction.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueConstant.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueFixParameter.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueHenyeyGreenstein.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueInterpolatedDistribution.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueMixed.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueNormalDistribution.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueRayleighScatteringCosAngle.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueSimplifiedLiu.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueUniform.h",
    "simclasses/public/simclasses/random_value/I3CLSimRandomValueWlenCherenkovNoDispersion.h",
    "simclasses/public/simclasses/util/ToFloatString.h",
    "simprod-scripts/resources/docs/examples/fullSimulation.rst",
    "simprod-scripts/resources/scripts/SnowSuite/3-Snowstorm.py",
    "topsimulator/private/pybindings/module.cxx",
    "topsimulator/private/pybindings/vem_yield.cxx",
    "trigger-sim/private/pybindings/GlobalTriggerSim.cxx",
    "trigger-sim/private/pybindings/module.cxx",
]

whitelist = [
    ".github",
    ".reuse",
    "astro",
    "cmake",
    "dataclasses",
    "dataio",
    "docs",
    "hdfwriter",
    "icetray",
    "interfaces",
    "LICENSES",
    "phys-services",
    "serialization",
    "steamshovel",
    "tableio",
]

ignore = shutil.ignore_patterns("*.pyc")

for x in whitelist_files:
    dest = Path(tempdir.name) / x
    os.makedirs(dest.parent, exist_ok=True)
    shutil.copy(src / x, dest)

for x in whitelist:
    shutil.copytree(src / x, Path(tempdir.name) / x, ignore=ignore)

ret = subprocess.run(["reuse", "lint"], cwd=tempdir.name, check=False)
print("reuse returned ", ret.returncode)
sys.exit(ret.returncode)
