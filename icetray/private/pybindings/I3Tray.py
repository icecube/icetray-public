#
# Copyright (C) 2004-9   Troy D. Straszheim
#
import os, sys, inspect
import icecube.icetray as icetray

I3Units = icetray.I3Units

# manually create a global symbol 'NaN'
NaN = float('NaN')
Inf = float('inf')

def load(filename):
    """
    load the library (via dlopen) into the running tray.  This is
    primarily used for libraries that don't have python bindings
    (eventually all libraries should have at least stub python
    bindings, making them loadable via the standard python *import*,
    and this sould be obsolete.

    :param filename:
      should be the name of the file to load
      including the leading ``lib``, but *not* including the trailing
      ``.so`` or ``.dylib``, eg::

        load("libdataio")
        load("libexamples")
      
    """
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

    def AddModule(self, _type, _name, **kwargs):
        """
        Add a module to the tray's processing stream.

        :param _type:
          either a string (search for a registered c++
          module in the module factory) or a python class (create a
          module of this type), or a python function (create a python
          module to wrap this function).

        :param _name:
          instance name for this module, needed when
          connecting in/outboxes in unusual ways, otherwise just make
          this string unique among the set of *name* parameters passed
          to AddModule and AddService.

        :param kwargs: parameter values forwarded to the added module

        **Example**::

            tray.AddModule("I3Reader", "reader",
                           Filename = "/path/to/foo.i3",
                           SkipKeys = ['I3DST', 'RecoPulses'])
        
        """
        if inspect.isclass(_type) and not icetray.I3Module in inspect.getmro(_type):
            raise RuntimeError, "Module %s of type %s doesn't inherit from icecube.icetray.I3Module" % (_name, _type)
        try:
            self.the_tray.AddModule(_type, _name)
            self.last_added = _name
            for k,v in kwargs.items():
                self.the_tray.SetParameter(_name, k, v)
            return self
        except:
            raise

    def AddService(self, _type, _name, **kwargs):
        self.the_tray.AddService(_type, _name)
        self.last_added = _name
        for k,v in kwargs.items():
            self.the_tray.SetParameter(_name, k, v)
        return self

    def AddSegment(self, segment, _name, **kwargs):
        segment(self, _name, **kwargs)
    
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


