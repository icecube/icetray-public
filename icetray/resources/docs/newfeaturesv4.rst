===========
 New In v4
===========

.. toctree::
   :glob:

   v4_*
   segments


Introduction of DAQ frames
""""""""""""""""""""""""""

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

Pulse Series Masks
""""""""""""""""""

Subevent splitting and hit cleaning involve making multiple subevents with a
subset of the full pulse series. Any pulse series or object implicitly
convertable to a pulse series, including masks (see below) can have a subset
of its contents written by reference as a mask, vastly reducing disk space
requirements. This mask can be treated (below) by reconstructions directly
as an I3RecoPulseSeriesMap, requiring no software changes for readout.

Synthetic Pulse Series Support
""""""""""""""""""""""""""""""

Things that are "castable" to an I3RecoPulseSeriesMap in a generic sense can
now be retrieved from the frame by I3Frame::Get<I3RecoPulseSeriesMap>(). This
allows implicit conversion to an I3RecoPulseSeriesMap of pulse masks, SuperDST
data, and reco hits.

Service Enhancements
""""""""""""""""""""

I3ServiceBase imported from Gulliver, changes to I3Module::GetParameter() to
allow transparent use of named context services or direct python argument
services.

Miscellaneous Changes
"""""""""""""""""""""
- I3RecoHit support removed from most modules, mostly in phys-services
  - I3RecoHit still fully supported for reading from files
  - Auto conversion to I3RecoPulse available by the synthetic pulse support
- TWR and F2K projects removed from offline-software meta-project
- I3RecoPulse has new fields and new, lighter-weight serialization
- I3Waveform source information modified
- I3ConditionalModule's condition now operates on all frame types, not just P frames.
- C++ programs linking to libicetray should now call I3::init_icetray_lib()
  (from <icetray/init.h>) from their main().  This replaces a
  magical static library initialization step in earlier versions.

Tray Segment support
""""""""""""""""""""
IceTray has been expanded to include support for the concept of IceTray
"segments", which are pre-configured modules, services or sets of modules
and services, each properly configured and released as part of the project
that defines them.  These segments can easily be joined together to more
easily form larger segments, and large processing heavy scripts.

These segments allow for pre-defined "alternative" standard configurations
and are closely integrated into the standard icetray-tools (I3Tray, 
icetray-inspect, etc).  The TraySegment implementation was heavily
influenced by the work of the LE/IBEX framework.

