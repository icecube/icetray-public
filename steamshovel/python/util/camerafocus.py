from icecube import dataclasses
# Non-offline-software projects can be attempted to be imported here,
# but import errors should be caught and silently discarded

def getFocusPoint( frame, key ):
    '''
    Given an I3Frame and a key, return a camera focus point for that key

    The return value should be an I3Position (or generally any type convertable
    to a shovelart.vec3d).  If no position can be extracted from the object,
    return None or throw any exception.
    '''
    tname = frame.type_name(key)

    # discover usable types
    usable = set("I3Position")
    for cls_name in dir(dataclasses):
        cls = getattr(dataclasses, cls_name)
        if hasattr(cls, "pos"):
             # we assume that Python name matches C++ name...
            usable.add(cls_name)

    if tname not in usable:
        return None

    obj = frame[key]
    if hasattr(obj, 'pos'):
        return obj.pos
    return obj
