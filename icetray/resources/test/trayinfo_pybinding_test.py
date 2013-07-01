#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio, util
from I3Tray import *
import os

tray = I3Tray()

tray.AddModule("BottomlessSource", "bs")

def put_ints(frame, where="there", value=999, somedict=dict(three=3, four=4)):
    frame[where] = icetray.I3Int(value)
    
tray.AddModule(put_ints, "put_ints",
               where = 'thisiswhere',
               value = 777,
               somedict = dict(five=5, six=6))

tray.AddModule("I3Writer", "writer",
               Filename = "infos.i3",
               SkipKeys = ['one', 'two', 'three'])

tray.AddModule("TrashCan", "tc")

tray.Execute(1)
tray.Finish()

f = dataio.I3File('infos.i3')
infoframe = f.pop_frame()
keys = list(infoframe.keys())

ti = infoframe[keys[0]]
print(ti)

print(ti.modules_in_order)
print(ti.module_configs)
for m in ti.modules_in_order:
    config = ti.module_configs[m]
    print("%s: %s %s Configs: %s" % (m, config.ClassName, config.InstanceName, list(config.keys())))
    for k in config.keys():
        print("%s = %s" % (k, config[k]))


assert ti.module_configs['put_ints']['where'] == 'thisiswhere'
assert ti.module_configs['put_ints']['value'] == 777
assert ti.module_configs['put_ints']['somedict'] == { 'five' : 5, 'six' : 6 }
print(ti.host_info)
print(ti.svn_url)
print(ti.svn_revision)
print(ti.svn_externals)




