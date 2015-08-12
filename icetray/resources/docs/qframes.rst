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

The usual modules that operate on DAQ frames are calibration and low-level 
reconstruction modules (e.g. waveform calibration and pulse extraction), as well as
splitter modules that produce Physics frames that are views into the DAQ frame.
Since simulation is global data for a particular trigger, simulation modules
are also expected to operate on DAQ frames.

======================
 Working with Q Frames
======================

Migrating existing scripts to q-frames can require some updates to your 
existing scripts.  Several modules have been changed to run only
on DA(Q) frames, rather than (P)hysics frames.  Depending on
what your script does, and what your input data looks like,
you may not need any updates at all.

In preparing the Q frame release, here are some guidelines and hints
that we've come across that might help you transition more easily
to the Q frame release series.  Also included are some common 
errors and places to look for solutions.

If you are still stuck after all this, please bring your questions
and problems to the dataclasses mailing list or IRC:

  software@icecube.wisc.edu

Q Frames: the basics
^^^^^^^^^^^^^^^^^^^^

Q-frames are designed to hold all information from the DAQ global trigger
and readout, and anything derived directly from that, and for MC data,
and MC truth information.  The list of objects
(types and specific names) that are expected to live in the Q-frame includes

Specific names (QKeys):

* "CorsikaWeightMap"
* "I3MCTree"
* "I3MCTreeGen"
* "I3MCWeightDict"
* "CalibratedATWD"
* "CalibratedFADC"
* "I3DAQData"
* "InIceRawData"
* "IceTopRawData"

Specific types (QTypes):

* "I3EventHeader"
* "I3Map<OMKey, vector<I3Waveform> >"
* "I3Map<OMKey, vector<I3DOMLaunch> >"
* "I3Map<OMKey, vector<I3MCHit> >"
* "I3Map<OMKey, vector<I3RecoPulse> >"
* "I3Map<OMKey, I3PortiaPulse>"
* "I3PortiaEvent"
* "I3Tree<I3Trigger>"
* "JEBEventInfo"
* "I3Vector<I3MMCTrack>"


These are taken from the QConverter default lists.  You'll get these lists
if you do not override the default settings for the "QKeys" and "QTypes"
parameters.

Note: the FilterMask currently lives in the Q-frame, since
entire DAQ events are selected in the online-filter.

Other items, including per-event RecoPulseSeriesMasks (from hit
selection and cleaning), I3Particles, I3CutValues, and any
other per-physics-event items live in the P-frame.

But, given the "Mixed In" nature of the frames as seen by each module,
every P-frame stop will see all the Q-frame items.

Q Frames: Common usages
^^^^^^^^^^^^^^^^^^^^^^^
There are several common usages that are outlined here, each with
its own Q-frame specific advice.  

1. Reading data already Q-framed.  This case should not require any special
handling.  Data is already divided correctly between Q and P frames.  

2. Reading older L2 data.  This high-level data likely requires no
new low-level processing, but does not have Q-frames in the data file.
In this case, the basic QConverter module will add the needed
Q frames::

      tray.AddModule("QConverter", "qify", WritePFrame=True)

This should be placed right after the I3Reader.  Any files written
from out from this processing will keep these Q frames inline. See,
for an example::

      DOMcalibrator/resources/test/DOMcalibratorTest.py

3.  Reading low-level data (PFRaw or PFFilt files from Pole).
Depending on the level of processing you wish to do, you will need to use the
QConverter.  The QConverter has two modes, one where it issues P-frames for
each Q-frame and another where it maps the P-frames in the file to
Q-frames (saving any P-frame creation for another module (null or
advanced splitters for example).

To not create P-frames from each Q-frame, for example, to create input
files that you will later split, calibrate, etc, add::

   tray.AddModule("QConverter", "qify", WritePFrame=False)

Simple example of this is available::

   examples/resources/examples/make_testdata.py
   examples/resources/test/pass1.py
   daq-decode/resources/examples/dump-i3-online-data.py



Q Frames: Common problems
^^^^^^^^^^^^^^^^^^^^^^^^^
1. My script no longer does ANYTHING (or very close to it)

You are very likely trying to read older data (pre-Q frames) and
you are trying to do low-level processing (decoding, calibration
or feature extraction).  The I3Reader does not automatically convert
P frames into Q frames.  Try adding the QConverter to your script
after the I3Reader::

      tray.AddModule("QConverter", "qify", WritePFrame=True)

This will add a Q-frame ahead of each of your P-frames and keep
the appropriate items in the Q-frame.  

2. Script complains:

::

   RuntimeError: object in frame at "I3EventHeader" doesn't exist 
      or won't dynamic cast to type "I3EventHeader"

You're likely trying to use the I3NullSplitter on events that do not have
an I3EventHeader (undecoded DST only events, for example). 
An I3EventHeader is required for the I3NullSplitter to work.  
You could add a guard to make sure all events have an I3EventHeader::

    def data_check(frame):
    	if frame.Has("I3EventHeader"):
           return True
    	else:
           return False
    tray.AddModule(data_check,"daqthere",Streams=[icetray.I3Frame.DAQ])

3. "Remixing" is now built-in.  Early test versions of Q-frame software
might have required a "Remix" module to be added, to redistribute changes
in Q-frame objects to all daughter P-frames.  This has been added
to the PushFrame() call, and is done automatically, even if not
needed.

4. Care should be taken to cut or drop Q frames if they already have daughter
P-frames.  You should  make your Q-frame based selection
BEFORE any splitting.  There is an option in the I3Writer module
to remove any orphaned frames (like Q-frames where all daughter P-frames
have been cut)::

     tray.AddModule("I3Writer","writer",
                    DropOrphanStreams=[icetray.I3Frame.Calibration, icetray.I3Frame.DAQ])
                    filename = "TEST_DATA.i3",
               )	 


5. Ordering of modules in script.  Some care should be taken when ordering
the modules in your scripts.  You should generally follow the order:

  #. File reading, DB services (if needed), I3MetaSynth (if needed for
     DB services), and any data decoding.  Depending on your data,
     these may not be needed as they could already be done (L2 or higher 
     for example).  These steps generally create items in the all frame
     and especially the Q-frame.  If you are reading older data, you will
     likely need the QConverter (see #1)
  #. Event calibration and feature extraction.  These operate on the Q-frame
     items, calibrating all waveforms, and performing feature extraction,
     generating I3RecoPulseMaps, which also live in the Q-frame.  Depending 
     on your data, these may not be needed as they could already be 
     done (L2 or higher for example).
  #. Event splitting.  Now for each Q-frame, you may want to split
     Q-frames into 1 or more P-frames.  Options include the I3NullSplitter
     or more advanced splitters like the TTrigger.  Again, depending
     on your data, these may not be needed as they could already be 
     done (L2 or higher for example).
  #. Per P-frame hit-cleaning, reconstructions, analysis cuts, etc.  
     These higher level analysis generally will work as they have before, 
     only now, potentially many times per DA(Q) event.
