#!/usr/bin/env python
"""
Test I3TrayInfo
"""

import os
from icecube import icetray,dataio

def main():
    filename = os.path.expandvars('$I3_TESTDATA/superdst/test_data_1115_amd64.i3.gz')
    f = dataio.I3File(filename)
    fr = f.pop_frame()
    i = fr.values()[0]
    if not isinstance(i,icetray.I3TrayInfo):
        raise Exception('not a trayinfo object')

    m = i.modules_in_order
    if m[0] != 'reader':
        raise Exception('first module not reader')

    f = i.factories_in_order
    if f[0] != '__config':
        raise Exception('first factory not config')

    mc = i.module_configs
    fc = i.factory_configs
    # TODO: add tests for validity
    #       depends on #1531

    if i.svn_url != 'Unknown':
        raise Exception('svn_url incorrect')

    if i.svn_externals != '':
        raise Exception('svn_externals incorrect')

    if i.svn_revision != 0:
        raise Exception('svn_revision incorrect')

    h = i.host_info
    if h['username'] != 'jvansanten':
        raise Exception('bad host info')

    text = i.__str__()
    if 'Icetray run started' not in text:
        raise Exception('missing start time')

    text2 = repr(i)
    if not text2.startswith('tray = I3Tray.I3Tray()'):
        raise Exception('no I3Tray')

if __name__ == '__main__':
    main()
