..
.. Copyright  (C) 2010 The Icecube Collaboration
.. SPDX-License-Identifier: BSD-2-Clause
..
.. $Id$
..
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy$

.. py:currentmodule:: icecube.hdfwriter

.. _hdfwriter:

hdfwriter
=========

A tableio plugin for writing HDF5 files.

.. toctree::
   :maxdepth: 1

   FAQ
   C++ API Reference </doxygen/hdfwriter/index>
   IceTray Inspect Reference </inspect/hdfwriter>
   release_notes

Usage
^^^^^

The :func:`.I3HDFWriter` segment provides output to HDF5 tables. Use it like so::

    from icecube import icetray, dataio, dataclasses
    from icecube.icetray import I3Tray
    from icecube.hdfwriter import I3HDFWriter

    tray = I3Tray()
    tray.Add('I3Reader', Filename='foo.i3.gz')

    tray.Add(I3HDFWriter,
        Output='foo.hd5',
        Keys=['LineFit','InIceRawData'],
        SubEventStreams=["InIceSplit"]
    )

    tray.Execute()

.. note::

  The ``Output`` parameter must be a unique file name. If two instances of
   :func:`.I3HDFWriter` try to write to the same file, an exception will be
   raised.

.. _hdfwriter-sim-hdfwriter:

I3SimHDFWriter
^^^^^^^^^^^^^^

For pre-trigger simulation files that contain only Q frames, use :func:`I3SimHDFWriter`, e.g.::

    from icecube.hdfwriter import I3SimHDFWriter

    tray.Add(I3SimHDFWriter,
        Output='foo.hd5',
        Keys=['MCMuon','I3MCPESeriesMap']
    )

Note that the ``SubEventStreams`` parameter is forbidden in this context.

Utilities
^^^^^^^^^

The hdfwriter project provides 3 utility scripts:

* **hdfwriter-merge** concatenates HDF5 tables and writes them to any of the
  supported output formats (hdf5, root, or csv)

* **hdfwriter-mix** creates HDF5 files containing the union of all tables in
  its input files, e.g. if file ``A.hdf5`` contains tables *FooFit* and
  *BarFit* and file ``B.hdf5`` contains *BazFit*, the command ``hdfwriter-mix
  A.hdf5 B.hdf5 -o C.hdf5`` will produce a file with tables *FooFit*, *BarFit*,
  and *BazFit*.

* **hdfwriter-book** is a simple wrapper around the :func:`.I3HDFWriter` snippet
  above.
