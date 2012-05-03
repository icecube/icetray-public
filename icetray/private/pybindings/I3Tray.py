#
# Copyright (C) 2004-9   Troy D. Straszheim
#
import os, sys, inspect
import icecube.icetray as icetray

I3Units = icetray.I3Units

# manually create a global symbol 'NaN'
NaN = float('NaN')
Inf = float('inf')

import platform, sys
from os.path import expandvars
from ctypes import cdll
def load(libname, verbose = True):
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
    if verbose :
        print "Loading ", libname, 50*"."

    if platform.system() != "Darwin" and \
       platform.system() != "Linux" :
        print "platform ", platform.system(), " not supported"
        sys.exit(1)
    
    if libname.endswith(".dylib" ) or \
           libname.endswith(".so" ) :
        print "*** Failure loading '",libname,"'."
        print "*** Load external libraries without extension."
        print "*** e.g. please omit '.dylib' or '.so'.\n";
        sys.exit(1)

    if not libname.startswith("lib") :
            libname = "lib" + libname
        
    if platform.system() == "Darwin" :
        libname += ".dylib"

    if platform.system() == "Linux" :
        libname += ".so"

    # look in the "standard" places
    try :
        cdll.LoadLibrary( libname )
    except OSError :
        # look in I3_BUILD
        libname = expandvars("I3_BUILD/lib/") + libname
        try :
            cdll.LoadLibrary( libname )
        except :
            print """\n***\n*** Failed to load library. (%s)\n*** %s\n***\n""" % \
                  (sys.exc_info()[0], sys.exc_info()[1])
        sys.exit(1)
    
    
def OMKey(string,omnum):
    return icetray.OMKey(string,omnum)

class I3Tray(icetray.I3Tray):
    """
    A convenience wrapper around the wrapped c++ class icetray.I3Tray.
    This class provides keyword-arguments to the AddModule and so forth.
    """

    def __init__(self):
        """
        Creates an I3Tray.
        """
        self.last_added = None
        icetray.I3Tray.__init__(self)

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
	if hasattr(_type, '__i3traysegment__'):
            raise RuntimeError, "Trying to add tray segment %s with AddModule. Use AddSegment instead." % _name

        if inspect.isclass(_type) and not icetray.I3Module in inspect.getmro(_type):
            raise RuntimeError, "Module %s of type %s doesn't inherit from icecube.icetray.I3Module" % (_name, _type)
        try:
            super(I3Tray, self).AddModule(_type, _name)
            self.last_added = _name
            for k,v in kwargs.items():
                super(I3Tray, self).SetParameter(_name, k, v)
            return self
        except:
            raise

    def AddService(self, _type, _name, **kwargs):
        super(I3Tray, self).AddService(_type, _name)
        self.last_added = _name
        for k,v in kwargs.items():
            super(I3Tray, self).SetParameter(_name, k, v)
        return self

    def AddSegment(self, _segment, _name, **kwargs):
	(keys,vals) = (kwargs.keys(), kwargs.values())
	argnames = inspect.getargspec(_segment)[0]
	largnames = [a.lower() for a in argnames]

	# Make case-insensitive where possible by matching to signature
	for k in keys:
		if not k.lower() in largnames:
			continue
		keys[keys.index(k)] = argnames[largnames.index(k.lower())]
	kwargs = dict(zip(keys, vals))

        return _segment(self, _name, **kwargs)
    
    def SetParameter(self, module, param, value):
        super(I3Tray, self).SetParameter(module, param, value)
        return self
    
    def ConnectBoxes(self, *args):
        if len(args) == 4:
            super(I3Tray, self).ConnectBoxes(args[0], args[1], args[2], args[3])
        elif len(args) == 3:
            super(I3Tray, self).ConnectBoxes(args[0], args[1], args[2])
        else:
            print 'Wrong number of arguments', len(args), 'to ConnectBoxes'
        
    def __call__(self, *args):
        for pair in args:
            print self.last_added +': ', pair[0], '=', pair[1]
            super(I3Tray, self).SetParameter(self.last_added, pair[0], pair[1])
        return self

    def __repr__(self):
	return self.TrayInfo().__repr__()

    def Execute(self, *args):

        assert len(args) < 2, "Too many arguments to Execute()"
            
        if len(args) == 1:
            super(I3Tray, self).Execute(args[0])
        else:
            super(I3Tray, self).Execute()

    def PrintUsage(tray, fraction=0.9):
    	"""
    	Pretty-print the time spent in each module, as usually done in the I3Module
    	destructor.
    
    	:param fraction: Print out the usage of modules consuming this much of the
    	                 total runtime.
    	"""
    	usage = tray.Usage()
	if len(usage) == 0:
		return
    	keys = [p.key() for p in usage]
    	keys.sort(key=lambda k: usage[k].usertime + usage[k].systime)
    	total_time = sum([p.data().usertime + p.data().systime for p in usage])
    	acc_time = 0
    	print '-'*99
    	for k in keys[::-1]:
    		pusage = usage[k]
    		print "%40s: %6u calls to physics %9.2fs user %9.2fs system" % (k, pusage.ncall, pusage.usertime, pusage.systime)
    
    		acc_time += pusage.systime + pusage.usertime
    		if acc_time/total_time > fraction:
    			break
    	print '-'*99


