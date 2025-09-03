.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

Dataio Viewers
==============

dataio-shovel
-------------

The dataio-shovel is a .i3 file browser utility. It has interactive
help and can be handy to identify what is inside a .i3 file. Try it
out. Here comes the screenshot:

.. image:: Dataio_shovel.gif

The bottom part of the display shows a tape-like representation of the
icecube data stream. The G, C, D, Q, and P represent Geometry,
Calibration, DetectorStatus, DAQ, and Physics, respectively. The bar in the
middle is the location of the tape read head. On the top half are what
is currently under the read head: a 'frame'. Each row is one data
item. On the left is the 'key' the object is stored under, in the
middle is the type of the object (with typedefs stripped away), on the
right is the on-disk size of the object in bytes.

The up-down arrows select frame items and the 'x' key will display
that item in XML. The 'c' key will show the configuration of the tray
that generated the datafile, including all module parameters, svn
information, time of run, hostname, compiler and root versions, and
operating system type. Full help is available in the shovel itself.

