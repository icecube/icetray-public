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

1. Logically connected sets of services and modules that together perform a single task (e.g. Gulliver-based reconstructions or pulse extraction with NFE)
2. Build large segments of standard processing (e.g. modularization of bulk filtering and processing scripts)
3. Support modules with widely used alternate set of defaults for some specific use (e.g. when different settings are required for real data vs MC data)

A Basic Tray Segment Example
""""""""""""""""""""""""""""

Tray segments are defined by a python function, such as the following::

  @icetray.tragsegment
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

The decorator (@icetray.traysegment) marks the function 
as a tray segment, allowing it to be found later (for example, by I3Tray and 
icetray-inspect). Note that this also allows you to have multiple tray segments 
in one file. This segment can be added to
your tray like so::

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

TODO: Where they live, default values, keywords, etc

TODO:  include something about icetray.traysegment_inherit

Segments of Segments
""""""""""""""""""""

TODO: Putting it all together to make a large scripts suitable for mass processing

Expanding segments in the I3Tray
""""""""""""""""""""""""""""""""

TODO: How to mega-print

