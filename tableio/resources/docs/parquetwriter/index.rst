..
.. Copyright  (C) 2025 The Icecube Collaboration
.. SPDX-License-Identifier: BSD-2-Clause
..
.. $Id$
..
.. @version $Revision$
.. @date $LastChangedDate$
.. @author Aurora Wohlfahrt <lepeler@icecube.wisc.edu>, $LastChangedBy$

tableio/parquetwriter
=====================

This is a writer service for :ref:`tableio` to create Apache Parquet files.
See below for a list of supported compression algorithms.
Please refer to the :ref:`tableio` docs for a comprehensive user's guide.

example script:
---------------
.. code-block:: python

    from icecube import icetray, dataio
    from icecube.icetray import I3Tray
    from icecube.tableio import I3TableWriter
    from icecube.tableio import I3ParquetTableService

    tray = I3Tray()
    tray.AddModule('I3Reader',
                filename = 'your_input_file.i3')

    writer = I3ParquetTableService(folder_path = 'your_output_folder', compression = 'chosen compression type')
    tray.AddModule(I3TableWriter,
                'writer',
                TableService = writer,
                Keys = ['I3MCTree'],
                SubEventStreams = ['InIceSplit', 'NullSplit'],
                )

    tray.Execute()
    tray.Finish()


compression types:
------------------
.. list-table::
    * - 'brotli' | '.br'
    * - 'bz2' | '.bz2'
    * - 'gzip' | '.gz'
    * - 'lz4' | '.lz4'
    * - 'lz4_frame'
    * - 'lz4_hadoop'
    * - 'lz0' | '.lz0'
    * - 'snappy' | '.sz'
    * - 'uncompressed' | ''
    * - 'zstd' | '.zst'


