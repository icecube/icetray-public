#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from icecube.icetray import I3Tray
from icecube import dataio, icetray


import tempfile


def put_ints(frame,
             where="there",
             value=999,
             somedict=None):
    if somedict is None:
        somedict = dict(three=3, four=4)
    frame[where] = icetray.I3Int(value)


with tempfile.NamedTemporaryFile() as f:
    tray = I3Tray()
    tray.Add("BottomlessSource")
    tray.Add(put_ints, "put_ints",
             where = 'thisiswhere',
             value = 777,
             somedict = dict(five=5, six=6))
    tray.Add("I3Writer",
             Filename = f.name,
             SkipKeys = ['one', 'two', 'three'])
    tray.Execute(1)

    f = dataio.I3File(f.name)

infoframe = f.pop_frame()
keys = list(infoframe.keys())

ti = infoframe[keys[0]]
print(ti)

print(ti.modules_in_order)
print(ti.module_configs)
for m in ti.modules_in_order:
    config = ti.module_configs[m]
    print(type(m))
    print(type(config))
    keys = config.keys()
    print("%s: %s %s Configs: %s" % (m, config.ClassName, config.InstanceName, keys))
    for k in config.keys():
        print("%s = %s" % (k, config[k]))

assert ti.module_configs['put_ints']['where'] == 'thisiswhere'
assert ti.module_configs['put_ints']['value'] == 777
assert ti.module_configs['put_ints']['somedict'] == { 'five' : 5, 'six' : 6 }
print(ti.host_info)
print(ti.svn_url)
print(ti.svn_revision)
print(ti.svn_externals)
