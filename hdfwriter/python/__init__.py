from icecube.load_pybindings import load_pybindings
from icecube import icetray, tableio, dataio

load_pybindings(__name__, __path__)

@icetray.traysegment_inherit(tableio.I3TableWriter,
    removeopts=('TableService',))
def I3HDFWriter(tray, name, Output=None, CompressionLevel=6, **kwargs):
	"""Tabulate data to an HDF5 file.

	:param Output: Path to output file
	:param CompressionLevel: gzip compression to apply to each table
	"""
	
	if Output is None:
		raise ValueError("You must supply an output file name!")
	
	# Ready file for staging out if so configured
	if 'I3FileStager' in tray.context:
		stager = tray.context['I3FileStager']
		Output = stager.GetWriteablePath(Output)
	
	tabler = I3HDFTableService(Output, CompressionLevel)
	tray.AddModule(tableio.I3TableWriter, name, TableService=tabler,
	    **kwargs)

@icetray.traysegment_inherit(I3HDFWriter)
def I3SimHDFWriter(tray, name, RunNumber=0, **kwargs):
	"""
	Tabulate untriggered data (Q frames only, no event headers)
	
	:param RunNumber: run ID to use in made-up headers. This should be unique
	                  in each file.
	"""

	if "SubEventStreams" in kwargs:
		raise ArgumentError("SubEventStreams cannot be set for I3SimHDFWriter")

	from icecube import icetray, dataclasses, phys_services
	def fake_event_header(frame):
		header = dataclasses.I3EventHeader()
		header.run_id = RunNumber
		header.event_id = fake_event_header.event_id
		fake_event_header.event_id += 1
		frame['I3EventHeader'] = header
	fake_event_header.event_id = 0
	tray.Add(fake_event_header, Streams=[icetray.I3Frame.DAQ])
	
	tray.Add("I3NullSplitter",SubEventStreamName="SimHDFWriter")

	tray.Add(I3HDFWriter, name, SubEventStreams=["SimHDFWriter"], **kwargs)
		
		


# clean the local dictionary
del icetray
