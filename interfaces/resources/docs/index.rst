.. _interfaces:

Interfaces Documentation
========================
The interfaces project is designed to be a holder for generic 
interfaces between IceTray projects.  It should help minimize 
project inter-dependencies.

The project consists of the following services, modules, and templates:

* I3GeometryService - Abstract base class for serving up a geometry given an I3Time.
* I3CalibrationService - Abstract base class for serving up calibrations given an I3Time.
* I3DetectorStatusService - Abstract base class for serving up detector status given an I3Time.
* I3MetaService - Abstract base class for anything that returns frame pointers.
* I3EventService - Abstract base class for services that serve up events.
* I3OMKey2MBID - Abstract base class for services that map between OMKeys and MBIDs (DOM MainBoard IDs).
* I3IcePickModule - Module template for using I3IcePicks.
* I3IceForkModule - Module template for I3IcePicks that push frames into either the TrueBox or FalseBox, depending on whether the frame passed or not.

Note :  This project has no tests and are probably not appropriate
anyway.  This project consists of 7 abstract base classes and 2
templates.  Tests of the base classes would have to provide derived
implementations and the tests would therefore mostly test the 
test code, which is obviously pointless.

.. toctree::
   :maxdepth: 3

   release_notes
