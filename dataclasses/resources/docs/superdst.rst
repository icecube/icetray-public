.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. @version $Revision: 66546 $
.. @date $LastChangedDate: 2010-08-31 13:09:44 -0500 (Tue, 31 Aug 2010) $
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: kislat $

.. highlight:: python

.. _superdst-main:

SuperDST
=================

A (lossy) compression format for trigger-level data.

Rationale
^^^^^^^^^^^

Certain types of analyses may be able to benefit by trading data fidelity for
higher statistics. The most extreme example of this is the cosmic-ray anisotropy
analysis using DST data, where everything but the coarsely-binned result of the
online analysis is discarded before transfer to the North. This project defines a
(lossy) compression format for the readout data itself, so that slightly more
complicated processing and analysis can still be done after the fact.

The Pole filter starts with an event record ("payload") provided by the DAQ. 
The record is an encoded bytestring that consists of an event header, a list of 
hit records, and a list of trigger records. The event header contains the 
run/event number and event start and end times, while each hit records contains 
a timestamp and one or more digitized waveforms from a DOM-level trigger 
(launch). Each trigger record contains a type, source, and configuration ID, as 
well as a start and end time and the indices of the hits participating in the 
trigger. Once in IceTray, the payload is decoded into an I3EventHeader, 
separate I3DOMLaunchSeriesMaps for IceTop and InIce detectors, and an 
I3TriggerHierarchy. The I3DOMLaunches are processed further into I3RecoPulses, 
which for the current IceTop processing (tpx) represent the leading-edge time 
of the PMT pulse and its integrated charge, and for InIce (wavedeform) 
represent the arrival time of a group of photons at the photocathode. Each 
IceTop waveform is represented by a single I3RecoPulse. While the number of 
I3RecoPulses needed to represent an InIce waveform depends on the complexity of 
the waveform, the single-PE waveforms that make up the vast majority of InIce 
data can be represented by a single pulse.

Prior to the 2011 Pole season, the Pole filter calibrated the raw waveforms and 
processed them into RecoPulses, but then discarded the RecoPulses and sent the 
encoded DAQ payload to the North, where low-level processing was repeated. 
SuperDST takes a different approach, sending a compressed representation of the 
RecoPulses instead of the full DAQ records for those launches that are 
adequately represented by a small number of RecoPulses, resulting in 
significant satellite bandwidth savings.

Using SuperDST
^^^^^^^^^^^^^^^^

:cpp:class:`I3SuperDST` and :cpp:class:`I3SuperDSTTriggerSeries` are 
effectively compactly serialized versions of :cpp:class:`I3RecoPulseSeriesMap` 
and :cpp:class:`I3TriggerHierarchy`, respectively. They can be implicitly 
converted to their conventional representation by :cpp:class:`I3Frame`, so 
client code doesn't have to know whether a frame object uses SuperDST 
serialization or not.

C++
-----

.. highlight:: cpp

C++ code (e.g. in an :cpp:class:`I3Module`) can treat 
:cpp:class:`I3SuperDST` objects as if they were 
:cpp:class:`I3RecoPulseSeriesMaps`. The following code snippet works the same 
whether the frame object "I3SuperDST" is an :cpp:class:`I3RecoPulseSeriesMap` 
or :cpp:class:`I3SuperDST` (or, in fact, :cpp:class:`I3RecoPulseSeriesMapMask`, 
:cpp:class:`I3RecoPulseSeriesMapUnion`, or :cpp:class:`I3RecoHitSeriesMap`)::
    
    I3RecoPulseSeriesMapConstPtr pulses = frame->Get<I3RecoPulseSeriesMapConstPtr>("I3SuperDST");

:cpp:class:`I3TriggerHierarchy` and :cpp:class:`I3SuperDSTTriggerSeries` are similarly interchangeable::
    
    I3TriggerHierarchyConstPtr pulses = frame->Get<I3TriggerHierarchyConstPtr>("DSTTriggers");

.. note::
    
    The frame must also contain an :cpp:class:`I3DetectorStatus` for 
    :cpp:class:`I3SuperDSTTriggerSeries` decoding to work automatically as shown 
    above. The frame will contain an :cpp:class:`I3DetectorStatus` if the I3 file 
    being read has GCD frames.


Python
-------

.. highlight:: python

The same mechanism works in Python, albeit with slightly different syntax. To use the automatic decoding facility, replace::
    
    pulses = frame["I3SuperDST"]
    triggers = frame["DSTTriggers"]

with::
    
    pulses = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, "I3SuperDST")
    triggers = dataclasses.I3TriggerHierarchy.from_frame(frame, "DSTTriggers")

Format details
^^^^^^^^^^^^^^^^

I3SuperDST
------------

The I3SuperDST object contains a compressed representation of an 
I3RecoPulseSeriesMap. In the Pole filter, this contains all pulses from the 
InIce and IceTop detectors, merged into a single map.

Group headers
***************

Pulses in a single DOM are sorted into groups spaced closely in time. Each 
pulse is converted to a charge stamp that contains a discretized version of the 
pulse's charge and leading-edge time. The groups of charge stamps are ordered 
by start time; the time of a charge stamp represents the offset with respect to 
the previous charge stamp. This allows arbitrarily long events to be encoded 
efficiently.

Each group is represented on disk by a 2-byte header followed by one or more
2-byte charge stamps: 

.. list-table:: Group header: 2 bytes
    :widths: 15 20 5
    :header-rows: 1
    
    *   - Field
        - Discretization
        - Bits
    *   - DOM ID
        - String/OM
        - 13
    *   - Leading time
        - 1 ns
        - 3

The first 13 bits of the header are used to encode the string and OM number of the
DOM; the remaining 3 bits extend the range of the following 8-bit stamp time. In
this way, time differences between groups can be represented up to 2047 ns
[#fallback_timing]_, while time differences within groups must be less than 255
ns. Pulses more than 255 ns apart are split into separate groups.

Charge stamps: InIce
**********************

.. list-table:: InIce Charge stamp: 2 bytes
    :widths: 15 20 5
    :header-rows: 1

    *   - Field
        - Discretization
        - Bits
    *   - Time offset
        - 1 ns
        - 8
    *   - Charge
        - 0.05 PE
        - 6
    *   - HLC bit
        - bool
        - 1
    *   - Stop
        - bool
        - 1

The charge is discretized at 0.05 PE, allowing individual pulse charges to be 
represented up to 1.6 PE [#fallback_charge]_. The HLC bit marks whether the 
underlying pulse was extracted from a hard-local-coincidence DOM launch 
(containing a full ATWD+FADC readout) or a soft-local-coincidence launch 
(containing only select bins from the FADC for InIce DOMs, or the sum over the 
ATWD for IceTop). Finally, the stop bit indicates whether the group contains 
further charge stamps. This allows the group to contain as many charge stamps 
as can fit in the entire container.

Charge stamps: IceTop
***********************

IceTop charge stamps are stored in a slightly different format. In the current 
IceTop processing scheme there can be only one pulse per DOMLaunch, but the 
charges of the pulses can vary by many orders of magnitude. Accordingly, the 
charge is encoded logarithmically in 14 bits, with a step size of 
9.0/((1<<14)-1) in the base-10 logarithm of pulse charge.

.. list-table:: IceTop Charge stamp: 4 bytes
    :widths: 15 20 5
    :header-rows: 1

    *   - Field
        - Discretization
        - Bits
    *   - Time offset
        - 1 ns
        - 8
    *   - Charge
        - 0.001099 in log10(PE)
        - 14
    *   - HLC bit
        - bool
        - 1
    *   - Stop
        - bool
        - 1

Pulse widths
***************

Each charge stamp has an associated width, stored in 4 bits as the base-2 
logarithm of the pulse width in nanoseconds. The widths are stored in a 
run-length-coding scheme where N widths can be stored in 1+log2(N)/4 bytes in 
the best case (N identical widths) or 1+N bytes in the worst case (N different 
widths).

For InIce pulses the width gives the binning uncertainty in the time of the 
pulse, and storing this information in SuperDST preserves an order-of-magnitude 
estimate of this uncertainty. For IceTop the width is related to the pulse 
shape, and the SuperDST representation of the width adds a small amount of 
information versus storing only the pulse time and integrated charge.

In addition to streams of group headers and charge stamps, the serialized version
of an I3SuperDST must store the total number of headers, charge stamps, and 
widths. Each of these sizes is encoded as a variable-width integer, where 
values up to 247 are encoded in a single byte, and values between 248 and 
UINT64_MAX are encoded in 1+(log2(N)+1)/8 bytes.

This (along with some overhead) brings the typical footprint of an I3SuperDST 
object to 18 bytes + 5 bytes per encoded pulse.

.. list-table:: Total on-disk size of an I3SuperDST object
    :widths: 20 10 10 5
    :header-rows: 1

    *   - Purpose
        - Per group?
        - Per stamp?
        - Bytes
    *   - I3FrameObject overhead
        -
        -
        - 12
    *   - Group count
        - 
        -
        - 1--9
    *   - Stamp count
        - 
        -
        - 1--9
    *   - Width count
        - 
        -
        - 4--36
    *   - Group header
        - *
        -
        - 2
    *   - Charge stamp (InIce)
        -
        - *
        - 2+N
    *   - Charge stamp (IceTop)
        -
        - *
        - 4
    *   - Pulse width
        -
        - *
        - 1


.. [#fallback_timing] An overflow mechanism is used to represent gaps between 
    groups longer than 2047 ns. If, for example, the gap to be encoded is 34818 ns 
    (timecode 34818), the first 2047 ns (binary 11111111111) are subtracted and 
    packed into the group's timecode. The saturated value indicates that the next 2 
    bytes should be added to the timecode. The remainder (32771) is still too big 
    to fit in 16 bits, so 32768 (binary 1111111111111111) is subtracted and packed 
    into a 2-byte overflow block. The last remainder (3) is packed into a final 
    2-byte overflow block. When reading the stream, the decoder finds 2 saturated 
    codes (2047 and 32768) and one unsaturated code (3), and adds them to obtain 
    the original timecode (34818). This scheme allows the most common (small) 
    timecodes to be represented efficiently while still allowing for arbitrarily 
    large values.

.. [#fallback_charge] The same overflow mechanism used to represent long time gaps
    is used to represent pulse charges larger than 1.6 PE.

I3SuperDSTTriggerSeries
------------------------

The SuperDST family of tools also includes a compression format for trigger 
records. Each record contains the start time and length of the trigger (encoded 
variable width integers) as well as index of the :cpp:class:`TriggerKey` in 
:cpp:member:`I3DetectorStatus::triggerStatus`. The index is used during 
decoding to reconstruct the type, source and configuration ID of the trigger. 
Only "configured" triggers are stored; if needed, 
:cpp:class:`I3GlobalTriggerSim` can be used to insert global and throughput 
triggers to complete the trigger hierarchy.

.. list-table:: SuperDSTTrigger
    :widths: 15 20 5
    :header-rows: 1

    *   - Field
        - Discretization
        - Bits
    *   - TriggerKey index
        - 
        - 4
    *   - Start time
        - 1 ns
        - 12--76
    *   - Length
        - 1 ns
        - 8--72




