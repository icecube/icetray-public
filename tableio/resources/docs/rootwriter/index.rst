..
.. Copyright  (C) 2010 The Icecube Collaboration
.. SPDX-License-Identifier: BSD-2-Clause
..
.. @author Fabian Kislat <fabian.kislat@desy.de>, $LastChangedBy$

.. highlight:: pycon

.. _tableio/rootwriter:

tableio/rootwriter
==================

A tableio backend for writing root files.
Authors: Fabian Kislat, Giuliano Maggi.
Current maintainer: Katherine Rawlins <krawlins@alaska.edu>.

.. toctree::
   :maxdepth: 1

   release_notes

.. toctree::

   I3ROOTTableService
   root_trees

Usage
^^^^^

This page summarizes the usage of the rootwriter tableio backend. For more
details on tableio, please read :ref:`the tableio documentation <tableio-main>`.

The tableio/rootwriter project supplies a tableio :class:`I3TableService` and an :class:`I3Table`
implementation to export data from IceTray to root trees. Tableio converters
are used to convert the objects in the :class:`I3Frame`.

The easiest way to use ``tableio/rootwriter`` is to add an :class:`I3ROOTWriter` segment
to you tray. It accepts the same parameters as :class:`I3TableWriter` plus a
parameter for the output file name::

    from icecube.tableio import I3ROOTWriter

    tray.AddSegment(I3ROOTWriter, 'rootwriter',
                    Output = '/SomePath/outputfilename.root',
		    SubEventStreams = ['in_ice','ice_top'],
                    Keys = ['MPEFit','SPEFit2'])

* SubEventStreams: What kind of events you want to book.
* Output: The output root file.
* Keys: This list has to contain all the objects that you want to book.
* BookEverything: This is another parameter that you can pass to
  I3ROOTWriter. Default is False, if you set it as True, you would book
  everything creating a very large file. We would suggest: Do not do that.


For more advanced usage, for instance to change the name of the ``MasterTree``
(see :doc:`root_trees` to learn what that is), construct an
:class:`I3ROOTTableService` object in the steering file::

    from icecube.tableio import I3TableWriter
    from icecube.tableio import I3ROOTTableService

    rootout = I3ROOTTableService(outputfilename)

The first parameter of the constructor is the filename of the output file.
For a documentation of the other (optional) parameters, read the
documentation of :cpp:class:`I3ROOTTableService`.

Once created you can pass this object as the ``tableservice`` parameter to
an :class:`I3TableWriter`::

    tray.AddModule(I3TableWriter, "recowriter",
                   tableservice = rootout,
		   ...
		   )

The ROOT files created by rootwriter contain a :cpp:class:`TTree` for each
frame object. The trees themselves have an entirely flat structure.
All trees are friends of a master tree, which by default is called
``MasterTree``. Using this master tree, individual branches can be
referred to as branches of this master tree, under the name
``tree_name.branch_name``.

For more information on how to use trees created by ``rootwriter``, see
:doc:`root_trees`.


Automatic file splitting
^^^^^^^^^^^^^^^^^^^^^^^^

ROOT has the feature to automatically split files when they reach a certain
size. In rootwriter this size is set to 1TB by default, effectively disabling
the file splitting. Users can change this value by calling::

    icecube.tableio.setMaxTreeSize(maxFileSizeInMB)

in python to set the maximum size to maxFileSizeInMB, given in units of MiB.
One should be aware, however, that file splitting in rootwriter is considered
experimental and might cause unexpected problems when reading the output files.
Also, rootwriter cannot guarantee that all trees have the same number of
entries in all files. Especially, all entries of the master tree will be in the
last file. This means that multiple files created by file splitting can only be
read at once using a TChain.
