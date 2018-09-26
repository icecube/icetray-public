# -*- coding: utf-8 -*-
# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id: types.py 107762 2013-07-01 22:55:31Z nwhitehorn $
# 
# @version $Revision: 107762 $
# @date $LastChangedDate: 2013-07-01 18:55:31 -0400 (Mon, 01 Jul 2013) $
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: nwhitehorn $
# 

from icecube.tableio import I3Datatype

dt = I3Datatype.TypeClass
def make_dtype(kind, size, signed):
    dtype = I3Datatype()
    dtype.kind = kind
    dtype.size = size
    dtype.is_signed = signed
    return dtype

Int8    = make_dtype(dt.Int,1,True)
Int16   = make_dtype(dt.Int,2,True)
Int32   = make_dtype(dt.Int,4,True)
Int64   = make_dtype(dt.Int,8,True)
        
UInt8   = make_dtype(dt.Int,1,False)
UInt16  = make_dtype(dt.Int,2,False)
UInt32  = make_dtype(dt.Int,4,False)
UInt64  = make_dtype(dt.Int,8,False)

Float32 = make_dtype(dt.Float,4,True)
Float64 = make_dtype(dt.Float,8,True)

Bool    = make_dtype(dt.Bool,1,False)

# make a pretty-printer for I3Datatype
def pretty_dtype(self):
    return "I3Datatype(kind=%s, size=%s, signed=%s)"  % (self.kind,self.size,self.is_signed)
    
I3Datatype.__repr__ = pretty_dtype

del I3Datatype,dt,make_dtype,pretty_dtype

import sys
if sys.version_info[0] >= 3:
	from icecube.tableio.enum3 import enum as Enum
else:
	from icecube.tableio.enum2 import enum as Enum

