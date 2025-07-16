# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import icecube.icetray # be nice and pull in our dependencies

# XXX Work around a stupid bug with certain combinations of Boost, compiler, and
# RTLD where pulling in libdataclasses as a dependency of the pybindings will
# cause corruption of the internal state of the extended_type_info database
# when classes in dataclasses are inherited from in certain circumstances I
# can't identify.
# Observed on: FreeBSD 10 + Boost 1.52 + clang 3.2 when loading mutineer
# (of all things)
icecube.icetray.load('dataclasses', False)
from icecube._dataclasses import (
    AMANDA_MUON_DAQ, AMANDA_TWR_DAQ, AntennaKey, AntennaSpectrum, AntennaSpectrumMap, AntennaTimeSeries,
    AntennaTimeSeriesMap, CALIBRATION, Custom, Database, DroopedSPETemplate, EFieldSpectrum, EFieldSpectrumMap,
    EFieldTimeSeries, EFieldTimeSeriesMap, EXTERNAL, ExtrapolatedLinear, FAINT_PARTICLE, FFTData, FFTData3D,
    FFTData3DMap, FFTDataMap, FRAGMENT_MULTIPLICITY, Feb2011, GLOBAL, I3AntennaChannel, I3AntennaChannelMap,
    I3AntennaDataMap, I3AntennaGeo, I3AntennaGeoMap, I3CalendarDate, I3Calibration, I3ComplexVector, I3Constants,
    I3DOMCalibration, I3DOMLaunch, I3DOMLaunchSeries, I3DOMLaunchSeriesMap, I3DOMStatus, I3DetectorStatus, I3Direction,
    I3Double, I3EventHeader, I3FilterResult, I3FilterResultMap, I3FlasherInfo, I3FlasherInfoVect, I3FlasherStatus,
    I3FlasherStatusMap, I3FlasherSubrunMap, I3Geometry, I3IceActBoolMap, I3IceActDoubleMap, I3IceActGeo, I3IceActGeoMap,
    I3IceActIntMap, I3IceActPixelPositionMap, I3IceActRecoPulseSeries, I3IceActRecoPulseSeriesMap, I3IceActStringMap,
    I3IceActTelescopePixelDoubleMap, I3IceActTelescopePixelUnsignedMap, I3IceActTelescopePixelVectorIntMap,
    I3IceActTelescopeStringDoubleMap, I3IceActTriggerMap, I3IceActUnsignedMap, I3IceActWaveform, I3IceActWaveformMap,
    I3IceTopSLCCalibration, I3IceTopSLCCalibrationCollection, I3LinearizedMCTree, I3MCHit, I3MCHitSeriesMap, I3MCTree,
    I3MapAntennaKeyAntennaKey, I3MapI3ParticleIDDouble, I3MapIntVectorInt, I3MapKeyDouble, I3MapKeyUInt,
    I3MapKeyVectorDouble, I3MapKeyVectorInt, I3MapModuleKeyString, I3MapStringBool, I3MapStringDouble, I3MapStringInt,
    I3MapStringStringDouble, I3MapStringVectorDouble, I3MapTriggerDouble, I3MapTriggerUInt, I3MapTriggerVectorUInt,
    I3MapUShortUShort, I3MapUnsignedUnsigned, I3Matrix, I3ModuleGeo, I3ModuleGeoMap, I3OMGeo, I3OMGeoMap, I3Orientation,
    I3Particle, I3ParticleID, I3Position, I3RecoHit, I3RecoHitSeriesMap, I3RecoPulse, I3RecoPulseSeriesMap,
    I3RecoPulseSeriesMapApplySPECorrection, I3RecoPulseSeriesMapCombineByModule, I3RecoPulseSeriesMapMask,
    I3RecoPulseSeriesMapUnion, I3ScintGeo, I3ScintGeoMap, I3ScintRecoPulseSeriesMap, I3ScintWaveformSeriesMap,
    I3StationGeo, I3StationGeoMap, I3String, I3SuperDST, I3SuperDSTTrigger, I3SuperDSTTriggerSeries, I3TankGeo, I3Time,
    I3TimeWindow, I3TimeWindowSeries, I3TimeWindowSeriesMap, I3Trigger, I3TriggerHierarchy, I3TriggerStatus, I3UInt64,
    I3VEMCalibration, I3VectorAntennaKey, I3VectorBool, I3VectorChar, I3VectorDouble, I3VectorDoubleDouble,
    I3VectorFloat, I3VectorI3Particle, I3VectorI3Position, I3VectorI3Trigger, I3VectorInt, I3VectorInt64,
    I3VectorModuleKey, I3VectorOMKey, I3VectorParticleType, I3VectorScintKey, I3VectorShort, I3VectorString,
    I3VectorTankKey, I3VectorUInt, I3VectorUInt64, I3VectorUShort, I3Waveform, I3WaveformSeries, I3WaveformSeriesMap,
    ICE_TOP, IN_ICE, IN_ICE_PULSES, ITSLCCalProvenance, IceActKey, InterpolatedLinear, LASER, Level2Raw, Level2VEM,
    LinearFit, ListAntennaKey, ListBool, ListDoubleDouble, ListFloat, ListI3Particle, ListI3Position, ListInt64,
    ListModuleKey, ListParticleType, ListScintKey, ListTankKey, ListUShort, M18, M24, MERGED, MIN_BIAS,
    Map_OMKey_I3DOMCalibration, Map_OMKey_I3DOMStatus, Map_OMKey_I3IceTopSLCCalibration, Map_OMKey_I3VEMCalibration,
    Map_TriggerKey_I3TriggerStatus, Mixed, ModuleKey, MoniRaw, NO_SUBTYPE, PFFiltRaw, PairDoubleDouble, Placeholder,
    QuadraticFit, SIMPLE_MULTIPLICITY, SLOW_PARTICLE, SPASE, SPASE_2, SPEChargeDistribution, SPETemplate, SPHERE,
    STRING, ScintKey, SnowHeightProvenance, SourceID, SubtypeID, T0, THREE_COINCIDENCE, THROUGHPUT, TWO_COINCIDENCE,
    TankKey, TauParam, TestMapStringString, TriggerKey, TriggerKeyVector, TypeID, UNBIASED, UNKNOWN_SOURCE,
    UNKNOWN_SUBTYPE, UNKNOWN_TYPE, Unknown, VEMCal, VOLUME, VectorI3WaveformStatusCompound, atwd_sampling_rate,
    day_of_year, domcaal_version, fadc_baseline, fft, get_most_energetic, get_most_energetic_cascade,
    get_most_energetic_inice, get_most_energetic_inice_cascade, get_most_energetic_muon, get_most_energetic_neutrino,
    get_most_energetic_nucleus, get_most_energetic_primary, get_most_energetic_track, i3position_to_tuple, identity,
    julianday, leap_second_on_mjd, make_I3Time, make_pair, map_OMKey_I3MCHit, map_Subdetector_I3TriggerReadoutConfig,
    max_daq_time, mean_spe_charge, modjulianday, mpe_discriminator_threshold, old_mpe_threshold, old_spe_threshold,
    pmt_gain, spe_discriminator_threshold, spe_mean, spe_pmt_threshold, takes_vector_I3Particle, transit_time,
    vector_I3MCHit, vector_I3RecoHit, vector_I3RecoPulse, which_atwd, year_of
)

# Add some aliases for naming consistency
I3RecoPulseSeries = vector_I3RecoPulse
I3RecoHitSeries = vector_I3RecoHit
I3MCHitSeries = vector_I3MCHit

# Aliases for collections factored out into their own implementation files
I3TriggerStatusMap = Map_TriggerKey_I3TriggerStatus
I3VEMCalibrationMap = Map_OMKey_I3VEMCalibration
I3IceTopSLCCalibrationMap = Map_OMKey_I3IceTopSLCCalibration
I3DOMCalibrationMap = Map_OMKey_I3DOMCalibration
I3DOMStatusMap = Map_OMKey_I3DOMStatus
ParticleTypeVect = ListParticleType
setattr(TankKey, "I3VectorTankKey", I3VectorTankKey)  # noqa: B010

del icecube

import icecube._tableio_dataclasses as converters
