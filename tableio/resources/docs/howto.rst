.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$


Booking data in IceTray scripts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The IceTray interface to the framework is provided by a service that manages
the actual output file and a module that shuffles data between the frame and
the service. A simple script could look something like this::

    from icecube import icetray, dataio
    from I3Tray import I3Tray
    from icecube.hdfwriter import I3HDFWriter
    
    tray = I3Tray()
    tray.AddModule('I3Reader',
                   filename = 'foo.i3.gz')
    
    tray.AddSegment(I3HDFWriter,
                   output="foo.hdf5",
                   keys=['LineFit','InIceRawData'],
                   SubEventStreams=['in_ice'],
                  )
    
    tray.Execute()
    

Here, the I3HDFTableService table_service is passed as a parameter to
I3TableWriter. The other parameter, 'keys', is a list of frame object
names. Any objects in the file named 'LineFit' will be written to a table
called 'LineFit' in the HDF5 file 'foo.hd5'.

Specifying objects by key
*************************

Objects can be specified by key. In addition, you may specify which converter should be used and what name the table should have in the output file. If you pass a list of keys::

    keys = ['LineFit','InIceRawData']

The frame objects called 'LineFit' and 'InIceRawData' will be written to the tables 'LineFit' and 'InIceRawData' using the default converters.

You can also pass a dictionary to specify which converters should be used::

    keys = {'LineFit': dataclasses.converters.I3ParticleConverter(), 
            'InIceRawData': dataclasses.converters.I3DOMLaunchSeriesMapConverter()}

For full control, you can pass a list of dicts, specifying the key, converter, and name of the output table::

    keys = [dict(key='LineFit', converter=dataclasses.converters.I3ParticleConverter(), name='SuperSpecialLineFit'),
            dict(key='InIceRawData', converter=dataclasses.converters.I3DOMLaunchSeriesMapConverter(), name='DOMLaunches')]



Specifying objects by type
**************************

If you don't know ahead of time which specific key-names you'd like to book, you can tell I3TableWriter to book all objects of a given type. Types are specified via their Python bindings::

    types = [dataclasses.I3Particle, dataclasses.I3DOMLaunchSeriesMap]

You can also specify converters to use for each type::

    types = {dataclasses.I3Particle: dataclasses.converters.I3ParticleConverter(),
             dataclasses.I3DOMLaunchSeriesMap: dataclasses.converters.I3DOMLaunchSeriesMapConverter()}

You can also pass a list of dicts specifying the type and converter::

    types = [dict(type=dataclasses.I3Particle, converter=dataclasses.converters.I3ParticleConverter(),
             dict(type=dataclasses.I3DOMLaunchSeriesMap, dataclasses.converters.I3DOMLaunchSeriesMapConverter())]

Specifying a default booker
***************************

You may want to use a special converter for some types, and default
converters for others.  You can specify that particular *keys* or
*types* should be processed with the default converter by passing in
``tableio.default``.

.. rubric:: Example

To use a custom converter named ``MyCustomConverter`` for the
``I3MCTree`` and the default/builtin converter for ``I3Particle``::

  types = { dataclasses.I3MCTree   : MyCustomConverter(),
            dataclasses.I3Particle : tableio.default }

similarly::

  keys = { 'LineFit' : tableio.default,
           'I3MCTree' : MyCustomConverter() }

Finding out which converters exist
**********************************

If you want to discover which converters are already defined, you can inspect
:data:`tableio.I3ConverterRegistry.registry` in ipython. This dictionary
contains all the converters tableio knows about. The I3ConverterRegistry is also used
by I3TableWriter to automatically find an appropriate converter. So your converter has
to be listed there in order to be used by I3TableWriter.

.. note::
    Please note that only those converters for which the library has been loaded are listed in the I3ConverterRegistry.
    This means that if you want to book a class I3Class that lives in library mylib, you have to do a :data:`from icecube import mylib`
    first. The reason is that the converters are residing in the pybindings of the libraries.

Booking everything in the file
*********************************

.. warning::
    You should really avoid doing this whenever possible, as it's likely to create very, very large files.

If you really want dump *absolutely everything* in an I3 file using the
default converters, you can set BookEverything to True::

    tray.AddModule(I3TableWriter,'writer', tableservice = table_service, BookEverything = True)

This gives you absolutely no control over how objects are booked. If you just
want to discover which converters are defined, you can inspect
:data:`tableio.I3ConverterRegistry.registry` in ipython. This dictionary
contains all the converters tableio knows about.

Booking to multiple files at once
************************************

You can also route output to multiple files in parallel by instantiating many
I3TableServices and passing them as a list to tableio.I3TableWriter::

    from icecube import icetray
    from I3Tray import I3Tray
    from icecube.tableio import I3TableWriter, I3CSVTableService
    from icecube.hdfwriter import I3HDFTableService
    from icecube.rootwriter import I3ROOTTableService
    
    tray = I3Tray()
    tray.AddModule('I3Reader',filename = 'foo.i3.gz')
    
    hdf = I3HDFTableService('foo.hd5')
    root = I3ROOTTableService('foo.root','master_tree')
    csv = I3CSVTableService('foo_csv')
    
    tray.AddModule(I3TableWriter,
                   tableservice = [hdf, root, csv],
                   keys         = ['LineFit','InIceRawData']
                  )
                  
    tray.Execute()
    

Booking from files with Q-frames
********************************

Using files that contain Q-frames, you have to know that there may be different 
SubEventStreams and that you have to tell I3TableWriter which one you want to book.
Otherwise, it will just produce empty files. If this happens, you should get a WARN 
message telling you that there were SubEventStreams present that were not booked.
Book the ones you want to write out using parameter *SubEventStreams*. E.g. you can do::

    tray.AddModule("I3NullSplitter", "fullevent")
    
    tray.AddSegment(HDFWriter,
       output="foo.hdf5",
       keys=["LineFit", "MPEFit"],
       SubEventStreams=["fullevent"],
       )

