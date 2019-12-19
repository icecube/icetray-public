.. _phys-services:

Phys-Services Documentation
===========================
The phys-services project is a collection of IceTray services and other
utilities common to all or most of icetray-based software.

.. toctree::
   :maxdepth: 1
   
   spe_fit_injector
   random_service
   distributed_rng
   geo_selector
   omkey2mbid
   release_notes


Overview
--------
* :cpp:class:`I3XMLOMKey2MBIDFactory`
* :cpp:class:`I3VEMCalManipulator`
* :cpp:class:`I3VEMCalManipulatorFactory`
* :cpp:class:`I3SummaryService`
* :cpp:class:`I3GSLRandomService`
* :cpp:class:`I3Splitter`
* :cpp:class:`I3ScaleCalculator`
* :cpp:class:`I3RandomService`
* :cpp:class:`I3FileOMKey2MBIDFactory`
* :cpp:class:`I3CutValuesBase`
* :cpp:class:`I3CascadeCutValues`
* :cpp:class:`I3CutValues`
* :cpp:class:`I3XMLOMKey2MBID`
* :cpp:class:`I3Cuts`
* :cpp:class:`I3SPRNGRandomService`
* :cpp:class:`I3EventCounter`
* :cpp:class:`I3GSLRandomServiceFactory`
* :cpp:class:`I3FileOMKey2MBID`
* :cpp:class:`I3SPRNGRandomServiceFactory`
* :cpp:class:`I3BadDOMAuditor`
* :cpp:class:`I3EventCounter`
* :cpp:class:`I3GCDAuditor`
* :cpp:class:`I3GeometryDecomposer`
* :cpp:class:`I3NullSplitter`
* :cpp:class:`I3OrphanQDropper`
* :cpp:class:`I3SplitTriggerSelector`

Source
%%%%%%
* :cpp:class:`I3TextFileGeometryServiceFactory`
* :cpp:class:`I3GCDFileGeometryService`
* :cpp:class:`I3GCDFileCalibrationService`
* :cpp:class:`I3GCDFileDetectorStatusService`
* :cpp:class:`I3GCDFileServiceFactory`
* :cpp:class:`I3TextFileGeometryService`

Surfaces
%%%%%%%%
See `doxygen <../../doxygen/phys-services/index.html>`__ docs.

Geometry Selectors
%%%%%%%%%%%%%%%%%%
* :cpp:class:`I3StringAdderService`
* :cpp:class:`I3StringAdderServiceFactory`
* :cpp:class:`I3GeometrySelectorServiceFactory`
* :cpp:class:`I3GeometrySelectorService`

Also see the `doxygen <../../doxygen/phys-services/index.html>`__ docs.
   
I3NullSplitter
^^^^^^^^^^^^^^
I3Nullsplitter is the trivial event splitter; for each Q frame, it emits a
single P frame containing nothing but an I3EventHeader with the SubEventStream
set to the name of the module. This can be useful for passing untriggered or
generator-level data to segments like I3HDFWriter that expect to operate on P
frames.

