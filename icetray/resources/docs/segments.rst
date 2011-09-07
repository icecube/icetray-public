Tray segments
=============

In addition to services and modules, IceTray has a concept of a tray segment, which is an automatic configuration function that adds one or more modules or services to an existing tray. This is designed to reduce configuration duplication and simplify scripts in several cases:

1. Single modules with an alternate set of defaults for some specific use
2. Logically connected sets of services and modules that together perform a single task (e.g. Gulliver-based reconstructions and pulse extraction with NFE)
3. Large segments of standard processing (e.g. modularization of bulk filtering and processing scripts)

Tray segments are defined by a python function, such as the following:

  @icetray.tragsegment
  def small_segment(tray, name, NDumps=1): 
    for i in range(0, NDumps):
      tray.AddModule('Dump', name + ('_dump_%d' % i))

This can then be added to a tray by doing:

  tray.AddSegment(small_segment, 'dumps', NDumps=4)

For tasks connected to a single module, relevant standard configurations should be added to the module's python directory.

Case 1 above (modules with alternate defaults), although still a tray segment, can be handled very quickly using :func:`icetray.module_altconfig`. For example, adding an alternate configuration for I3WaveCalibrator to be used with DOMSimulator-based simulation can be done as follows:

  DOMSimulatorCalibrator = icetray.module_altconfig('I3WaveCalibrator',
    DOMsimulatorWorkArounds=True, FADCSaturationMargin=1) 

This can then be added to a tray using AddSegment in the same way (or AddConfig, which is a synonym):

  tray.AddSegment(WaveCalibrator.DOMSimulatorCalibrator, 'calibrator')

When using :func:`icetray.module_altconfig`, those parameters specified in the arguments to :func:`icetray.module_altconfig` override the module's defaults. These can be overridden again in AddSegment and any other arguments to the original module can also be specified there.
