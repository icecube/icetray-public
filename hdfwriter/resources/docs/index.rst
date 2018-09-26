.. 
.. copyright  (C) 2010
.. The Icecube Collaboration
.. 
.. $Id$
.. 
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$

.. highlight:: pycon

.. _hdfwriter:

hdfwriter
=================

A tableio plugin for writing HDF5 files.

.. toctree::
   :maxdepth: 1

   FAQ
   release_notes

Usage
^^^^^^^^

:class:`I3HDFTableService` provides output to HDF5 tables. Use it like any other writer service::

    from icecube import icetray
    from I3Tray import I3Tray
    from icecube.tableio import I3TableWriter
    from icecube.hdfwriter import I3HDFTableService
    
    tray = I3Tray()
    tray.AddModule('I3Reader','reader',filename = 'foo.i3.gz')
    
    hdf = I3HDFTableService('foo.hd5')
    
    tray.AddModule(I3TableWriter,'writer',
                   tableservice = hdf,
                   keys         = ['LineFit','InIceRawData']
                  )
                  
    
    tray.Execute()
    

Reading with pytables
^^^^^^^^^^^^^^^^^^^^^^^^^^^

:class:`I3HDFTableService` creates a flat structure of tables under the file root. Since the HDF5 high-level Table API does not explicitly support units and docstrings, these are stored in the attributes FIELD_N_UNIT and FIELD_N_DOC for each field N. Support for reading these attributes is provided by the :mod:`hdfwriter.pytables_ext` module::

    In [1]: from icecube.hdfwriter import pytables_ext
    In [2]: import tables
    In [3]: f=tables.openFile('foo.hdf5')
    In [4]: f.root.LineFit.coldoc
    Out[4]: 
    ['run number',
     'event number',
     'object was found in the frame',
     'x-position of particle',
     'y-position of particle',
     'z-position of particle',
     'origin time of particle',
     'zenith angle of particle origin',
     'azimuthal angle of particle origin',
     'energy of particle',
     'particle speed',
     'length of track',
     '',
     '',
     '',
     '']
    In [5]: f.root.LineFit.colunits
    Out[5]: 
    ['',
     '',
     'bool',
     'm',
     'm',
     'm',
     'ns',
     'radian',
     'radian',
     'GeV',
     'Gm/s',
     'm',
     '',
     '',
     '',
     '']

Utilities
^^^^^^^^^

The hdfwriter project provides two utility scripts:

* **hdfwriter-merge** concatenates HDF5 tables and writes them to any of the
  supported output formats (hdf5, root, or csv)

* **hdfwriter-mix** creates HDF5 files containing the union of all tables in
  its input files, e.g. if file A.hdf5 contains tables *FooFit* and *BarFit* and file B.hdf5
  contains *BazFit*, `hdfwriter-mix A.hdf5 B.hdf5 -o C.hdf5` will produce a file with tables
  *FooFit*, *BarFit*, and *BazFit*.


