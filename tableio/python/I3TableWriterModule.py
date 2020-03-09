# -*- coding: utf-8 -*-
# 
# copyright  (C) 2010
# The Icecube Collaboration
# 
# $Id: I3TableWriterModule.py 129552 2015-02-25 13:45:04Z jvansanten $
# 
# @version $Revision: 129552 $
# @date $LastChangedDate: 2015-02-25 08:45:04 -0500 (Wed, 25 Feb 2015) $
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: jvansanten $
# 

from icecube.icetray import I3ConditionalModule, vector_string
from icecube.tableio import I3TableService, I3Converter, I3ConverterBundle, \
    I3TableWriterWorker, I3ConverterRegistry, I3ConverterMill, vector_I3ConverterMillPtr, \
    I3BroadcastTableService
from icecube import dataclasses
import re,warnings

class DefaultConverter(I3Converter):
    """ The user passes this to indicate that the default converter should be used.
        this converter doesn't actually work.
	"""
    def __init__(self):
        I3Converter.__init__(self)

default = DefaultConverter()



class I3TableWriter(I3ConditionalModule):
    def __init__(self,context):
        I3ConditionalModule.__init__(self,context)
        self.AddParameter('TableService','The I3TableService to receive output.',None)
        self.AddParameter('SubEventStreams','The names of the SubEvent streams to be booked',[""])
        self.AddParameter('Keys','A list or dict of FrameObject keys to convert',None)
        self.AddParameter('Types','A list or dict of types to convert',None)
        self.AddParameter('BookEverything','Book absolutely everything in the frame, \
using the default converters. This has the tendency to produce very, very large files, \
and is almost certainly not what you actually want to do.', False)
        self.writer = None

    def _get_tableservice(self):
        """Get the table service (passed v3-style as a python object)"""
        table_service = self.GetParameter('TableService')
        # if a list is passed, wrap it in a repeater service
        if isinstance(table_service, list):
            table_service = I3BroadcastTableService(tuple(table_service))
        elif not isinstance(table_service, I3TableService):
            raise TypeError("TableService must be an instance of I3TableService (got %s instead)" % table_service)
        self.table_service = table_service
    
    def _transform_keyitem(self,item):
        if isinstance(item,tuple):
            if len(item) == 1:
                return dict(key=item[0])
            elif len(item) == 2:
                return dict(key=item[0],converter=item[1])
            elif len(item) == 3:
                return dict(key=item[0],converter=item[1],name=item[2])
            else:
                raise ValueError("Keys must be 2- or 3-tuples (got %s instead)" % item)
        elif isinstance(item,str):
            return dict(key=item)
        elif isinstance(item,dict):
            #sanitize your inputs
            dictus = dict()
            for k in ['key','converter','name']:
                if k in item: dictus[k] = item[k]
            return dictus
        else:
            raise TypeError("Keys must be dicts, tuples, or strings.")
    
    # get the base class of all boost::python wrapped objects 
    # (<type 'Boost.Python.class'>)
    bp_class = type(I3TableService)
        
    def _transform_typeitem(self,item):
        typus = None
        dictus = None
        if isinstance(item,tuple):
            if len(item) == 1:
                dictus = dict(type=item[0])
            elif len(item) == 2:
                dictus = dict(type=item[0],converter=item[1])
            else:
                raise ValueError("Types must be 1- or 2-tuples (got %s instead)" % item)
        elif isinstance(item,dict):
            #sanitize your inputs
            dictus = dict()
            for k in ['type','converter']:
                if k in item: dictus[k] = item[k]
        else:
            dictus = dict(type=item)
        typus = dictus['type']
        if type(typus) != self.bp_class:
            raise TypeError("Type must be an instance of Boost.Python.class (got %s instead)" % typus)
        return dictus
                
    def _parse_args(self,arg,transformer):
        if arg is None:
            return []
        if isinstance(arg,dict):
            arg = list(arg.items())
        try:
            arg = [transformer(item) for item in arg]
        except TypeError:
            raise TypeError("Arguments must be passed as something list-like.")
        valid_name = re.compile("^[a-zA-Z_][a-zA-Z0-9_]*$") 
        for item in arg:
            converter = item.get('converter',None)
            if not converter is None:
                # if this is a list of converters, make a bundle
                if isinstance(converter,list):
                    converter = I3ConverterBundle(converter)
                    item['converter'] = converter
                elif not isinstance(converter,I3Converter):
                    raise TypeError("In '%s': converter must be an instance of I3Converter")
            name = item.get('name',None)
            if not name is None:
                if not valid_name.match(name):
                    raise ValueError("'%s' is not a valid table name. Table names must contain only letters, numbers, and underscores and may not start with a number" % name)    
        return arg
                
    def Configure(self):
        self._get_tableservice()
        
        streams = vector_string()
        streams.extend(self.GetParameter('SubEventStreams'))

        # The grand configuration!
        keys = self.GetParameter('Keys')
        types = self.GetParameter('Types')
        data_flood = self.GetParameter('BookEverything')
        empty = lambda t: (t is None) or (len(t) == 0)
        
        if (not data_flood) and empty(keys) and empty(types):
            raise ValueError("""You must specify which frame objects should be booked by setting Keys \
and/or Types. If you really want to dump every object from every frame, \
set BookEverything = True.""")
        
        # convert whatever was passed as 'Keys' to a list of dicts
        keys = self._parse_args(keys,self._transform_keyitem)
        
        # convert whatever was passed as 'Types' to a list of dicts
        types = self._parse_args(types,self._transform_typeitem)

        if data_flood:
            warnings.warn("""
I3TableWriter is now going to book everything it can find in the frame. This \
has the tendency to produce very, very large files, and is almost certainly \
not what you want to do.

To book only the frame objects you're interested in, pass a list of keys as \
the `keys' parameter, or a list of types as the `types' parameter when \
configuring I3TableWriter. You can find a tutorial in \
$I3_BUILD/doc/projects/tableio/howto.html .
""")
            # add type-specifications for everything we know about that wasn't included in 'Types'            
            all_types = I3ConverterRegistry.registry.keys()
            # remove GCD types if they exist
            for t in [dataclasses.I3Geometry,dataclasses.I3DetectorStatus,dataclasses.I3Calibration]:
                if t in all_types:
                    del all_types[t]
            specified_types = [d['type'] for d in types]
            for t in all_types:
                if not t in specified_types:
                    types.append(dict(type=t, converter=default))

        # now, pull in all of the registered converters from Python-land
        converter_list = vector_I3ConverterMillPtr()
        defaults = dict(I3ConverterRegistry.defaults)

        # if no default was registered for a type, pick the first one registered
        # in the normal converter registry.
        for typus, converters in I3ConverterRegistry.registry.items():
            if typus not in defaults:
                defaults[typus] = converters[0]

        for converter in defaults.values():
            # only instantiate the converter registered as default
            converter_list.append(I3ConverterMill(converter))

        self.writer = I3TableWriterWorker(self.table_service, converter_list, streams)
        tablespec = I3TableWriterWorker.TableSpec
        typespec = I3TableWriterWorker.TypeSpec
        
        for item in keys:
            key = item.pop('key')
            # if the user passed 'default', remove it and let the writer figure
            # out what the default is
            if 'converter' in item and item['converter'] == default:
                del item['converter']
            t = tablespec(**item)
            self.writer.add_object(key,t)

        for item in types:
            typus = item.pop('type')
            if 'converter' in item and item['converter'] == default:
                del item['converter']
            t = tablespec(**item)
            self.writer.add_type(typespec(typus),t)

    def Process(self):
        frame = self.PopFrame()
        self.writer.convert(frame)
        self.PushFrame(frame)
        return True
        
    def Finish(self):        
        if self.writer is not None:
            self.writer.finish()

