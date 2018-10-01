.. _tray_segments:

Tray segments
=============

In addition to services and modules, IceTray has a concept of a tray 
segment, which is an automatic configuration function that adds 
one or more modules, services and/or other segments to a
I3Tray. These segments would be defined by module experts, live
within the project source tree and be released with project and
meta-projects.  This is designed to reduce configuration duplication 
(and remove the need to cut-paste from other scripts) and to simplify 
scripts in several cases (by building massive data processing 
scripts from segments of segments).  With this in mind, segments provide a way to:

1. Logically connect sets of services and modules that together perform a single task (e.g. Gulliver-based reconstructions or pulse extraction with NFE)
2. Build large segments of standard processing (e.g. modularization of bulk filtering and processing scripts)
3. Support modules with widely used alternate set of defaults for some specific use (e.g. when different settings are required for real data vs MC data)

.. note::

  When creating a segment for use in production, adhere to the 
  "Give one entity, one cohesive responsibility." standard.  
  We've seen several cases of segments taking on too much responsibilty.


A Basic Tray Segment Example
""""""""""""""""""""""""""""

Tray segments are defined by a python function, such as the following::

  @icetray.traysegment
  def small_segment(tray, name, NDumps=1): 
    for i in range(0, NDumps):
      tray.AddModule('Dump', name + ('_dump_%d' % i))

This can then be added to a tray by using the following in your python scripts::

  tray.AddSegment(small_segment, 'dumps', NDumps=4)

A more realistic example from the payload-parsing project::

  @icetray.traysegment
  def I3DOMLaunchExtractor(tray, name, OMKey2MBIDXML=os.path.expandvars("$I3_SRC") + "/phys-services/resources/mainboard_ids.xml", BufferID="I3DAQData", ExceptionId="I3DAQDecodeException"):
      if OMKey2MBIDXML != None:
         tray.AddService("I3XMLOMKey2MBIDFactory", name + "_OmKey2MbId", InFile=OMKey2MBIDXML)
      tray.AddService("I3PayloadParsingEventDecoderFactory", name + "_EventDecoder")
      tray.AddModule("I3FrameBufferDecode", name + "_fbdecoder", BufferID=BufferID, ExceptionId=ExceptionId)

The decorator (@icetray.traysegment) marks the function as a tray segment, allowing it to be found later (for example, by I3Tray and icetray-inspect). Note that this also allows you to have multiple tray segments in one file. The arguments of the segment are specified as keywords (e.g. OMKey2MBIDXML), with default values following the = sign. This segment can be added to your tray like so::

  tray.AddSegment(payload_parsing.I3DOMLaunchExtractor, "extract")


How to find Tray Segments
"""""""""""""""""""""""""

For tasks connected to a single module, relevant standard configurations 
should be added to the module's python directory (more details below).

Since tray segments are not stored in a single place but
distributed throughout the source projects, finding them could be
an issue.  To address this, icetray-inspect also reports all 
available tray segments as well::

  blaufuss@squeak[build]% icetray-inspect --no-modules --no-services payload-parsing
   -----------------------------------------------------------------------------
   icecube.payload_parsing.I3DOMLaunchExtractor (I3Tray segment) 
 
     Extract DOMLaunches and triggers from raw DAQ data.
 
   Parameters:
     BufferID
       Description : Name of the raw data buffer in the frame.
       Default     : 'I3DAQData'
 
     ExceptionId
       Description : Name of the boolean to put in the frame when an error occurs.
       Default     : 'I3DAQDecodeException'
 
     OMKey2MBIDXML
       Description : Path to XML file mapping mainboard IDs to OMKeys.
       Default     : '/data/i3home/blaufuss/icework/offline-software/trunk/src/phys-services/resources/mainboard_ids.xml'
 
   -----------------------------------------------------------------------------

You can get a list of all available tray-segments from icetray-inspect::

   blaufuss@squeak[build]% icetray-inspect --no-modules --no-services -a ./lib


Alternative configurations in Segments
""""""""""""""""""""""""""""""""""""""

For case 3 above (modules with alternate defaults), although still 
a tray segment, can be handled very quickly using :func:`icetray.module_altconfig`. 
For example, adding an alternate configuration for I3WaveCalibrator to be 
used with DOMSimulator-based simulation can be done as follows::

  DOMSimulatorCalibrator = icetray.module_altconfig('I3WaveCalibrator',
    DOMsimulatorWorkArounds=True, FADCSaturationMargin=1) 

This can then be added to a tray using AddSegment in the same 
way::

  tray.AddSegment(WaveCalibrator.DOMSimulatorCalibrator, 'calibrator')

When using :func:`icetray.module_altconfig`, those parameters specified in the arguments to :func:`icetray.module_altconfig` override the module's defaults. These can be overridden again in AddSegment and any other arguments to the original module can also be specified there.

These alterative configurations also are reported by icetray-inspect::

  blaufuss@squeak[build_deb(I3)]% icetray-inspect --no-modules --no-services WaveCalibrator
  *** WaveCalibrator ***
  -------------------------------------------------------------------------------
      icecube.WaveCalibrator.DOMSimulatorCalibrator (I3Tray segment)
  
      Alternate configuration for I3WaveCalibrator
      
      Overridden defaults:
      
      DOMsimulatorWorkArounds=True 
      FADCSaturationMargin=1


Writing a Segment for your Modules
""""""""""""""""""""""""""""""""""

Segments can be put in any location accessible to python, but those that are part of Icetray projects typically live in the module's pybdindings. They are then imported from the module's namespace.

Modules without any python component will need to add a PYTHON_DIR directive to their project's CMakeLists.txt::

  i3_project(myproject PYTHON_DIR python)

and an `__init__.py` file to a new python subdirectory like this::

  from icecube import icetray
  import os
  icetray.load('myproject', False)

Short segments can be added directly to this file, but longer ones should be added to another .py file, and then brought into the project's namespace with an import command in `__init__.py`.

There is one additional variant of the icetray.traysegment decorator that can be useful in some circumstances. This is an example from the hdfwriter project::

  @icetray.traysegment_inherit(tableio.I3TableWriter)
  def I3HDFWriter(tray, name, Output=None, **kwargs):
        """Tabulate data to an HDF5 file.

        :param Output: Path to output file
        """
        tabler = I3HDFTableService(Output)
        tray.AddModule(tableio.I3TableWriter, name, TableService=tabler,
            **kwargs)

This is a wrapper around tableio.I3TableWriter that adds one additional service. Any options besides `Output` are passed through to tableio.I3TableWriter via the kwargs parameter, but what those available options are would not ordinarily show up in the output of icetray-inspect. The use of traysegment_inherit here makes no functional changes, but causes the options taken by tableio.I3TableWriter to be appended to the segment's own options (in this case, `Output`) when shown in icetray-inspect.

Segments of Segments
""""""""""""""""""""

Segments can of course include tray.AddSegment() calls within them, and in this way large processing chains can be built up. A working example from the IC86 L2 processing::

  @icetray.traysegment
  def RawDataToPulses(tray, name, superdstname = 'I3SuperDST',
    pulses='UncleanedPulses'):
       # Raw Data
       tray.AddSegment(payload_parsing.I3DOMLaunchExtractor, name + '_launches')
       tray.AddModule('I3WaveCalibrator', name + '_wavecal')
       tray.AddModule('I3Wavedeform', name + '_wavedeform', Output=pulses)
       tray.AddModule('Delete', name + '_delete', Keys=['CalibratedWaveforms'])

       # Super DST
       if superdstname != None:
          tray.AddModule('Rename', name + '_sdstrename',
            Keys=[superdstname, pulses], If=lambda fr: pulses not in fr)

For common processing chains like the L2 processing, such a segment would typically live in a pure python project. For small personal projects, they can live in any python file (including in the script from which they are being used).

Expanding segments in the I3Tray
""""""""""""""""""""""""""""""""

There are several ways to see what is inside of a segment. The simplest is using the --expand-segments argument to icetray-inspect::

  [nwhitehorn@wanderer ~/i3/offline/build]$ icetray-inspect --expand-segments --no-modules --no-services payload-parsing

  *** payload-parsing ***
  -------------------------------------------------------------------------------
    icecube.payload_parsing.I3DOMLaunchExtractor (I3Tray segment)

      Extract DOMLaunches and triggers from raw DAQ data.
    
    
    
      Extra keyword arguments are passed to I3PayloadParsingEventDecoderFactory.

    Equivalent to:
      AddService('I3XMLOMKey2MBIDFactory', 'example_OmKey2MbId', InFile='/home/nwhitehorn/i3/offline/src/phys-services/resources/mainboard_ids.xml.gz')
      AddService('I3PayloadParsingEventDecoderFactory', 'example_EventDecoder')
      AddModule('I3FrameBufferDecode', 'example_fbdecoder', BufferID='I3DAQData', ExceptionId='I3DAQDecodeException')


It is also possible to print out the contents of an I3Tray or TrayInfo object using the Python `print` operator to get the contents and configuration of the entire tray, with all segments expanded, in a human-readable form. The Python `repr` operator can also be used to get a more-tractable (and potentially executable) version of a tray or TrayInfo frame::

  print repr(tray)

gives::

  tray = I3Tray.I3Tray()
  tray.AddModule('I3Reader', 'reader', DropBuffers=False,
    Filename='/tmp/bork.i3', FilenameList=[], SkipKeys=[])
  tray.AddModule('Dump', 'dump', IcePickServiceKey='', If=None)
  tray.AddModule('I3Writer', 'writer', CompressionLevel=-2,
    DropOrphanStreams=[], Filename='bork-out.i3', IcePickServiceKey='',
    If=None, SkipKeys=[], Streams=[])

You can also, of course, read the source code for the segment.
