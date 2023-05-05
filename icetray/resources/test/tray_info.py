#!/usr/bin/env python3
"""
Test I3TrayInfo
"""

import os

from icecube import dataio, icetray


def main():
    filename = os.path.expandvars('$I3_TESTDATA/superdst/test_data_1115_amd64.i3.gz')
    f = dataio.I3File(filename)
    fr = f.pop_frame()
    i = fr.values()[0]
    if not isinstance(i, icetray.I3TrayInfo):
        raise Exception('not a trayinfo object')

    m = i.modules_in_order
    if m[0] != 'reader':
        raise Exception('first module not reader')

    f = i.factories_in_order
    if f[0] != '__config':
        raise Exception('first factory not config')

    mc = i.module_configs
    for k in mc.keys():
        if not isinstance(mc[k], icetray.I3Configuration):
            raise Exception(f'module_config {k} not an I3Configuration object')
    fc = i.factory_configs
    for k in fc.keys():
        if not isinstance(fc[k], icetray.I3Configuration):
            raise Exception(f'factory_config {k} not an I3Configuration object')

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
