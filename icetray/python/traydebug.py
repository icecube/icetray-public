# 
#  $Id: traydebug.py 1667 2013-07-01 21:30:57Z nwhitehorn $
#  
#  Copyright (C) 2011
#  Eike Middell <eike.middell@desy.de>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#


import sys, os.path
from collections import namedtuple, defaultdict
from xml.etree.ElementTree import ElementTree,Element
import itertools
import re
import math
import cgi

from icecube.icetray import i3inspect

from jinja2 import Template

################################################################################

class ParameterDescription:
    """
        bundles name, description and defaultvalue of a Parameter
    """
    def __init__(self, name, defaultvalue, description):
        self.name = name
        self.defaultvalue = defaultvalue
        self.description = description

    def __repr__(self):
        return "<Parameter name='%s' default='%s'>" % (self.name, self.defaultvalue)

################################################################################

class ModuleDescription:
    """
        module definitions, basically a dict of ParameterDescription
    """
    def __init__(self, typename):
        self.typename = typename
        self.params = dict() # name.lower() -> ParameterDescription(..)
        
        # Get the defaults from the module. This will fail if the module
        # is actually a Python function, but then it has no defaults anyhow.
        try:
            config = i3inspect.get_configuration(typename)
        except TypeError:
            return
        
        desc = config.descriptions
        for k in config.keys():
            self.add_param(k, config[k], desc[k])

    def add_param(self, name,default, description):
        self.params[name.lower()] = ParameterDescription(name,default,description)

    def has_param(self, pname):
        return ( pname.lower() in self.params )

    def __repr__(self):
        return "<Module typename='%s' with %d parameters>" % (self.typename, len(self.params))

################################################################################
 
class ModuleInstance:
    """
        a configured module. has a link to the ModuleDescription and contains a dict with
        the parameters that were configured in the steering file
    """
    def __init__(self, instancename, module, before=None,after=None):
        self.instancename = instancename
        self.module = module
        self.before = before
        self.after = after
        self.paramvalues = dict()

    def set_param(self, pname, pvalue):
        if not self.module.has_param(pname):
            # print "adding parameter %s with value %s to %s %s" % (pname, pvalue, self.module.typename, self.instancename)
            self.module.add_param(name= pname,
                                  default=None,
                                  description=None)
        self.paramvalues[pname.lower()] = pvalue

    def __call__(self, *args):
        """
            I3TrayDebugger's Add[Module,Service] will return the ModuleInstance
            to collect parameters configured the old way (without keywords)
        """
        for ptuple in args:
            assert isinstance(ptuple, tuple)
            assert isinstance(ptuple[0], str)
            self.set_param(*ptuple)

    def get_params(self):
        result = dict()
        for pname, pvalue in self.paramvalues.items():
            pdesc = self.module.params[pname]
            result[pdesc.name] = (pvalue, pdesc.defaultvalue, pdesc.description)

        for pname, pdesc in self.module.params.items():
            if pdesc.name not in result:
                result[pdesc.name] = (pdesc.defaultvalue, pdesc.defaultvalue, pdesc.description)

        return result

    def __str__(self):
        rep = "%s '%s'" % (self.module.typename, self.instancename)
        for pname, pvalue in self.paramvalues.items():
            rep += '\n    %s=%s' % (pname, repr(pvalue))
        return rep

class XMLOutput:
    def __init__(self, modules, services):
        modules = modules
        services = services

        self.modules = []

        for k, mod in itertools.chain(iter(modules.items()), 
                                      iter(services.items())):
            d = dict()
            d["classname"] = self.escape(mod.module.typename)
            d["instancename"] = self.escape(mod.instancename)
            d["params"] = []
            for pname,(pvalue,pdefault,pdesc) in mod.get_params().items(): 
                d["params"].append( dict(name=self.escape(pname), 
                                         default=self.escape(pdefault), 
                                         configured=self.escape(pvalue), 
                                         description=self.escape(pdesc)) )

            self.modules.append(d)

    template = Template("""<xml>
        {% for mod in modules %}
        <module>
         <classname> {{mod.classname}} </classname>
         <instancename> {{mod.instancename}} </instancename>
         <parameters>
            {% for param in mod.params %}
            <param>
                <name> {{ param.name }} </name>
                <default> {{ param.default }} </default>
                <configured> {{ param.configured }} </configured>
                <description> {{ param.description }} </description>
            </param>
            {% endfor %}
         </parameters>
        </module>
        {% endfor %}
    </xml>
    """)

    def escape(self, s):
        if isinstance(s, str):
            return cgi.escape(s)
        else:
            return cgi.escape(str(s))

    def generate(self, outputfile):
        out = open(outputfile, "w")
        out.write(self.template.render(modules=self.modules))
        out.close()



################################################################################

class I3TrayDebugger:
    """
        duck-typed replacement for I3Tray to collect information about the steering file
    """
    def __init__(self, outputfile=None, **kwargs):
        self.outputfile = outputfile
        self.modules  = dict()
        self.modules_in_order = []
        self.services = dict()

        self.last_added_module = None

        self.graph_args = kwargs

    def create_instance(self, typename, instancename, paramvalues):
        mod = ModuleDescription(typename)
        
        inst = ModuleInstance(instancename, mod)
        for pname, pvalue in paramvalues.items():
            inst.set_param(pname, pvalue) 

        return inst

    def AddService(self, typename, instancename, **kwargs):
        inst = self.create_instance(typename, instancename, kwargs)
        self.services[instancename] = inst
        return inst
    
    def AddModule(self, typename, instancename, **kwargs):
        inst = self.create_instance(typename, instancename, kwargs)
        self.modules_in_order.append(instancename)
        self.modules[instancename] = inst
        inst.before = self.last_added_module
        if self.last_added_module is not None:
            self.last_added_module.after = inst
        self.last_added_module = inst

        return inst

    def MoveModule(self, name, anchor, after):
        self.modules_in_order.remove(name)
        idx = self.modules_in_order.index(anchor)
        if after and idx == len(self.modules_in_order)-1:
            self.modules_in_order.append(name)
        else:
            self.modules_in_order.insert(idx, name)

    def AddSegment(self, _segment, _name, **kwargs):
        return _segment(self, _name, **kwargs)

    def __str__(self):
        def center(text, fill, size):
            nf = (size-len(text)-2)/2
            line = fill*nf + " " + text + " " + fill*nf
            return line + fill*(size-len(line))

        rep = str()
        if len(self.services) > 0:
            rep += center("I3Services", "-", 60)
            for name, conf in self.services.items():
                rep += "\n" + str(conf)
        if len(self.modules) > 0:
            rep += "\n" + center("I3Modules", "-", 60)
            for name in self.modules_in_order:
                rep += "\n" + str(self.modules[name])
        return rep

    def Execute(self, maxcount=0):
        print(self)
        
        if self.outputfile is not None:
            print("creating graph")
            XMLOutput(self.modules, self.services).generate(self.outputfile)

    def Finish(self):
        pass

