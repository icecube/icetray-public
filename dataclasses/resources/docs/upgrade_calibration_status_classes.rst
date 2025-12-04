..
.. Copyright  (C) 2025 The Icecube Collaboration
.. SPDX-License-Identifier: BSD-2-Clause
..
.. @date $LastChangedDate: 24th October 2025 $
.. @author lbloom12

.. highlight:: python

.. _upgrade_calibration_status_classes-main:

===================================
IceCube Upgrade Calibration classes
===================================

This is an introduction to the dataclasses containing calibrations of the IceCube Upgrade OMs. Calibration classes contain the parameters and relationships that describe the individual PMTs and their digitizers. This is the type of information that you would expect to reference from a "datasheet". These classes are populated from information provided by the manufacturers, measurements taken in lab, and measurements taken in-situ.

****

I3DEggCal
-----------------------------------

:cpp:struct:`I3DEggCal` contains the unique parameters that describe a DEgg PMT and its digitizer. It also contains relationships between various settings and their effects that can be referenced. Many of the parameters are defined in `degg_constants.cpp <https://github.com/WIPACrepo/STM32Workspace/blob/master/wf-processing/include/wf-processing/degg/degg_constants.h>`_

**Members**

ADCToVolts

* The factor that converts the output of the digitizer (ADC Counts) to a calibrated PMT Voltage. For the DEgg, this is a constant value of 0.073e-3 Volts / ADC Counts.

SampleRate

* The Sample Rate of the digitizer. This is the factor that converts the sample bins (ADC Sample Number) to a time. For the DEgg, this is a constant value of 240 MHz.

FrontEndImpedance

* The Front-end Impedance of the mainboard. In the DEgg, this is a constant value of 36.96 Ohms.

DEggTimeOffset

* Systematic timing offset between DEggs and Gen1 DOMs. The measurement of this constant value is still in progress... in the meantime a value of **NAN** ns has been assigned.

LinearityParams

* The parameters that describe the relationship between an Ideal Number of Photoelectrons (NPEs) and the Measured NPEs. Ideally this relationship would be perfectly linear on the 1:1 line, but due to saturation of the PMT it tends to fall below this line at high NPE. This is stored as :cpp:struct:`LinearityParameters`, where all parameters are in units of milliamps (see ``LinearityParameters``).

HVGainRelation

* The slope and intercept that describe the linear relationship between log10(Gain) and log10(HV). This is referenced to determine the HV that should be set in order to achieve the desired Gain, and is stored as :cpp:struct:`LinearFit`.

PMTTransitTime

* Time offset between the start of a pulse (leading edge of a waveform) and when the photon initially hit the PMT, measured in nanoseconds.

PMTTransitTimeSpread

* The variance of the Transit Time distribution about the mean, measured in nanoseconds. This is used to sample an offset from the average Transit Time for use in simulations.

DACBaselineRelation

* The slope and intercept that describe the linear relationship between the ADC Count Baseline of the digitizer output and a DAC setting. This is referenced to determine the DAC setting that should be set in order to achieve the desired Baseline, and is stored as :cpp:struct:`LinearFit`.

Temperature

* Temperature of the DEgg Mainboard, measured in degrees Celsius. This is used to determine the Droop time-constant value (see ``TauParams``).

TauParams

* The parameters that describe the relationship between the Droop time-constant and Temperature. This is stored as :cpp:struct:`TauParam`. The droop effect is from the toroid in the DEgg mainboard and causes the waveform output to exponentially fall off. The time-constant is then necessary to correct for this effect. The Droop Correction is described in the `Wiki <https://wiki.icecube.wisc.edu/index.php/Droop_correction>`_, where we will employ a Single Tau Correction for the DEgg.



****


I3mDOMCal
-----------------------------------

:cpp:struct:`I3mDOMCal` contains the unique parameters that describe an mDOM PMT and its digitizer. It also contains relationships between various settings and their effects that can be referenced. Many of the parameters are defined in `mdom_constants.cpp <https://github.com/WIPACrepo/STM32Workspace/blob/master/wf-processing/include/wf-processing/mdom/mdom_constants.h>`_

**Members**

ADCToVolts

* The factor that converts the output of the digitizer (ADC Counts) to a calibrated PMT Voltage. For the mDOM, this is a constant value of 0.045e-3 Volts / ADC Counts.

SampleRate

* The Sample Rate of the digitizer. This is the factor that converts the sample bins (ADC Sample Number) to a time. For the mDOM, this is a constant value of 120 MHz.

DiscSampleRate

* The Sample Rate of the discriminator. Only mDOMs have a separate discriminator, which has a constant sample rate of 960 MHz.

FrontEndImpedance

* The Front-end Impedance of the mainboard. In the mDOM, this is a constant value of 75.35 Ohms.

mDOMTimeOffset

* Systematic timing offset between mDOMs and Gen1 DOMs. The measurement of this constant value is still in progress... in the meantime a value of **NAN** ns has been assigned.

LinearityParams

* The parameters that describe the relationship between an Ideal Number of Photoelectrons (NPEs) and the Measured NPEs. Ideally this relationship would be perfectly linear on the 1:1 line, but due to saturation of the PMT it tends to fall below this line at high NPE. This is stored as :cpp:struct:`LinearityParameters`, where all parameters are in units of PEs (see ``LinearityParameters``).

HVGainRelation

* The slope and intercept that describe the linear relationship between log10(Gain) and log10(HV). This is referenced to determine the HV that should be set in order to achieve the desired Gain, and is stored as :cpp:struct:`LinearFit`.

PMTTransitTime

* Time offset between the start of a pulse (leading edge of a waveform) and when the photon initially hit the PMT, measured in nanoseconds.

PMTTransitTimeSpread

* The variance of the Transit Time distribution about the mean, measured in nanoseconds. This is used to sample an offset from the average Transit Time for use in simulations.




****



LinearityParameters
----------------------------------

:cpp:struct:`LinearityParameters` contains the three parameters that describe the relationship between the Ideal Number of Photoelectrons (NPEs) and the Measured NPEs. Ideally this relationship would be perfectly linear on the 1:1 line, but due to saturation of the PMT it tends to fall below this line at high NPE.

The PMT Linearity parameters (p0, p1, p2) are described in Equation 4.1 of the `DEgg Paper <https://arxiv.org/pdf/2212.14526>`_

This relationship is:

    Observed = Ideal * ln(1 + Ideal/p2) / ( ln(1 + Ideal/p2) + (Ideal/p0)*ln(1 + (Ideal/p1)^3) )
 
where all variables and parameters are measured in the same units.




****

=======================================




=======================================
IceCube Upgrade DetectorStatus classes
=======================================

This is an introduction to the dataclasses containing detector status settings of the IceCube Upgrade OMs. DetectorStatus classes contain the settings, tuned to each individual PMT and its digitizer, that allow for the desired function of the OM. These settings are typically determined by referencing the relationships stored in the Calibration classes.


****


I3DEggStatus
-----------------------------------------

:cpp:struct:`I3DEggStatus` contains settings configured for a DEgg PMT at the start of every Run.

**Members**

Enabled

* A boolean that determines if this PMT is on *and* sending its readouts.

DEggPMTTrigMode

* The internal Trigger Mode of this DEgg PMT. The options are:
  
  * Analog-to-Digital Converter (ADC) = trigger whenever the counts in a single bin exceeds a threshold
  
  * Finite Impulse Response (FIR) = add up the counts from a set width of bins, weighted by their coefficients, and trigger whenever this sum exceeds a count threshold (done as to combat PMT noise so that a noisy count spike in a single bin does not trigger). This will most likely be the default option.

TrigThreshold

* The count threshold that determines if this PMT triggers. If using the FIR trigger, this threshold is compared to the weighted sum of bin counts.

FIRCoefficients

* An array of 16 integers that act as the weights to use with the FIR trigger. A more detailed description of how this is implemented in Feature Extraction is found on pg.47 of `xDOM MCU-FPGA Interface <https://uwprod.sharepoint.com/:w:/r/sites/icecubeupgrade/_layouts/15/Doc.aspx?sourcedoc=%7B7890b5f7-e337-4973-8d57-517a5e04bb02%7D&action=view&wdAccPdf=0&wdparaid=77FCD26>`_

PreSamples

* The number of samples included in the readout BEFORE the sample that was triggered.

PostSamples

* The number of samples included in the readout AFTER the trigger condition is no longer satisfied.

PMTHV

* The real operating High Voltage (HV) of this PMT, measured in Volts.

BaselineDAC

* The DAC setting used for shifting the waveform baseline.



****




I3mDOMStatus
-----------------------------------------

:cpp:struct:`I3mDOMStatus` contains settings configured for an mDOM PMT at the start of every Run.

**Members**

Enabled

* A boolean that determines if this PMT is on *and* sending its readouts.

mDOMPMTTrigMode

* The internal Trigger Mode of this mDOM PMT. The options are:
  
  * Analog-to-Digital Converter (ADC) = trigger whenever the counts in a single bin exceeds a threshold
  
  * Discriminator (Disc) = trigger whenever the discriminator exceeds some threshold. This will most likely be the default option.

ADCThreshold

* The count threshold for the ADC trigger. This value is set for the entire mDOM module so it will be the same for each of the 24 individual PMTs.

PreSamples

* The number of samples included in the readout BEFORE the sample that was triggered.

PostSamples

* The number of samples included in the readout AFTER the trigger condition is no longer satisfied.

PMTHV

* The real operating High Voltage (HV) of this PMT, measured in Volts.

ADCBaselineDAC

* The DAC setting used for shifting the waveform baseline.

DiscDAC

* The DAC setting used for shifting the discriminator threshold voltage.