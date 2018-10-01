import collections
import re

from icecube import icetray
from icecube import dataclasses
from icecube import dataio

def format_line( frame, key, maxwidth = None, ellipsis = '...' ):
    '''Given an icecube frame and a key in that frame, return
    exactly one line of text describing the I3FrameObject with that key.
    Try to make the text as useful to a human reader as possible.

    If accessing the object generates an exception, catch it and 
    return its description.

    Clip to an optional maximum width with a trailing ellipsis'''
    try:
        obj = frame[key]        
        if (obj is None) and (key in frame):
            return '(Unreadable)'
        if hasattr(obj, "apply"):
            obj = obj.apply(frame)
        haslength = isinstance( obj, collections.Iterable )
    except Exception as e:
        obstr = '(Unreadable)'
    else:
        if( haslength ):
            obstr = 'Iterable with {0} items'.format(len(obj))
        else:
            try:
                # give the module and class name
                obstr = '{0}.{1} object'.format(obj.__module__,obj.__class__.__name__)
            except Exception as e:
                # try basic repr
                obstr = repr(obj).split('\n')[0]

    if( maxwidth ):
        if( len(obstr) > maxwidth ):
            obstr = obstr[:maxwidth - len(ellipsis)] + ellipsis[0:maxwidth]

    return obstr


def format_detail( frame, key ):
    '''Given an icecube frame and a key in that frame, return
    a human-readable string that describes the item in detail.'''

    try:
        obj = frame[key]
        if hasattr(obj, "apply"):
            obj = obj.apply(frame)
        if isinstance(obj,dataclasses.I3String):
            message = obj.value
        if isinstance(obj,dataclasses.I3Double):
            message = str(obj.value)
        elif hasattr(obj, "items"):
            message = '{\n'
            for k in obj.keys():
                message += str(k)+': '+str(obj[k])+'\n'
            message += '}'
        else:
            message = str(obj)
    except Exception as e:
        message = '({0})'.format(e)
    
    if re.match('<icecube\.[\S]*\.[\S]* object at [0-9xa-f]*>', message):
        # Standard boring format.  In some cases we might be able to do better.
        if isinstance( obj, collections.Iterable):
            message += ', contents:\n' + '\n'.join( [ str(x) for x in frame[key] ] )

    return message

def format_xml( frame, key ):
    '''Given an icecube frame and a key in that frame, return
    the xml serialization of the item.'''

    try:
        if key in frame:
            message = frame.as_xml(key)
        else:
            message = key+' not in frame'
    except Exception as e:
        message = '({0})'.format(e)
    
    return message.expandtabs(4)

def format_size( frame, key):
    '''Given an icecube frame and a key in that frame, return
    the size of the string.

    Default converts the string in Kilo, Mega, or GigaByte.
      Adjust conversion to different formats by supplying
      the list with given unit names.'''

    cfactor = 1024.
    sunit = False
    unit = ['K', 'M', 'G']

    if key in frame:
        size = frame.size(key)
    else:
        return str()

    while size > cfactor and bool(unit):
        size /= cfactor
        sunit = unit.pop(0)

    if bool(sunit):
        if size < 10:
            return '{0:1.1f}{1:1s}'.format(size,sunit)
        else:
            return '{0:4.0f}{1:1s}'.format(size, sunit)
    # Bytes are integer value, so show them like this
    return '{0:4d} '.format(size)

