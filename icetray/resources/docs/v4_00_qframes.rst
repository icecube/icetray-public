==========
 Q Frames
==========

Native extensions for handling of sub-events by adding a fifth canonical
stream type, called DAQ (for Data Acquisition). This frame type holds the
detector readout information (and simulation information) for a particular
detector global trigger. One or multiple views can be made into these data
by using a splitter module to produce Physics frames following the DAQ frame.
Using the mix-in mechanism for multiple streams, all the DAQ information (i.e.
the readouts) is available to reconstructions also in the subevent frames,
without duplication on disk. This lays the foundation for
introduction of other time-varying non-physics data (e.g. monitoring output
like mainboard temperatures) into the data stream.

The usual modules that operate on DAQ frames are calibration and low-level reconstruction modules (e.g. waveform calibration and pulse extraction), as well as
splitter modules that produce Physics frames that are views into the DAQ frame.
Since simulation is global data for a particular trigger, simulation modules
are also expected to operate on DAQ frames.

New Classes and Modules
^^^^^^^^^^^^^^^^^^^^^^^

QConverter
  Converts old-style Physics-only data into data that looks like it began as
  Q frames and passed through the null splitter. Reassigns frame keys based on
  name and type from the P frame in the original data into a new Q frame.

I3InfiniteSource
  Provides an infinite source of empty frames (DAQ frames, by default)
  optionally preceded by the contents of an I3 file (e.g. GCD data).

I3Splitter
  Base class for splitter modules. Handles low-level details like frame mixing
  and subevent ID assignment.

I3NullSplitter
  The simplest splitter module -- produces a P frame containing the entire
  event.

I3RecoPulseSeriesMapMask
  An object representing a subset of another I3RecoPulseSeriesMap.

I3PacketModule
  A base class for modules that want to operate atomically on "packets" of DAQ
  frames followed by all of the subevents into which they have been split.
  This is intended for use by algorithms that need to make global decisions
  (e.g. a module that joins several subevents together) based on the entire
  set of events. Note that thought is required before implementing such a
  module -- this is very rarely something you actually want to do.

Extended Classes
^^^^^^^^^^^^^^^^

I3EventHeader
  Additional fields for sub event stream (a string set to the instance name
  of the splitter module that produces this subevent) and subevent ID (unique
  per stream per event).

I3Frame
  Added additional stop type (DAQ).

I3Module
  Added additional stop type (DAQ).

I3Writer
  Add DropOrphanStreams option to remove parent frames with no children. For
  example, one may want to discard DAQ frames where all child physics events
  have been dropped; this can be done by setting
  ``DropOrphanStreams=[icetray.I3Frame.DAQ]``. A similar procedure can be
  used to drop redundant GCD frames.

