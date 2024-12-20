#
# Copyright (C) 2004-9   Troy D. Straszheim
# SPDX-License-Identifier: BSD-2-Clause
import inspect
from icecube import _icetray
from . import i3logging as logging

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from typing import Type, Union, Callable, Optional, Any
    from typing_extensions import Self

class I3Tray(_icetray._I3TrayBase):
    """
    A convenience wrapper around the wrapped c++ class _icetray.I3Tray.
    This class provides keyword-arguments to the AddModule and so forth.
    """

    def __init__(self):
        """
        Creates an I3Tray.
        """
        self.last_added = None
        self.segments_in_order = []
        self.current_condition_stack = []
        _icetray._I3TrayBase.__init__(self)

    def _create_name(self, _type, kind, module_names):
        """
        :param _type: Thing to be added to the tray (module/service/segment)
        :param kind: Type of the thing to be added (Module/Service/Segment)
        :param module_names: List of already-taken names
        """
        n=0
        if hasattr(_type,'func_name'):
            prefix = _type.func_name
        elif hasattr(_type,'__name__'):
            prefix = _type.__name__
        else:
            prefix = str(_type)
        while 1:
            name = "%s_%04d"%(prefix,n)
            if name not in module_names:
                break
            n+=1
        logging.log_debug("Adding Anonymous %s of type '%s' with name '%s'" \
                                 % (kind, _type,name), "I3Tray")
        return name

    def Add(self, _type: "Union[str, Type[_icetray.I3Module], Callable[..., Any]]", _name: "Optional[str]"=None, **kwargs):
        """
        Add a module, service, or segment to the tray.
        """
        method: Optional[Callable] = None
        if isinstance(_type, "".__class__):
            # Search registries for a C++ factory type
            if _icetray.is_module(_type) and _icetray.is_service(_type):
                raise ValueError("'%s' is registered both as a service and module. Use AddModule()/AddService() to disambiguate." % _type)
            elif _icetray.is_module(_type):
                method = self.AddModule
            elif _icetray.is_service(_type):
                method = self.AddService
            else:
                raise ValueError("'%s' is registered neither as a service nor a module." % _type)
        elif callable(_type):
            # Callables are either segments or simple I3Modules
            if hasattr(_type, "__i3traysegment__"):
                method = self.AddSegment
            else:
                method = self.AddModule
        elif inspect.isclass(_type) and _icetray.I3Module in inspect.getmro(_type):
            # Python subclass of I3Module
            method = self.AddModule

        if method is None:
            raise ValueError("%s is not a thing that can be added to an I3Tray" % _type)
        else:
            return method(_type, _name, **kwargs)

    def AddModule(self, _type: "Union[str, Type[_icetray.I3Module], Callable[..., Any]]", _name: "Optional[str]"=None, *, If=None, **kwargs) -> "Self": # type: ignore[override]
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
        #if the script does not provide a name find a suitable name for this module find a suitable name
        if _name is None:
            _name = self._create_name(_type, "Module", self.tray_info.modules_in_order)

        if hasattr(_type, '__i3traysegment__'):
            raise RuntimeError("Trying to add tray segment %s with AddModule. Use AddSegment instead." % _name)

        if inspect.isclass(_type) and _icetray.I3Module not in inspect.getmro(_type):
            raise RuntimeError("Module %s of type %s doesn't inherit from icecube._icetray.I3Module" % (_name, _type))
        try:
            super().AddModule(_type, _name)
            self.last_added = _name
            for k,v in kwargs.items():
                super().SetParameter(_name, k, v)
            # Handle module conditions as a special case
            if If is None and len(self.current_condition_stack) > 0:
                If = self.current_condition_stack[-1]
            if If is not None:
                super().SetParameter(_name, 'If', If)
            return self
        except:
            raise

    def AddService(self, _type: str, _name: "Optional[str]"=None, /, **kwargs):
        """
        Add a service factory to the tray.

        :param _type:
          a string (search for a registered c++ service factory)

        :param _name:
          instance name for this factory

        :param kwargs: parameter values forwarded to the added module

        **Example**::

            tray.AddService("I3GSLRandomServiceFactory", "gslrng")

        """
        if _name is None:
            _name = self._create_name(_type, "Service", self.tray_info.factories_in_order)
        super().AddService(_type, _name)
        self.last_added = _name
        for k,v in kwargs.items():
            super().SetParameter(_name, k, v)
        return self

    def AddSegment(self, _segment, _name: "Optional[str]"=None, *, If=None, **kwargs):
        """
        Add a tray segment to the tray. This is a small scriptlet that can
        autoconfigure some set of modules and services according to a
        predefined configuration.

        :param _segment:
          tray segment (a python callable registered with @icetray.traysegment)

        :param _name:
          base for names of all added modules and services

        :param kwargs: parameter values forwarded to the added segment

        **Example**::

            tray.AddSegment(icetray.ExampleSegment, "example")

        """
        (keys,vals) = (list(kwargs.keys()), list(kwargs.values()))
        fullargspec = inspect.getfullargspec(_segment)
        argnames = fullargspec.args
        if 'If' not in argnames and 'If' in fullargspec[4]:
            argnames.append('If')
        largnames = [a.lower() for a in argnames]

        if _name is None:
            _name = self._create_name(_segment.__name__, "Segment", self.segments_in_order)
        self.segments_in_order.append(_name)

        # Make case-insensitive where possible by matching to signature
        for k in keys:
            if k.lower() not in largnames:
                continue
            keys[keys.index(k)] = argnames[largnames.index(k.lower())]
        kwargs = dict(zip(keys, vals))

        try:
            # if the segment handles If itself, then let it
            # otherwise, save for submodules
            if 'If' in argnames and If is not None:
                self.current_condition_stack.append(None)
                kwargs['If'] = If
            else:
                self.current_condition_stack.append(If)
            return _segment(self, _name, **kwargs)
        finally:
            self.current_condition_stack.pop()

    def SetParameter(self, module, param, value):
        super().SetParameter(module, param, value)
        return self

    def __call__(self, *args: "tuple[str, Any]"):
        assert self.last_added
        for pair in args:
            logging.log_info('%s: %s = %s' % (self.last_added, pair[0], pair[1]), unit='I3Tray')
            super().SetParameter(self.last_added, pair[0], pair[1])
        return self

    def __repr__(self):
        return self.TrayInfo().__repr__()

    def Execute(self, *args):

        assert len(args) < 2, "Too many arguments to Execute()"

        if len(args) == 1:
            super().Execute(args[0])
        else:
            super().Execute()

    def PrintUsage(self, fraction=0.9):
        """
        Pretty-print the time spent in each module, as usually done in the
        I3Module destructor.

        :param fraction: Print out the usage of modules consuming this much of
                         the total runtime.

        :return: Returns a list of keys that were printed.  Implemented for testing.
        """
        usage = self.Usage()
        if len(usage) == 0:
            return
        keys = sorted(usage, key = lambda k: usage[k].usertime + usage[k].systime, reverse = True)
        total_time = sum([ru.usertime + ru.systime for ru in usage.values()])
        acc_time = 0.
        print('-'*99)
        printed_keys = list()
        for k in keys:
            pusage = usage[k]
            print("%40s: %6u calls to physics %9.2fs user %9.2fs system" \
                  % (k, pusage.ncall, pusage.usertime, pusage.systime))
            acc_time += pusage.systime + pusage.usertime
            printed_keys.append(k)
            if total_time > 0 and acc_time/total_time > fraction:
                break
        print('-'*99)
        return printed_keys

