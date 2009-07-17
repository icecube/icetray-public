#
# Copyright (C) 2004-9   Troy D. Straszheim
#
import os, sys, inspect
import icecube.icetray as icetray
import I3Units

# manually create a global symbol 'NaN'
NaN = float('NaN')
Inf = float('inf')

def load(filename):
    try:
        icetray.load(filename)
    except:
        print """\n***\n*** Failed to load library. (%s)\n*** %s\n***\n""" % (sys.exc_info()[0], sys.exc_info()[1])
        sys.exit(1)
    
def OMKey(string,omnum):
    return icetray.OMKey(string,omnum)

class I3Tray:
    """
    A convenience wrapper around the wrapped c++ class icetray.I3Tray.
    This class provides keyword-arguments to the AddModule and so forth.
    """

    def __init__(self):
        """
        Creates an I3Tray.
        """
        self.last_added = None
        self.the_tray = icetray.I3Tray()

    def AddModule(self, type, name, **kwargs):
        """
        Add a module to the tray's processing stream.

        The first argument, *type*, can be either a string (search for
        a registered c++ module in the module factory) or a python
        class (create a module of this type), or a python function
        (create a python module to wrap this function).
        """
        if inspect.isclass(type) and not icetray.I3Module in inspect.getmro(type):
            raise RuntimeError, "Module %s of type %s doesn't inherit from icecube.icetray.I3Module" % (name, type)
        try:
            self.the_tray.AddModule(type, name)
            self.last_added = name
            for k,v in kwargs.items():
                self.the_tray.SetParameter(name, k, v)
            return self
        except:
            raise

    def AddService(self, type, name, **kwargs):
        self.the_tray.AddService(type, name)
        self.last_added = name
        for k,v in kwargs.items():
            self.the_tray.SetParameter(name, k, v)
        return self
    
    def SetParameter(self, module, param, value):
        self.the_tray.SetParameter(module, param, value)
        return self
    
    def ConnectBoxes(self, *args):
        if len(args) == 4:
            self.the_tray.ConnectBoxes(args[0], args[1], args[2], args[3])
        elif len(args) == 3:
            self.the_tray.ConnectBoxes(args[0], args[1], args[2])
        else:
            print 'Wrong number of arguments', len(args), 'to ConnectBoxes'
        
    def __call__(self, *args):
        for pair in args:
            print self.last_added +': ', pair[0], '=', pair[1]
            self.the_tray.SetParameter(self.last_added, pair[0], pair[1])
        return self

    def Execute(*args):

        assert len(args) < 3, "Too many arguments to Execute()"
            
        if len(args) == 2:
            args[0].the_tray.Execute(args[1])
        else:
            args[0].the_tray.Execute()

    def Usage(self):
        return self.the_tray.Usage()

    def Finish(self):
        self.the_tray.Finish()


