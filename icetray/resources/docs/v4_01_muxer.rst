===================================
 I3Muxer / I3ReaderService removed
===================================

After initial deprecation in Icetray v3, I3Muxer and I3ReaderService have been
removed in Icetray v4, replaced by several new classes. The simple uses of
I3Muxer can be replaced by I3Reader. For example,
::

 tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("FilenameList", file_list)
    )
 tray.AddModule("I3Muxer","muxme")

can be replaced by
::

 tray.AddModule("I3Reader", "reader", FilenameList=file_list)

The typical case in simulation:
::

 tray.AddService("I3ReaderServiceFactory","reader")(
    ("omitevent",True),
    ("filename",gcdfile),
    )
 tray.AddModule("I3Muxer","muxme")

can be replaced by
::

 tray.AddModule("I3InfiniteSource", "somanyevents", Prefix=gcdfile)

Remaining uses of I3Muxer (e.g. generating GCD files from the database) can be handled using I3MetaSynth and I3Reader. For example: 
::

 [...]
 tray.AddService("I3ReaderServiceFactory","readerfactory")(
    ("FilenameList", file_list)
    )
 tray.AddModule( "I3Muxer", "Muxme" ) (
    ( "GeometryService",            "I3GeometryService" ),
    ( "CalibrationService",         "I3CalibrationService" ),
    ( "DetectorStatusService",      "I3DetectorStatusService" )
    ( "EventService",      "I3EventService" )     # Default
    )

can be replaced by
::

 [...]
 tray.AddModule("I3Reader", "reader", FilenameList=file_list)
 tray.AddModule( "I3MetaSynth", "challengeassumptions",
        GeometryService= "I3GeometryService",
        CalibrationService= "I3CalibrationService",
        DetectorStatusService= "I3DetectorStatusService"
        )
