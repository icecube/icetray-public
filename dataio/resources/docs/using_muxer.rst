Using the I3Muxer
===================

I3Muxer ("muxer" is short for "multiplexer") is an IceTray module which is responsible for arranging and selecting the right streams and sending out I3rames on the right stream.  The muxer accomplishes its job by looking at four services from which it gets the Geometry, Calibration, DetectorStatus and Event data.

Introduction
-------------
The idea is that the I3Muxer module has four interfaces available to it for getting the Geometry, the Calibration, the detector status and some Events. Those interfaces are the I3EventService, I3GeometryService, and I3CalibrationService and the I3DetectorStatusService. These four are abstract interfaces which allow the I3Muxer to decide in which order Frames should be issued and what data they should contain.

The idea is that when it comes time for a frame, the I3Muxer looks at each of these interfaces and looks at the time of the next event . If a geometry frame should be issued next, then it gets an I3Geometry from the interfaces and puts these in a frame.  Same too for DetectorStatus and Calibration.

The idea is to make it easy to extend different ways of getting frames. For a particular example, there would be one implementation of the I3GeometryService which gets one geometry for a file and gives it up. There would be another implementation which reads from the database. One also for reading from an f2k file.

The Interfaces
----------------
Here's a sketch of what the relavent interfaces look like:

The I3EventService interface has two methods.  The method I3EventService::MoreEvents() should return 'true' as long as there are more events to be filled.  The I3EventService::PopEvent method should fill 
up the given frame with event information and return the time of the event::

 class I3EventService
 {
  public:
   /**
    * @brief indicates whether or not there are more events to find.
    * 
    */
   virtual bool MoreEvents() = 0; 
 
   /**
    * @brief This method should fill the frame with all the relevent
    * frame data.  Returned is the time of the event.
    */
   virtual I3Time PopEvent(I3Frame& frame) = 0;
 };


The I3GeometryService, I3DetectorStatusService, and I3CalibrationService all basically look the same.  Here is the I3GeometryService for reference.  The one interface method I3GeometryService::GetGeometry(I3Time time) should 
return an I3Geometry corresponding to the time that it is given::


 class I3GeometryService
 {
  public:
   /**
    * @brief This is the default name of the service in an I3Context
    */
   static const char* DefaultName() { return "GeometryService";} 
 
   /**
    * @brief gives the I3Geometry and I3GeometryHeader for the
    * indicated time.
    */
   virtual I3GeometryConstPtr GetGeometry(I3Time time) = 0;
 };

Using the Muxer
----------------

To use the Muxer in an IceTray run, the person configuring IceTray needs to 

* Make sure that there is an implementation of each stream service installed
* Add the Muxer as the first module in the chain.

By way of an example, consider the following line::

 tray.AddService("I3TestDaqXMLFileStatusServiceFactory","status")

installs the I3TestDaqXMLFileStatusService which gets the DetectorStatus object from testdaq configuration file.

The following::

 tray.AddService("I3ReaderServiceFactory","i3reader")

installs all four stream interfaces, which are read from the provided .i3 file.

In both cases the same icetray module::

  tray.AddModule("I3Muxer","muxme")

is used.

Digging Around 
^^^^^^^^^^^^^^^

If you want to get into the nitty-gritty of how this whole thing works, I would suggest taking a look at the following files:

The Muxer itself:

* phys-services/public/phys-services/source/I3Muxer.h
* phys-services/private/phys-source/I3Muxer.cxx

The Stream Service interface definitions (now in project interfaces):

* interfaces/public/interfaces/I3EventService.h
* interfaces/public/interfaces/I3GeometryService.h
* interfaces/public/interfaces/I3CalibrationService.h
* interfaces/public/interfaces/I3DetectorStatusService.h

A very simple implementation of these services which just fills empty events:

* phys-services/public/phys-services/empty-streams/I3EmptyCalibrationService.h
* phys-services/public/phys-services/empty-streams/I3EmptyDetectorStatusService.h
* phys-services/public/phys-services/empty-streams/I3EmptyEventService.h
* phys-services/public/phys-services/empty-streams/I3EmptyGeometryService.h
* phys-services/public/phys-services/empty-streams/I3EmptyStreamsFactory.h
* phys-services/private/empty-streams/I3EmptyCalibrationService.cxx
* phys-services/private/empty-streams/I3EmptyDetectorStatusService.cxx
* phys-services/private/empty-streams/I3EmptyEventService.cxx
* phys-services/private/empty-streams/I3EmptyGeometryService.cxx
* phys-services/private/empty-streams/I3EmptyStreamsFactory.cxx

Two simple examples using the muxer:

* phys-services/resources/scripts/EmptySourceExample.py
* phys-services/resources/scripts/FileGeometryExample.py

