#!/usr/bin/env python

import platform

hosts = ['kipple', 'morax']
for h in hosts:
    if h in platform.node():
        print("This test is disabled on this host: " + platform.node())
        quit()
    

"""
Run all registered converters through both HDFWriter and ROOTWriter
"""


#Since there is no clear way to inspect converters in pythonland,
#we can just put a list of S-Frame converters here I guess
sframe_converters = ['I3CorsikaInfo','I3PrimaryInjectorInfo']

from icecube import icetray, dataclasses, tableio, phys_services, dataio
from I3Tray import I3Tray

# hobo "from icecube import *"
import icecube, os
for path in sorted(os.listdir(os.path.dirname(icecube.__file__))):
	try:
		__import__("icecube."+os.path.splitext(path)[0])
	except ImportError:
		continue
	except RuntimeError:
		# e.g. dlopen() failure
		continue

def potemkin_object(klass):
	"""
	return an instance of `klass`. If `klass` is a container, fill it with stuff.
	"""
	obj = klass()
	# handle some classes of objects that need special contents
	if isinstance(obj, str):
		obj = "foo"
	elif isinstance(obj, dataclasses.I3Waveform):
		obj.waveform.extend(range(128))
	
	# fill standard containers
	if hasattr(klass, "__key_type__"):
		# it's a map
		value = potemkin_object(klass.__value_type__())
		obj[potemkin_object(klass.__key_type__())] = value
	elif any((hasattr(obj, "add_"+name) for name in ("root", "primary"))):
		if hasattr(obj, "add_root"):
			add = obj.add_root
		else:
			add = obj.add_primary
		node = potemkin_object(klass.__value_type__())
		add(node)
	elif hasattr(klass, "__value_type__"):
		obj.append(potemkin_object(klass.__value_type__()))
	return obj

def fake_event_header(frame):
	header = dataclasses.I3EventHeader()
	header.run_id = 0
	header.event_id = fake_event_header.event_id
	fake_event_header.event_id += 1
	frame['I3EventHeader'] = header
fake_event_header.event_id = 0

def fill_frame(frame):
	for typus, converters in tableio.registry.I3ConverterRegistry.registry.items():
		name = typus.__name__
		if name in sframe_converters:
		    continue
		if name in frame:
			continue
		converter = converters[0]()
		try:
			obj = potemkin_object(typus)
		except Exception as e:
			# doesn't have a default constructor
			continue
		frame[name] = obj
		fill_frame.objects[name] = obj
		
fill_frame.objects = dict()
tray = I3Tray()

tray.Add("I3InfiniteSource")
tray.Add(fake_event_header, Streams=[icetray.I3Frame.DAQ])
tray.Add("I3NullSplitter", "nullsplit")
tray.Add(fill_frame)

from icecube.tableio import I3BroadcastTableService

tablers = [tableio.I3CSVTableService('test_converters')]
outfiles = ['test_converters']
try:
	from icecube.hdfwriter import I3HDFTableService
	tablers.append(I3HDFTableService("test_converters.hdf5", 6, 'w'))
	outfiles.append('test_converters.hdf5')
except ImportError:
	pass
try:
	from icecube.rootwriter import I3ROOTTableService
	tablers.append(I3ROOTTableService("test_converters.root"))
	outfiles.append('test_converters.root')
except ImportError:
	pass
if len(tablers) == 1:
	tabler = tablers[0]
else:
	tabler = I3BroadcastTableService(tuple(tablers))

tray.Add(tableio.I3TableWriter,
    TableService=tabler,
    BookEverything=True,
    SubEventStreams=["nullsplit"],
)

tray.Execute(1)


try:
	import tables
	with tables.open_file("test_converters.hdf5") as hdf:
		for name, obj in fill_frame.objects.items():
			table = hdf.get_node('/' + name)
			assert table.nrows == 1
			row = table[0]
			for name in row.dtype.names:
				if hasattr(obj, name) and type(getattr(obj, name)) == type(row[name]):
					assert getattr(obj, name) == row[name]
except ImportError:
	pass

import shutil, os
shutil.rmtree(outfiles[0])
for outfile in outfiles[1:]:
	os.unlink(outfile)

