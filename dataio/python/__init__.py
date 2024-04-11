# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from typing import Any, Iterable, Type, Optional

from icecube import icetray, dataclasses
from icecube._dataio import *

from icecube.dataio.I3FileStagerFile import AbstractFileStager
set_local_scratch_dir = AbstractFileStager.set_local_scratch_dir

def get_stagers(staging_directory=None, extra_stagers=[]):
    # type: (Optional[str], Iterable[Type[I3FileStager]]) -> I3FileStagerCollection
    """
    Set up file stagers for all supported URL schemes.

    :param staging_directory: use this directory for temporary files. If not\
        specified, the staging directory will be guessed.
    :param extra_stagers: a list of stager classes to instantiate in addition to\
        the those included with the base distribution
    """
    if staging_directory is not None:
        set_local_scratch_dir(staging_directory)

    impls = set(extra_stagers)
    for stager in AbstractFileStager.get_subclasses():
        impls.add(stager)

    return I3FileStagerCollection([stager() for stager in impls])

@icetray.traysegment_inherit('I3Reader')
def I3Reader(tray, name, **kwargs):
    # type: (icetray.I3Tray, str, dict[str,Any]) -> None
    """Read an .i3 file. This supports remote files
    by specifying URLs and will stage them in an auto-configured
    local scratch directory.

    Using a URL like
    file://home/user/file.i3 will stage local files,
    using plain filenames will read the files directly.
    """

    import os, pwd
    from icecube import icetray, dataclasses

    tray.context['I3FileStager'] = get_stagers()

    tray.AddModule('I3Reader', name, **kwargs)

del icetray
