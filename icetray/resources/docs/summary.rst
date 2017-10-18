A History Lesson
================

Pre-2008
--------

Must "load" each library.  Parameter list of tuples.

python code::

    import sys
    from I3Tray import I3Tray
    load('libicetray')
    load('libdataclasses')
    load('libdataio')
    
    tray = I3Tray()
    tray.AddModule('I3Reader','reader')(('Filenamelist',sys.argv[1:]))
    tray.AddModule('Dump','dumper')
    tray.Execute()
    

Pre-2012
--------

Python bindings for every project.  Parameters as python arguments
(keyword or positional).

python code::
    
    import sys
    from I3Tray import I3Tray
    from icecube import icetray,dataclasses,dataio,phys_service
    
    tray = I3Tray()
    tray.AddService('I3GSLRandomServiceFactory,'gsl',Seed=42)
    tray.AddModule('I3Reader','reader',Filenamelist=sys.argv[1:])
    tray.AddModule('Dump','dumper')
    tray.Execute()
    

2013
----

Module names unnecessary. TrashCan module is added automatically.
Tray.Add figures out if it's a module or a service.

python code::
    
    import sys
    from I3Tray import I3Tray
    from icecube import icetray,dataclasses,dataio,phys_services
    
    tray = I3Tray()
    tray.Add('I3GSLRandomServiceFactory, Seed=42)
    tray.Add('I3Reader', Filenamelist=sys.argv[1:])
    tray.Add('Dump')
    tray.Execute()
    

2014
----

No need to call Finish after executing.  This is done automatically.

python code::
    
    import sys
    from I3Tray import I3Tray
    from icecube import icetray,dataclasses,dataio,phys_services
    
    tray = I3Tray()
    tray.Add('I3GSLRandomServiceFactory, Seed=42)
    tray.Add('I3Reader', Filenamelist=sys.argv[1:])
    tray.Add('Dump')
    tray.Execute()
   

IceTray Summary
=================

tray.Execute
------------

tray.Execute takes an optional argument for the number of frames to process::

    tray.Execute(10)
    
Module Inboxes and Outboxes
---------------------------

Modules have inboxes and outboxes.  The inbox accepts frames from the previous
module, and the outbox pushes frames to the next module.

IceTray Inspect
---------------

Searching: -R argument

Directory: -a directory

example::

    icetray-inspect -R 'I3GSL*' phys-services

    icetray-inspect -a lib/

Hyphen vs Underscore
--------------------

Some projects had hyphens in their names, but this violates python syntax.
Thus, these are turned into underscores in pybindings.

example::
    phys-services vs phys_services

Module Names
------------

String names are c++ modules.  Values are python modules.

c++ module::

    tray.Add('I3Reader')

python module::

    def cut(frame):
        return 'I3DAQData' in frame
    tray.Add(cut)
    
Python Modules
--------------

Python modules run only on physics frames.  Their return value is cast
to a bool, and used to decide whether to pass the current frame to the
next module.  False will drop the frame, while True will push the frame
to the next module.
    
Anonymous Modules
-----------------

Use lambdas in python to add an anonymous module.

function example::

    def cut(frame):
        return 'I3DAQData' in frame
    tray.Add(cut)

lambda example::

    tray.Add(lambda frame: 'I3DAQData' in frame)

Tray Segments
-------------

Segments can contain multiple I3Modules to help bundle code together.

Let's use a segment::
    
    from icecube import payload_parsing
    tray.Add(payload_parsing.I3DOMLaunchExtractor)
    
Writing a segment::

    from iceucbe import icetray
    @icetray.traysegment
    def MySegment(tray, name, arg1, If = lambda f:True, **kwargs):
        # we can use arg1 or the dict of kwargs
        tray.Add("Dump",If=If)
        
IceTray Inspect Segments
^^^^^^^^^^^^^^^^^^^^^^^^

IceTray Inspect can see segments too::

    icetray-inspect --expand-segments
    
        Equivalent to:
            AddService()
            AddModule()
            Addmodule()

