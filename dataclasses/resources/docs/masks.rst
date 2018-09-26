===================================================
Selecting pulses: :class:`I3RecoPulseSeriesMapMask`
===================================================

Purpose
=======

IceCube events consist of a collection of photoelectron pulses reconstructed
from waveforms recorded in the DOMs. It is often desirable to consider only a
subset of the pulses in an event when calculating observables for an event.
This procedure is generally known as "pulse cleaning." Since many different
selection schemes exist, simply copying a subset of the pulse data to represent
the selection leads to much wasted storage space. The
:class:`I3RecoPulseSeriesMapMask` class exists to efficiently represent a pulse
selection. Internally, the class is just a bitmask with one entry for every
pulse in the parent pulse collection and a string storing the name of the
parent pulse collection in the frame.

Examples
========

Creation
--------

The easiest way to create a :class:`I3RecoPulseSeriesMapMask` is via a callback
function of the form

.. function:: callback(omkey, index, pulse)
	
	Return True if the pulse should be selected, otherwise False
	
	:param omkey: The :class:`OMKey` of the DOM that recorded the pulse
	:param index: The index of the current pulse within the series on the given DOM
	:param pulse: The :class:`I3RecoPulse` itself

Select the first pulse on every DOM::
	
	frame['FirstInIcePulses'] = dataclasses.I3RecoPulseSeriesMapMask(frame, 'InIcePulses', lambda omkey, index, pulse: index == 0)

Select only pulses from HLC launches::
	
	frame['InIcePulsesHLC'] = dataclasses.I3RecoPulseSeriesMapMask(frame, 'InIcePulses', lambda omkey, index, pulse: pulse.flags & pulse.LC)

Select only pulses from DeepCore strings::
	
	frame['DeepCorePulses'] = dataclasses.I3RecoPulseSeriesMapMask(frame, 'InIcePulses', lambda omkey, index, pulse: omkey.string > 78)

Manipulation
------------

:class:`I3RecoPulseSeriesMapMask` can be combined with logical operators.
Continuing the previous example, HLC pulses in DeepCore can be had by taking
the logical AND of the HLC and DeepCore masks::
	
	frame['DeepCorePulsesHLC'] = frame['InIcePulsesHLC'] & frame['DeepCorePulses']

Retrieving the underlying pulses
--------------------------------

:class:`I3RecoPulseSeriesMap` may be represented as
:class:`I3RecoPulseSeriesMap`, :class:`I3RecoPulseSeriesMapMask`, or as
unions of :class:`I3RecoPulseSeriesMap` from different sources. No matter what
the underlying representation, a compatible frame object can be converted to an :class:`I3RecoPulseSeriesMap` like this::
	
	dc_pulses = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, 'DeepCorePulses')
