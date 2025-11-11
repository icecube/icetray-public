# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

# create the registry first, so that compiled bookers can be added
from icecube import icetray
from icecube.tableio.registry import I3ConverterRegistry

from icecube._tableio import (
    ConvertState, EnumMember, EnumMemberList, I3BroadcastTableService, I3CSVTableService, I3Converter,
    I3ConverterBundle, I3ConverterMill, I3Datatype, I3Table, I3TableRow, I3TableRowDescription, I3TableService,
    I3TableTranscriber, I3TableWriterWorker, vector_I3ConverterMillPtr, I3_USE_ROOT
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

from icecube.tableio import types

from icecube.tableio.enum3 import enum

# clean the local dictionary
del icetray
