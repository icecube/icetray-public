#!/usr/bin/env python3

"""
Run all registered converters through both HDFWriter and ROOTWriter
"""

import os
import shutil
import warnings

import icecube  # noqa: F401
from I3Tray import I3Tray
from icecube import dataclasses, icetray, tableio
from icecube.tableio import I3BroadcastTableService

# Since there is no clear way to inspect converters in pythonland,
# we can just put a list of S-Frame converters here I guess
sframe_converters = ['I3CorsikaInfo', 'I3PrimaryInjectorInfo','I3TopInjectorInfo', 'I3GenieInfo']

# hobo "from icecube import *", import everything to run as many converters as possible
for path in sorted(os.listdir(os.environ['I3_BUILD']+'/lib/icecube')):
	if 'ml_suite' in path or path[0]=='_':
		continue

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
	for typus, _ in tableio.registry.I3ConverterRegistry.registry.items():
		name = typus.__name__
		if name in sframe_converters:
			continue
		if name in frame:
			continue

		try:
			obj = potemkin_object(typus)
		except Exception:
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

ol = icetray.I3Logger.global_logger
icetray.I3Logger.global_logger = icetray.I3NullLogger()
with warnings.catch_warnings():
	warnings.simplefilter("ignore")
	tray.Execute(1)
icetray.I3Logger.global_logger = ol

try:
	import h5py
	with h5py.File("test_converters.hdf5", "r") as hdf:
		for name, obj in fill_frame.objects.items():
			table = hdf[name]
			assert table.len() == 1
			row = table[0]
			for name in row.dtype.names:
				if hasattr(obj, name) and isinstance(getattr(obj, name), type(row[name])):
					assert getattr(obj, name) == row[name]
except ImportError:
	icetray.logging.log_warn("Read test requires python3-h5py. Install it via pip/apt/yum/brew.")
	pass

try:
	shutil.rmtree(outfiles[0])
except OSError:
	pass
for outfile in outfiles[1:]:
	try:
		os.unlink(outfile)
	except OSError:
		pass
