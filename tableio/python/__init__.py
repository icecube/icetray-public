# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# create the registry first, so that compiled bookers can be added
from icecube import icetray
from icecube.tableio.registry import I3ConverterRegistry

from icecube._tableio import (
    ConvertState, EnumMember, EnumMemberList, I3BroadcastTableService, I3CSVTableService, I3Converter,
    I3ConverterBundle, I3ConverterMill, I3Datatype, I3Table, I3TableRow, I3TableRowDescription, I3TableService,
    I3TableTranscriber, I3TableWriterWorker, vector_I3ConverterMillPtr,
    I3_USE_ROOT, I3_USE_HDF5, I3_USE_SQLITE3, I3_USE_ARROW
)

from icecube.tableio.I3TableWriterModule import I3TableWriter, default

if I3_USE_ROOT:
    from icecube._tableio import I3ROOTTableService

    @icetray.traysegment_inherit(I3TableWriter,
        removeopts=('TableService',))
    def I3ROOTWriter(tray, name, Output=None, **kwargs):
        """Tabulate data to a ROOT file.

        :param Output: Path to output file
        """

        if Output is None:
            raise ValueError("You must supply an output file name!")

        # Ready file for staging out if so configured
        if 'I3FileStager' in tray.context:
            stager = tray.context['I3FileStager']
            Output = stager.GetWriteablePath(Output)

        tabler = I3ROOTTableService(Output)
        tray.AddModule(I3TableWriter, name, TableService=tabler,
            **kwargs)

if I3_USE_HDF5:
    from icecube._tableio import I3HDFTableService

    _OPEN_FILENAMES_KEY = "I3HDFWriter_open_filenames"

    class OutputFileInUseError(ValueError):
        pass

    @icetray.traysegment_inherit(I3TableWriter,
        removeopts=('TableService',))
    def I3HDFWriter(tray, name, Output=None, CompressionLevel=6, **kwargs):
        """Tabulate data to an HDF5 file.

        :param Output: Path to output file
        :param CompressionLevel: gzip compression to apply to each table
        """

        if Output is None:
            raise ValueError("You must supply an output file name!")

        if _OPEN_FILENAMES_KEY in tray.context:
            if Output in tray.context[_OPEN_FILENAMES_KEY]:
                raise OutputFileInUseError(
                    f"File {Output} was already opened in segment "
                    f"{tray.context[_OPEN_FILENAMES_KEY][Output]}. "
                    "Use unique output file names."
                )
            tray.context[_OPEN_FILENAMES_KEY][Output] = name
        else:
            tray.context[_OPEN_FILENAMES_KEY] = {Output: name}

        # Ready file for staging out if so configured
        if 'I3FileStager' in tray.context:
            stager = tray.context['I3FileStager']
            Output = stager.GetWriteablePath(Output)

        tabler = I3HDFTableService(Output, CompressionLevel)
        tray.AddModule(I3TableWriter, name, TableService=tabler,
            **kwargs)

    @icetray.traysegment_inherit(I3HDFWriter)
    def I3SimHDFWriter(tray, name, RunNumber=0, **kwargs):
        """
        Tabulate untriggered data (Q frames only, no event headers)

        :param RunNumber: run ID to use in made-up headers. This should be unique
                        in each file.
        """

        if "SubEventStreams" in kwargs:
            raise TypeError("SubEventStreams cannot be set for I3SimHDFWriter")

        from icecube import icetray, dataclasses, phys_services
        class FakeEventHeader(icetray.I3ConditionalModule):
            def __init__(self, ctx):
                super().__init__(ctx)
                self.event_id = 0
            def DAQ(self, frame):
                header = dataclasses.I3EventHeader()
                header.run_id = RunNumber
                header.event_id = self.event_id
                self.event_id += 1
                frame['I3EventHeader'] = header
                self.PushFrame(frame)
        tray.Add(FakeEventHeader,
                    If=lambda f: 'I3EventHeader' not in f)

        tray.Add("I3NullSplitter",SubEventStreamName="SimHDFWriter")

        tray.Add(I3HDFWriter, name, SubEventStreams=["SimHDFWriter"], **kwargs)

if I3_USE_SQLITE3:
    from icecube._tableio import I3SQLiteTableService

    @icetray.traysegment_inherit(I3TableWriter,
        removeopts=('TableService',))
    def I3SQLiteWriter(tray, name, path=None, **kwargs):
        """Tabulate data to an SQL database.

        :param path: Path to output database file
        """

        if path is None:
            raise ValueError("You must supply an output database path!")

        tabler = I3SQLiteTableService(path)
        tray.AddModule(I3TableWriter, name, TableService=tabler,
            **kwargs)

if I3_USE_ARROW:
    from icecube._tableio import I3ParquetTableService  # type: ignore[attr-defined]

    @icetray.traysegment_inherit(I3TableWriter,
        removeopts=('TableService',))
    def I3ParquetWriter(tray, name, folder_path=None, compression="uncompressed", **kwargs):
        """Tabulate data to a parquet file.

        :param folder_path: Path to output folder
        :param compression: Compression type
        """

        if folder_path is None:
            raise ValueError("You must supply an output folder path!")

        tabler = I3ParquetTableService(folder_path, compression)
        tray.AddModule(I3TableWriter, name, TableService=tabler,
            **kwargs)

from icecube.tableio import types

from icecube.tableio.enum3 import enum

# clean the local dictionary
del icetray
