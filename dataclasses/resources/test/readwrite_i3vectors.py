#!/usr/bin/env python

from icecube import icetray, dataclasses, dataio
from os.path import expandvars
from os import unlink
from pprint import pprint

import sys
if sys.version_info[0] > 2 or sys.version_info[1] > 4:
	import hashlib
else:
	import md5 as hashlib

is_fail = False


def do_one(Type, name, gen, checksum):
    print("Writing %s" % Type)
    global is_fail
    name += '.i3'
    i3f = dataio.I3File(name, 'w')
	
    tinst = Type()
    i = 0
    for value in gen:
        tinst.append(value)
        i += 1
    print("%d entries" % i) 
    frame = icetray.I3Frame()
    frame[name] = tinst

    i3f.push(frame)
    i3f.close()

    f = open(name, 'rb')
    data = f.read()
    hsh = hashlib.md5()
    hsh.update(data)
    hd = hsh.hexdigest()
    if (hd != checksum):
        print("****************** ERRORZ ERRORZ ***********************")
        print("%s != %s (file %s, %u bytes)" % ( hd, checksum, name, len(data) ))
        is_fail = True

def check_on(Type, name, gen):
    print("Reading %s" % Type)
    global is_fail
    name += '.i3'
    i3f = dataio.I3File(name, 'r')
    frame = i3f.pop_frame()
    thing = frame[name]

    i = 0
    for value in gen:
        if not (thing[i] == value):
            print("********************* FAIL ***********************")
            print("at %d, %s != %s" % (i, thing[i], value))
            is_fail = True
        i += 1
    print("checked %d entries." % i)
    unlink(name)

    
def gen(n):
    value = 0;
    while value < n:
        yield value
        value+= 1

def bool_gen(max):
    n = 0
    while n < max:
        yield n % 3 == 0
        n += 1
	
checks = [(dataclasses.I3VectorBool, "vector_bool",
	   (bool_gen, [20000]), '4fc19aae89a02bcdd7a545bc1716243f'),
	  (dataclasses.I3VectorShort, "vector_short",
	   (gen, [5000]), 'b4d683a72e56ecda9e86d435a88e777e'),
	  (dataclasses.I3VectorUShort, "vector_ushort",
	   (gen, [5000]), '1e5b7983b448f5b06c6d7636d0af6334'),
	  (dataclasses.I3VectorInt, "vector_int",
	   (gen, [5000]), '8ca525efb28daab46c0bbbf16ece563d'),
	  (dataclasses.I3VectorUInt, "vector_uint",
	   (gen, [5000]), 'f10dd0f033c7eacd6ef619879a8a25d5'),
	  (dataclasses.I3VectorFloat, "vector_float",
	   (gen, [5000]), '1ea638013cffe539c55cd18dfc215490'),
	  (dataclasses.I3VectorDouble, "vector_double",
	   (gen, [5000]), '51189332e38ecaac9dd4dd195ed9f611')]

# Correct for change in typedef name: long vs. long long
import struct
if struct.calcsize("L") == 8:
	  checks += [(dataclasses.I3VectorInt64, "vector_int64",
	   (gen, [5000]), '2b69f7a70cbbede826914ff1ce7b5ef0'),
	  (dataclasses.I3VectorUInt64, "vector_uint64",
	   (gen, [5000]), '922c834496e99174536ca9fedacb5c1f')]
else:
	  checks += [(dataclasses.I3VectorInt64, "vector_int64",
	   (gen, [5000]), '5e8f791ae556d410d551e475d48a2fca'),
	  (dataclasses.I3VectorUInt64, "vector_uint64",
	   (gen, [5000]), '5b7248d0fa3f607d3b14409d5dfafe0a')]

for (T, N, (GEN,ARGS), CHECKSUM) in checks:
    do_one(T, N, GEN(*ARGS), CHECKSUM)
    check_on(T, N, GEN(*ARGS))
    
sys.exit(is_fail)

