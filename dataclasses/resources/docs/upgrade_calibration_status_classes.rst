..
.. Copyright  (C) 2025 The Icecube Collaboration
.. SPDX-License-Identifier: BSD-2-Clause
..
.. @date $LastChangedDate: 30th April 2026 $
.. @author lbloom12

.. highlight:: python

.. _upgrade_calibration_status_classes-main:

IceCube Upgrade Calibration classes
===================================

This is an introduction to the dataclasses containing calibrations of the IceCube Upgrade OMs. Calibration classes contain the
parameters and relationships that describe the individual PMTs and their digitizers. This is the type of information that you
would expect to reference from a "datasheet". These classes are populated from information provided by the manufacturers,
measurements taken in lab, and measurements taken in-situ.

****

I3PMTCal
--------

:cpp:struct:`I3PMTCal` is the generic calibration class. It holds parameters that reconstructions require for all PMTs in the
detector, regardless of the optical module (DOM, D-Egg, mDOM, etc.)

>>> from icecube import dataclasses
>>> from icecube.icetray import I3Units
>>>
>>> pmt_cal = dataclasses.I3PMTCal()

**Members**

RelativePMTEfficiency
  The modifier for the probability of a photon to create a photoelectron upon hitting the PMT face. This value is relative to
  the nominal efficiency for the specified PMT model.

  >>> pmt_cal.relative_pmt_eff = 1

PMTNoiseRate
  The noise rate of the PMT, measured in Hz.

  >>> pmt_cal.noise_rate = 5.5e-7 * (1/I3Units.ns)

SPEChargeDist
  The fitted parameters used to describe the distribution of charge observed for when a single photon hits the PMT face.
  This is stored as :cpp:struct:`SPEChargeDistribution` which allows for an arbitrary number of exponential and gaussian
  PDFs to be included upon initialization. See `SPEChargeDistribution documentation
  <https://github.com/icecube/icetray/blob/main/dataclasses/resources/docs/spechargedistribution.rst>`_

  >>> pmt_cal.spe_charge_dist.pdfs.append(dataclasses.SPEChargeDistribution.Exponential(amplitude = 1, width = 2))

****

I3DEggCal
---------

:cpp:struct:`I3DEggCal` contains the unique parameters that describe a DEgg PMT and its digitizer. It also contains
relationships between various settings and their effects that can be referenced. Many of the parameters are defined in
`degg_constants.cpp
<https://github.com/WIPACrepo/STM32Workspace/blob/master/wf-processing/include/wf-processing/degg/degg_constants.h>`_

>>> from icecube import dataclasses
>>> from icecube.icetray import I3Units
>>>
>>> degg_cal = dataclasses.I3DEggCal()

**Members**

ADCToVolts
  The factor that converts the output of the digitizer (ADC Counts) to a calibrated PMT Voltage. For the DEgg, this is a
  constant value of 7.280e-5 Volts / ADC Counts.

  >>> degg_cal.adcToVolts

SampleRate
  The Sample Rate of the digitizer. This is the factor that converts the sample bins (ADC Sample Number) to a time. For the
  DEgg, this is a constant value of 240 MHz.

  >>> degg_cal.sampleRate

FrontEndImpedance
  The Front-end Impedance of the mainboard. In the DEgg, this is a constant value of 36.96 Ohms.

  >>> degg_cal.frontEndImpedance

DEggTimeOffset
  Systematic timing offset between DEggs and Gen1 DOMs. The measurement of this constant value is still in progress... in the
  meantime a value of **NAN** ns has been assigned.

  >>> degg_cal.deggTimeOffset

HVGainRelation
  The slope and intercept that describe the linear relationship between log10(Gain) and log10(HV). This is referenced to
  determine the HV that should be set in order to achieve the desired Gain, and is stored as :cpp:struct:`LinearFit`.

  >>> degg_cal.hv_gain_relation.slope = 6.25    # [ln(Gain) / ln(V)]
  >>> degg_cal.hv_gain_relation.intercept = -30 # [ln(Gain)]

PMTTransitTime
  Time offset between the start of a pulse (leading edge of a waveform) and when the photon initially hit the PMT, measured in
  nanoseconds.

  >>> degg_cal.pmt_transit_time = 54 * I3Units.ns

PMTTransitTimeSpread
  The variance of the Transit Time distribution about the mean, measured in nanoseconds. This is used to sample an offset from
  the average Transit Time for use in simulations.

  >>> degg_cal.pmt_transit_time_spread = 2.89 * I3Units.ns

ADCBaselineRMS
  The uncertainty in the ADC Baseline of the digitizer output, measured in ADC Counts. See the `Wiki
  <https://wiki.icecube.wisc.edu/index.php/D-Egg_MC_Inputs#ADC_baseline.2Fnoise>`_ for more information.

  >>> degg_cal.adc_baseline_rms = 2.5 # [Counts]

DACBaselineRelation
  The slope and intercept that describe the linear relationship between the ADC Count Baseline of the digitizer output and a
  DAC setting. This is referenced to determine the DAC setting that should be set in order to achieve the desired Baseline,
  and is stored as :cpp:struct:`LinearFit`.

  >>> degg_cal.dac_baseline_relation.slope = 4/15  # [Counts / DAC]
  >>> degg_cal.dac_baseline_relation.intercept = 0 # [Counts]

Temperature
  Temperature of the DEgg Mainboard, measured in Kelvin. This is used to determine the Droop time-constant value (see
  ``TauParams``).

  >>> degg_cal.temperature = (-40 + 273.0) * I3Units.kelvin # (-40 degrees Celsius)

TauParams
  The parameters that describe the relationship between the Droop time-constant and Temperature. This is stored as
  :cpp:struct:`TauParam`. The droop effect is from the toroid in the DEgg mainboard and causes the waveform output to
  exponentially fall off. The time-constant is then necessary to correct for this effect. The Droop Correction is described in
  the `Wiki <https://wiki.icecube.wisc.edu/index.php/Droop_correction>`_, where we will employ a Single Tau Correction for the
  DEgg.

  >>> degg_cal.tau_params.p0 = 4.9  * I3Units.microsecond
  >>> degg_cal.tau_params.p1 = 26.7 * I3Units.microsecond
  >>> degg_cal.tau_params.p2 = (28.3 + 273.0) * I3Units.kelvin # (28.3 degrees Celsius)

****

I3mDOMCal
---------

:cpp:struct:`I3mDOMCal` contains the unique parameters that describe an mDOM PMT and its digitizer. It also contains
relationships between various settings and their effects that can be referenced. Many of the parameters are defined in
`mdom_constants.cpp
<https://github.com/WIPACrepo/STM32Workspace/blob/master/wf-processing/include/wf-processing/mdom/mdom_constants.h>`_

>>> from icecube import dataclasses
>>> from icecube.icetray import I3Units
>>>
>>> mdom_cal = dataclasses.I3mDOMCal()

**Members**

ADCToVolts
  The factor that converts the output of the digitizer (ADC Counts) to a calibrated PMT Voltage. For the mDOM, this is a
  constant value of 0.045e-3 Volts / ADC Counts.

  >>> mdom_cal.adcToVolts

SampleRate
  The Sample Rate of the digitizer. This is the factor that converts the sample bins (ADC Sample Number) to a time. For the
  mDOM, this is a constant value of 120 MHz.

  >>> mdom_cal.sampleRate

DiscSampleRate
  The Sample Rate of the discriminator. Only mDOMs have a separate discriminator, which has a constant sample rate of 960 MHz.

  >>> mdom_cal.discSampleRate

FrontEndImpedance
  The Front-end Impedance of the mainboard. In the mDOM, this is a constant value of 75.35 Ohms.

  >>> mdom_cal.frontEndImpedance

mDOMTimeOffset
  Systematic timing offset between mDOMs and Gen1 DOMs. The measurement of this constant value is still in progress... in the
  meantime a value of **NAN** ns has been assigned.

  >>> mdom_cal.mdomTimeOffset

HVGainRelation
  The slope and intercept that describe the linear relationship between log10(Gain) and log10(HV). This is referenced to
  determine the HV that should be set in order to achieve the desired Gain, and is stored as :cpp:struct:`LinearFit`.

  >>> mdom_cal.hv_gain_relation.slope = 6.85      # [ln(Gain) / ln(V)]
  >>> mdom_cal.hv_gain_relation.intercept = -14.5 # [ln(Gain)]

PMTTransitTime
  Time offset between the start of a pulse (leading edge of a waveform) and when the photon initially hit the PMT, measured in
  nanoseconds.

  >>> mdom_cal.pmt_transit_time = 44 * I3Units.ns

PMTTransitTimeSpread
  The variance of the Transit Time distribution about the mean, measured in nanoseconds. This is used to sample an offset from
  the average Transit Time for use in simulations.

  >>> mdom_cal.pmt_transit_time_spread = 2.5 * I3Units.ns

ADCBaselineRMS
  The uncertainty in the ADC Baseline of the digitizer output, measured in ADC Counts.

  >>> mdom_cal.adc_baseline_rms = 2.5 # [Counts]

ADCBaselineValue
  The requested ADC Baseline value in the calibration. This is the desired value for the baseline of the waveform in ADC Counts.

  >>> mdom_cal.adc_baseline_value = 3600 # [Counts]

ADCBaselineDAC
  The DAC value returned by the calibration that yields a baseline value that best matches ``ADCBaselineValue``.

  >>> mdom_cal.adc_baseline_dac = 13300  # [DAC]

DiscThreshold
  The requested Disciminator Threshold in the calibration. This is the desired value for the threshold in Volts.

  >>> mdom_cal.disc_threshold = 0.002 * I3Units.volt

DiscDAC
  The DAC value returned by the calibration that yields a discriminator threshold that best matches ``DiscThreshold``.

  >>> mdom_cal.disc_dac = 23500 # [DAC]

**Functions**

GetValidBaselineValue()
  Returns ``ADCBaselineValue`` if and only if the inputted value matches ``ADCBaselineDAC``, otherwise returns a RuntimeError.

  >>> mdom_cal.adc_baseline_value = 3600
  >>> mdom_cal.adc_baseline_dac = 13300
  >>> mdom_cal.GetValidBaselineValue(13300)
  3600
  >>> mdom_cal.GetValidBaselineValue(12300)
  RuntimeError: The specified DAC value does not match what is in calibration!

GetValidDiscThreshold()
  Returns ``DiscThreshold`` if and only if the inputted value matches ``DiscDAC``, otherwise returns a RuntimeError.

  >>> mdom_cal.disc_threshold = 0.002
  >>> mdom_cal.disc_dac = 23500
  >>> mdom_cal.GetValidDiscThreshold(23500)
  0.002
  >>> mdom_cal.GetValidDiscThreshold(12300)
  RuntimeError: The specified DAC value does not match what is in calibration!


****

IceCube Upgrade DetectorStatus classes
======================================

This is an introduction to the dataclasses containing detector status settings of the IceCube Upgrade OMs. DetectorStatus
classes contain the settings, tuned to each individual PMT and its digitizer, that allow for the desired function of the OM.
These settings are typically determined by referencing the relationships stored in the Calibration classes.

****

I3DEggStatus
------------

:cpp:struct:`I3DEggStatus` contains settings configured for a DEgg PMT at the start of every Run.

>>> from icecube import dataclasses
>>> from icecube.icetray import I3Units
>>>
>>> degg_status = dataclasses.I3DEggStatus()

**Members**

Enabled
  A boolean that determines if this PMT is on *and* sending its readouts.

  >>> degg_status.enabled = True # this PMT is sending readouts

DEggPMTTrigMode
  The internal Trigger Mode of this DEgg PMT. The options are:
  
  * Analog-to-Digital Converter (ADC) -- Trigger whenever the counts in a single bin exceeds a threshold
  * Finite Impulse Response (FIR) -- Add up the counts from a set width of bins, weighted by their coefficients, and trigger
    whenever this sum exceeds a count threshold (done as to combat PMT noise so that a noisy count spike in a single bin does
    not trigger). This will most likely be the default option.

  >>> degg_status.trig_mode = dataclasses.I3DEggStatus.FIR

TrigThreshold
  The count threshold that determines if this PMT triggers. If using the FIR trigger, this threshold is compared to the
  weighted sum of bin counts.

  >>> degg_status.trig_threshold = 12 # [Counts]

FIRCoefficients
  An array of 16 integers that act as the weights to use with the FIR trigger. A more detailed description of how this is
  implemented in Feature Extraction is found on pg.47 of `xDOM MCU-FPGA Interface
  <https://uwprod.sharepoint.com/:w:/r/sites/icecubeupgrade/_layouts/15/Doc.aspx?sourcedoc=%7B7890b5f7-e337-4973-8d57-517a5e04bb02%7D&action=view&wdAccPdf=0&wdparaid=77FCD26>`_

  >>> degg_status.fir_coefficients = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0]

ReadoutMode
  The readout can have either a fixed number of samples in a launch or a variable number of samples in a launch.
  If the readout mode is variable then it is configured with ``preSamples`` & ``postSamples``.

  >>> degg_status.readout_mode = dataclasses.I3DEggStatus.VARIABLE_LENGTH

PreSamples
  The number of samples included in the readout BEFORE the sample that was triggered.

  >>> degg_status.pre_samples = 16

PostSamples
  The number of samples included in the readout AFTER the trigger condition is no longer satisfied.

  >>> degg_status.post_samples = 16

PMTHV
  The real operating High Voltage (HV) of this PMT, measured in Volts.

  >>> degg_status.pmt_hv = 1600 * I3Units.volt

BaselineDAC
  The DAC setting used for shifting the waveform baseline.

  >>> degg_status.baseline_dac = 30000 # [DAC]

****

I3mDOMStatus
------------

:cpp:struct:`I3mDOMStatus` contains settings configured for an mDOM PMT at the start of every Run.

>>> from icecube import dataclasses
>>> from icecube.icetray import I3Units
>>>
>>> mdom_status = dataclasses.I3mDOMStatus()

**Members**

Enabled
  A boolean that determines if this PMT is on *and* sending its readouts.

  >>> mdom_status.enabled = True # this PMT is sending readouts

mDOMPMTTrigMode
  The internal Trigger Mode of this mDOM PMT. The options are:
  
  * Analog-to-Digital Converter (ADC) -- Trigger whenever the counts in a single bin exceeds a threshold
  * Discriminator (Disc) -- Trigger whenever the discriminator exceeds some threshold. This will most likely be the default
    option.

  >>> mdom_status.trig_mode = dataclasses.I3mDOMStatus.Disc

ADCThreshold
  The count threshold for the ADC trigger. This value is set for the entire mDOM module so it will be the same for each of the
  24 individual PMTs.

  >>> mdom_status.adc_threshold = 10 # [Counts]

ReadoutMode
  The readout can have either a fixed number of samples in a launch or a variable number of samples in a launch.
  If the readout mode is variable then it is configured with ``preSamples`` & ``postSamples``.

  >>> mdom_status.readout_mode = dataclasses.I3mDOMStatus.VARIABLE_LENGTH

PreSamples
  The number of samples included in the readout BEFORE the sample that was triggered.

  >>> mdom_status.pre_samples = 16

PostSamples
  The number of samples included in the readout AFTER the trigger condition is no longer satisfied.

  >>> mdom_status.post_samples = 16

PMTHV
  The real operating High Voltage (HV) of this PMT, measured in Volts.

  >>> mdom_status.pmt_hv = 85 * I3Units.volt

ADCBaselineDAC
  The DAC setting used for shifting the waveform baseline.

  >>> mdom_status.adc_baseline_dac = 13300 # [DAC]

DiscDAC
  The DAC setting used for shifting the discriminator threshold voltage.

  >>> mdom_status.disc_dac = 23500 # [DAC]
