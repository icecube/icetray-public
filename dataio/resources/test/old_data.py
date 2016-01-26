#!/usr/bin/env python
"""
Test loading old data with modern software.
"""

import os

from icecube import icetray,dataio,dataclasses

def check_file(f):
    print('checking file: '+f)
    for frame in dataio.I3File(f):
        if 'I3EventHeader' in frame:
            foo = frame['I3EventHeader']
        if 'LineFit' in frame:
            foo = frame['LineFit']

def main():
    root = os.path.expandvars('$I3_TESTDATA/dataio/olddata')
    for d in sorted(os.listdir(root)):
        d = os.path.join(root,d)
        if os.path.isdir(d):
            for f in os.listdir(d):
                check_file(os.path.join(d,f))

if __name__ == '__main__':
    main()
