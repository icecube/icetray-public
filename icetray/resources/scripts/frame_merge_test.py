#!/usr/bin/env python

"""
Test of how merges are done in the I3Reader, etc.
"""

from icecube.icetray import *

def docopy(frame):
    newframe = I3Frame(I3Frame.Stream('X'))
    newframe.copy(frame)
    newframe.merge(docopy.oldframe)
    docopy.oldframe.copy(newframe)
    return newframe

docopy.oldframe = I3Frame(I3Frame.Stream('X'))

aframe = I3Frame(I3Frame.Stream('A'))
aframe['a'] = I3Int(ord('a'))
print "aframe=", aframe

newfr = docopy(aframe)
print "docopy", newfr
assert 'a' in newfr

bframe = I3Frame(I3Frame.Stream('B'))
bframe['b'] = I3Int(ord('b'))
print "bframe=", bframe
newfr = docopy(bframe)
print "docopy", newfr
assert 'a' in newfr
assert 'b' in newfr

aframe = I3Frame(I3Frame.Stream('A'))
aframe['a2'] = I3Int(ord('a'))

newfr = docopy(aframe)
print newfr

assert 'a' not in newfr
assert 'a2' in newfr
assert 'b' in newfr

cframe = I3Frame(I3Frame.Stream('C'))
cframe['c'] = I3Int(7)

newfr = docopy(cframe)
print newfr
assert 'a2' in newfr
assert 'b' in newfr
assert 'c' in newfr

cframe.clear()

newfr = docopy(cframe)
print newfr
assert 'a2' in newfr
assert 'b' in newfr
assert not 'c' in newfr

#
#  Various other unity tests
#

a = I3Frame(I3Frame.Stream('A'))
b = I3Frame(I3Frame.Stream('B'))

a.merge(b)

assert a.GetStop() == I3Frame.Stream('A')
assert b.GetStop() == I3Frame.Stream('B')
assert len(a) == 0
assert len(b) == 0

a['a'] = None

print a

b.merge(a)

assert 'a' in b
assert 'a' in a

a.merge(b)

assert 'a' in b
assert 'a' in a

#
#  Let's put same thing on two streams and merge
#

b['X'] = I3Int(ord('b'))
a['X'] = I3Int(ord('a'))

a.merge(b)

# 'our' stream's version overwrites
assert a['X'].value == ord('a')

b.merge(a)

# 'our' stream's version overwrites
assert b['X'].value == ord('b')



