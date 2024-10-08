#
#  $Id: traysegment.py 1635 2013-06-28 21:33:24Z nwhitehorn $
#
#  Copyright (C) 2011 Nathan Whitehorn <nwhitehorn@icecube.wisc.edu>
#  Copyright (C) 2011 the IceCube Collaboration <http://www.icecube.wisc.edu>
#  SPDX-License-Identifier: BSD-2-Clause
#

import inspect # the real inspect
import functools
import time

def traysegment(function):
    """Register a tray segment configuration function with icetray.
    The segment can then be added to a tray using I3Tray.AddSegment().

    Usage:
    @icetray.traysegment
    def segment(tray, name, arg=stuff):
    """

    if inspect.getdoc(function) is None:
        function.__doc__ = "I3Tray segments should have docstrings. This one doesn't. Fix it."

    if len(inspect.getfullargspec(function)[0]) < 2:
        raise ValueError("I3Tray segments must have at least two arguments (tray and name)")

    function.__i3traysegment__ = True
    return function

def traysegment_inherit(parent, defaultoverrides=None, removeopts=None):
    """Register a tray segment configuration function with icetray,
    inheriting icetray-inspect information from another
    module/service/segment. The segment can then be added to a tray using
    I3Tray.AddSegment().

    Usage:
    @icetray.traysegment_inherit('I3ParentMod')
    def segment(tray, name, arg=stuff):
    """

    def traysegment_(function):
        func = traysegment(function)
        func.module = parent
        if defaultoverrides is not None:
            func.default_overrides = defaultoverrides
        if removeopts is not None:
            func.remove_opts = removeopts

        return func

    return traysegment_

def module_altconfig(module, **altdefargs):
    """Register an alternate [sub]set of defaults for a module or segment.
    Any parameters not specified in module_altconfig will use the
    original module defaults.

    Once added to a module's pybindings, an instance of the module with
    the specified alternate defaults can then be added to a tray using
    I3Tray.AddSegment().

    Usage: altconfig1 = icetray.module_altconfig('I3ExampleModule', Par1=5)
    """

    def segment(tray, name, **args):
        mergedargs = altdefargs.copy()
        for userarg in args.keys():
            mergedargs[userarg] = args[userarg]

        # Arguments to AddModule are, in general, case insensitive
        # Resolve any duplicate arguments with case ambiguities in favor
        # of the user's values.
        lowercasekeys = [i.lower() for i in mergedargs.keys()]
        for arg in altdefargs.keys():
            if lowercasekeys.count(arg.lower()) > 1:
                # Deduplicate by removing the default key
                mergedargs.pop(arg)

        if hasattr(module, '__i3traysegment__'):
            tray.AddSegment(module, name, **mergedargs)
        else:
            tray.AddModule(module, name, **mergedargs)

    # Mark what we're doing in various interesting places (like the docs)
    segment.__doc__ = "Alternate configuration for %s\n\nOverridden defaults:\n" % module
    for arg in list(altdefargs.keys()):
        segment.__doc__ += "\t%s=%s\n" % (arg, altdefargs[arg])

    func = traysegment_inherit(parent=module,
        defaultoverrides=altdefargs)(segment)
    return func


def timedtraysegment(original_traysegment):
    """Register a tray segment configuration function with icetray, but
    with an added timer put into the frame under name+"_SegmentTimer_seconds".
    The segment can then be added to a tray using I3Tray.AddSegment().

    Usage:
    @icetray.timedtraysegment
    def segment(tray, name, arg=stuff):
    """
    
    @traysegment
    @functools.wraps(original_traysegment)
    def segment_with_timers(tray, name, *args, **kwargs):
        from icecube.icetray import I3Frame
        from icecube.dataclasses import I3Double
        timer_name = name + "_SegmentTimer_seconds"
        def start_timer(frame):
            frame[timer_name] = I3Double(time.time())
        def end_timer(frame):
            runtime = time.time() - frame[timer_name].value
            del frame[timer_name]
            frame[timer_name] = I3Double(runtime)
            
        tray.Add(start_timer, timer_name+"_start",
                 Streams=[I3Frame.DAQ, I3Frame.Physics])
        return_vals = original_traysegment(tray, name, *args, **kwargs)
        tray.Add(end_timer, timer_name+"_stop",
                 Streams=[I3Frame.DAQ, I3Frame.Physics])
        return return_vals
        
    return segment_with_timers
