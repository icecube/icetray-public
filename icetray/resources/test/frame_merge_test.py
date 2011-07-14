#!/usr/bin/env python

"""
Test the I3Frame's merging/purging stuff
"""

from icecube.icetray import *
import sys

def makeframe(stream, key = None):
    if not key:
        key = stream
    frame = I3Frame(stream)
    frame[key] = I3Int(ord(stream))
    return frame

a = makeframe('a')

assert 'a' in a
assert 'a2' not in a

a2 = makeframe('a', 'a2')

assert 'a2' in a2
assert 'a' not in a2

a.Merge(a2)

assert 'a2' in a
assert 'a'  in a

################################################################################

a = makeframe('a')
b = makeframe('b')
c = makeframe('c')

a.Merge(b)

assert 'a' in a
assert 'b' in a

assert 'a' not in b
assert 'b' in b

a.Merge(c)

assert 'a' in a
assert 'b' in a
assert 'c' in a

print a

a.Purge(I3Frame.Stream('b'))

assert 'a' in a
assert 'b' not in a
assert 'c' in a

a.Purge(I3Frame.Stream('c'))

assert 'a' in a 
assert 'b' not in a 
assert 'c' not in a 

################################################################################

a = makeframe('a')
b = makeframe('b')
c = makeframe('c')

a.Merge(b)
a.Merge(c)

assert 'a' in a
assert 'b' in a
assert 'c' in a

a.Purge()

assert 'a' in a
assert 'b' not in a
assert 'c' not in a

################################################################################

a = makeframe('a')
a2 = makeframe('a')
a['a'].value = 666
a2['a'].value = 777
 
try:
    a.Merge(a2)
except:
    print "ok,  that threw"
    sys.exit(0)
else:
    print "that should have thrown"
    sys.exit(1)


