#!/usr/bin/env python3

import sys
from I3Tray import I3Tray

tray = I3Tray()

def takesthree(frame, where, value):
    print(frame)
    return False

tray.AddModule('BottomlessSource')
tray.AddModule(takesthree, value = 'blam')               

try:
    tray.Execute(10)
except Exception as e:
    print(e)
    print("OK, threw as expected")
else:    
    print('that should have complained about not enough arguments')
    sys.exit(1)


                   

