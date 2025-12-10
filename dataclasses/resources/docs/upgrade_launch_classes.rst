..
.. Copyright  (C) 2025 The Icecube Collaboration
.. SPDX-License-Identifier: BSD-2-Clause
..
.. @date $LastChangedDate: 22nd September 2025 $
.. @author Tom Stuttard

.. highlight:: python

.. _upgrade_launch_classes-main:

IceCube Upgrade launch/hit classes
==================================

This page gives a brief introduction to the dataclasses containing the raw
output data from the IceCube Upgrade OMs, known as *launches*. These launch
classes are populated via decoding of raw data in the case of real detector
operations, or by the :cpp:class:`DOMLauncher` project in simulations.

The main reference document for the output data format is the
`xDOMApp Reference Manual <https://github.com/WIPACrepo/STM32Workspace/tree/master/xdomapp/docs>`__.


I3XDOMHit
^^^^^^^^^

:cpp:class:`I3XDOMHit` contains an extracted pulse (single photoelectron) from
the readout of an OM running XDOMApp, such as an mDOM, DEgg or PDOM, for a
single channel/PMT. It is the result of the on-board waveform extraction. If
this extraction fails, a *launch* dataclass of the relevant OM type will
instead be filled (details below).

Members
-------

**Time**

The feature extracted time of the single photoelectron.

**Charge**

The feature extracted charge of the single photoelectron, in units of
photoelectrons (p.e.).

**LC**

Indicates if a *local coincidence* condition was flagged by the module. This
refers to a case where multiple PMTs see hits on the same OM within some
(configurable) time window. Note that unlike Gen1 DOMs, LC for Upgrade OMs
only considers a single OM (e.g. there is no LC connection to neighbouring
OMs).


I3DEggLaunch
^^^^^^^^^^^^

:cpp:class:`I3DEggLaunch` contains the output data from the DEgg, for a single
channel/PMT. It includes an instance of the common :cpp:class:`I3XDOMLaunch`
class. Note that this class is only returned when on-board waveform extraction
fails, otherwise :cpp:class:`I3XDOMHit` is returned.

The PDOM also outputs this dataclass, since it shares the same frontend
electronics as the DEgg. For convenience, the ``typedef``
:cpp:type:`I3PDOMLaunch` are also provided.

Members
-------

**Time**

The time (in nanoseconds) of the 240 MHz ADC bin in which the launch is
triggered (for this channel/PMT). Note that this is *not* the time of the first
ADC sample, since there are some recorded samples before the trigger (see
``NPreSamples``).

The time is marked as *valid* if the timestamp is synchronized with the ICM
clock, otherwise it is *invalid* (check this using ``HasValidTime``).

**ADCData**

The ADC counts for each sample recorded, e.g. the waveform. The sampling is
240 MHz. May be delta-compressed.

**BaselineValue**

The baseline counts of the ADC, which is calculated continuously by the FPGA
as a rolling sum. It can be marked as *valid* or *invalid*.

**NPreSamples**

The number of ADC samples recorded before the trigger (configurable).

**FEXResult**

Status of the on-board waveform extraction. Note that if the extraction is
successful, an ``I3XDOMHit`` object is returned instead of an
``I3DEggLaunch``.

**SelectionType**

Indicates if this waveform was selected by minimum bias.


I3mDOMLaunch
^^^^^^^^^^^^

:cpp:class:`I3mDOMLaunch` contains the output data from an mDOM readout, for a
single channel/PMT. It includes an instance of the common
:cpp:class:`I3XDOMLaunch` class. Note that this class is only returned when
on-board waveform extraction fails, otherwise :cpp:class:`I3XDOMHit` is
returned.

Members
-------

**Time**

The time (in nanoseconds) of the 120 MHz ADC bin in which the launch is
triggered (for this channel/PMT). Note that this is *not* the time of the first
ADC sample, since there are some recorded samples before the trigger (see
``NPreSamples``).

The time is marked as *valid* if the timestamp is synchronized with the ICM
clock, otherwise it is *invalid* (check this using ``HasValidTime``).

**ADCData**

The ADC counts for each sample recorded, e.g. the waveform. The sampling is
120 MHz. May be delta-compressed.

**BaselineValue**

The baseline counts of the ADC, which is calculated continuously by the FPGA
as a rolling sum. It can be marked as *valid* or *invalid*.

**NPreSamples**

The number of ADC samples recorded before the trigger (configurable).

**TOTHits**

A vector of :cpp:class:`TOTHit` instances gives a simplified representation of
the 960 MHz discriminator waveform. Each :cpp:class:`TOTHit` represents a
single *low-to-high* crossing of the discriminator threshold, and has two
members; ``Time`` (the start time of the discriminator waveform) and ``TOT``
(*time-over-threshold*, e.g. the time of the *low-to-high* threshold crossing
in the discriminator waveform).

**FEXResult**

Status of the on-board waveform extraction. Note that if the extraction is
successful, an ``I3XDOMHit`` object is returned instead of an
``I3mDOMLaunch``.

**SelectionType**

Indicates if this waveform was selected by minimum bias.

**TriggerType**

Indicates what caused the mDOM channel/PMT to trigger and create this launch.
Typically this is the *discriminator*, but *ADC threshold*, *software* or
*external* triggering are also options.


NoiseSuppressionRecord
^^^^^^^^^^^^^^^^^^^^^^

The high rates of radioactive decay noise in the mDOMs mean that not all mDOM
hit data can be transmitted to the surface. Events with two or more hits in a
given PMT channel within an exclusion time window, but without a corresponding
local-coincidence hit in a different PMT channel, are flagged as correlated
noise events. If a correlated noise event is detected, all subsequent hits
within the exclusion time window following the first hit are suppressed, and a
record is sent in the output data stream.

Members
-------

**Time**

The start time of the exclusion window.

**NNoiseHits**

The number of hits (more specifically, rising discriminator edges) within the
exclusion window (which are expected to be noise).

**ExclusionWindowSize**

The configured time window to exclude once the noise suppression condition is
triggered.

**LCWindowSize**

The configured time window within which two hits are considered to be in
coincidence.


I3LOMLaunch
^^^^^^^^^^^^

:cpp:class:`I3LOMLaunch` contains the output data from the LOM, for a single 
channel/PMT. This dataclass is used regardless of the LOM variant, e.g. 
LOM-16 vs LOM-18.

**Important:** This is a very preliminary implementation acting as a 
placeholder, as the LOM readout electronics are not yet fully developed. 
Expect this dataclass to change substantially over time.  

Members
-------

**Time**

The time (in nanoseconds) of the 60 MHz ADC bin in which the launch is
triggered (for this channel/PMT). Note that this is *not* the time of the 
first ADC sample, since there are some recorded samples before the trigger.

The time is marked as *valid* if the timestamp is synchronized with the 
ICM clock, otherwise it is *invalid* (check this using ``HasValidTime``).

The readout is triggered by an analog circuit if the high gain channel 
exceeds  a threshold voltage equivalent to 0.2 photoelectrons.


**ADCDataHighGain/LowGain**

The ADC counts for each sample recorded, e.g. the waveforms. There are
two distinct channels, one with high and one with low gain, each returning 
a waveform. The two waveforms always have the same length, typically approx. 
30 samples (5 of which are before the trigger) but can be longer for 
large/complex signals, and are concurrent (e.g. triggered at the same time). 
The sampling rate is 60 MHz. 

The high gain channel is set such that it can observe single photoelectrons, 
whilst the low gain channel is suited to much brighter events that would 
saturate the high gain channel. The low gain chain has opposite polarity to 
the high gain channel.
