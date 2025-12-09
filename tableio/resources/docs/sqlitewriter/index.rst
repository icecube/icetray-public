..
.. Copyright  (C) 2025 The Icecube Collaboration
.. SPDX-License-Identifier: BSD-2-Clause
..
.. $Id$
..
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Aurora Wohlfahrt <lepeler@icecube.wisc.edu>, $LastChangedBy$

.. _tableio/sqlitewriter:

tableio/sqlitewriter
====================

This is a writer service for :ref:`tableio-main` to create `SQLite
<https://sqlite.org/>`_ databases. Please refer to the :ref:`tableio-main` docs
for a comprehensive user's guide.

example script:
---------------
.. code-block:: python

    from icecube import icetray, dataio
    from icecube.icetray import I3Tray
    from icecube.tableio import I3TableWriter
    from icecube.tableio import I3SQLiteTableService

    tray = I3Tray()
    tray.AddModule('I3Reader',
                   filename = 'your_input_file.i3')

    writer = I3SQLiteTableService(path = 'your_output_file.db')
    tray.AddModule(I3TableWriter,
                   'writer',
                   TableService = writer,
                   Keys = ['I3MCTree'],
                   SubEventStreams = ['InIceSplit', 'NullSplit'])

    tray.Execute()
    tray.Finish()
