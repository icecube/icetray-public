#!/usr/bin/env python

from icecube import icetray,dataclasses,tableio,dataio
import array
import unittest
import sys

try:
	import numpy
	have_numpy = True
except ImportError:
	print("Numpy isn't installed, skipping numpy-specific tests")
	have_numpy = False

try:
	sys.maxsize
except AttributeError:
	sys.maxsize = sys.maxint
	
class I3PythonConverterTest(unittest.TestCase):
	def setUp(self):
		desc = tableio.I3TableRowDescription()
		
		self.types = {
		              'signed_char':   tableio.types.Int8,
		              'unsigned_char': tableio.types.UInt8,
		              'signed_short':  tableio.types.Int16,
		              'unsigned_short':tableio.types.UInt16,
		              'signed_int':    tableio.types.Int32,
		              'unsigned_int':  tableio.types.UInt32,
		              'signed_long':   tableio.types.Int64,
		              'unsigned_long': tableio.types.UInt64,
		              'float':         tableio.types.Float32,
		              'double':        tableio.types.Float64,
		              'bool':          tableio.types.Bool}
		# HACK: dial back longs on 32 bit systems for testing purposes
		if sys.maxsize == 2**31-1:
				self.types['signed_long'] = tableio.types.Int32
				self.types['unsigned_long'] = tableio.types.UInt32
		self.codes = {
		              'signed_char':   'b',
		              'unsigned_char': 'B',
		              'signed_short':  'h',
		              'unsigned_short':'H',
		              'signed_int':    'i',
		              'unsigned_int':  'I',
		              'signed_long':   'l',
		              'unsigned_long': 'L',
		              'float':         'f',
		              'double':        'd',
		              'bool':          'o'}
		self.conversions = {int:   ['c','b','B','h','H','i'],
		                    int:  ['I','h','H'],
		                    float: ['f','d'],
		                    bool:  ['o']}
		
		
		for t,code in self.types.items():
			desc.add_field(t,code,'','')
			desc.add_field('%s_vec'%t,code,'','',128)
		
		desc.add_field('trigger_type',tableio.I3Datatype(dataclasses.I3DOMLaunch.TriggerType),'','')
		
		self.desc = desc
		self.rows = tableio.I3TableRow(desc,1)
		
	def testKeys(self):
		fields = list(self.desc.field_names);
		self.assertEquals( fields, list(self.rows.keys()) )
	def testEnum(self):
		field = 'trigger_type'
		val = dataclasses.I3DOMLaunch.TriggerType.SPE_DISCRIMINATOR_TRIGGER
		self.rows[field] = val
		got = self.rows[field]
		self.assertEquals( int(val), got )
	def testIntegerScalars(self):
		types = self.conversions[int]
		reverse_types = dict([(b,a) for a,b in self.codes.items()])
		for i,t in enumerate(types):
			if t == 'c': continue # no from_python converter for char...ah well
			val = i+7
			field = reverse_types[t]
			self.rows[field] = val
			got_val = self.rows[field]
			self.assertEquals( val, got_val, "Set field '%s' to %d, got %d back."%(field,val,got_val))
	def testLongScalars(self):
		import sys
		field,val = 'signed_long',sys.maxsize
		self.rows[field] = val;
		got_val = self.rows[field]
		self.assertEquals( val, got_val, "Set field '%s' to %d, got %d back."%(field,val,got_val))
		bad_news = lambda: self.rows.set(field,sys.maxsize+1)
		self.assertRaises(OverflowError, bad_news)
	def testArray(self):
		field = 'signed_int_vec'
		arr = array.array(self.codes[field[:-4]],range(128))
		self.rows[field] = arr
		got = self.rows[field]
		self.assertEquals( list(arr), got )
	if have_numpy:
		def testNumpy(self):
			field = 'signed_long_vec'
			arr = numpy.array(range(128),self.codes[field[:-4]])
			self.rows[field] = arr
			got = self.rows[field]
			self.assertEquals( list(arr), got )
			import sys
			# try passing an array in byte-swapped order
			if (sys.byteorder == 'little'):
				swapped = '>'
			else:
				swapped = '<'
			arr = numpy.array(range(128), (swapped + self.codes[field[:-4]]) )
			bad_news = lambda: self.rows.set(field,arr)
			self.assertRaises(RuntimeError,bad_news)
	def testVectorInt(self):
		vec = dataclasses.I3VectorInt()
		for r in range(128): vec.append(r)
		field = 'signed_int_vec'
		self.rows[field] = vec
		got = self.rows[field]
		self.assertEquals( list(range(128)), got )
	def testVectorDouble(self):
		vec = dataclasses.I3VectorDouble()
		for r in range(128): vec.append(r+3)
		field = 'double_vec'
		self.rows[field] = vec
		got = self.rows[field]
		for a,b in zip(vec,got):
			self.assertAlmostEqual(a,b)
		

class DOMLaunchBookie(tableio.I3Converter):
    booked = dataclasses.I3DOMLaunchSeriesMap
    dl = dataclasses.I3DOMLaunch
    def CreateDescription(self,dlsm):
        desc = tableio.I3TableRowDescription()
        # make this a "ragged" table
        desc.is_multi_row = True
        # grab an example DOMLaunch
        dl = self._get_nonempty(dlsm)
        # unrolling fields
        desc.add_field('string',tableio.types.Int16,'','String number')
        desc.add_field('om',tableio.types.UInt16,'','OM number')
        desc.add_field('index',tableio.types.UInt16,'','Index within the vector')
        # DOMLaunch fields
        desc.add_field('start_time',tableio.types.Float32,'ns','')
        desc.add_field('pedestal_sub',tableio.types.Bool,'','Has the pedestal been subtracted?')
        desc.add_field('lc_bit',tableio.types.Bool,'','')
        desc.add_field('trigger_type',tableio.I3Datatype(self.dl.TriggerType),'','')
        desc.add_field('trigger_mode',tableio.I3Datatype(self.dl.TriggerMode),'','')
        desc.add_field('raw_charge_stamp',tableio.types.Int32,'','',len(dl.GetRawChargeStamp()))
        for i in range(3):
            desc.add_field('raw_atwd_%d'%i,tableio.types.Int32,'counts','Uncalibrated ATWD digitizer counts',128)
        desc.add_field('raw_fadc',tableio.types.UInt16,'counts','Uncalibrated fADC digitizer counts',256)
        return desc
    def _get_nonempty(self,dlsm):
        dl = None
        for vec in dlsm.values():
            if len(vec) == 0: continue
            dl = vec[0]
            if dl != None: break
        return dl
    def GetNumberOfRows(self,frameobj):
        nrows = 0
        for vec in frameobj.values():
            nrows += len(vec)
        return nrows
    def Convert(self,dlsm,rows,frame):
        rowno = 0
        for omkey,dl_series in dlsm.items():
            string = omkey.GetString()
            om = omkey.GetOM()
            for i,domlaunch in enumerate(dl_series):
                rows.current_row = rowno
                rowno += 1
                rows['string']           = string
                rows['om']               = om
                rows['index']            = i
                rows['pedestal_sub']     = domlaunch.GetIsPedestalSub()
         
                rows['start_time']       = domlaunch.GetStartTime()
                rows['raw_charge_stamp'] = domlaunch.GetRawChargeStamp()
                rows['pedestal_sub']     = domlaunch.GetIsPedestalSub()
                rows['lc_bit']           = domlaunch.GetLCBit()
                rows['trigger_type']     = domlaunch.GetTriggerType()
                rows['trigger_mode']     = domlaunch.GetTriggerMode()
                # we can pass I3VectorInts
                rows['raw_atwd_0']       = domlaunch.GetRawATWD(0)
                # ooooor arrays with the proper typecode
                rows['raw_atwd_1']       = array.array('i',domlaunch.GetRawATWD(1))
                # or even numpy ndarrays!
                rows['raw_atwd_2']       = numpy.array(domlaunch.GetRawATWD(2),'i')
    
                rows['raw_fadc']         = array.array('H',domlaunch.GetRawFADC())
        return rowno

        
class WaveformBookie(tableio.I3Converter):
    booked = dataclasses.I3WaveformSeriesMap
    wf = dataclasses.I3Waveform
    def CreateDescription(self,dlsm):
        desc = tableio.I3TableRowDescription()
        # make this a "ragged" table
        desc.is_multi_row = True
        # grab an example DOMLaunch
        wf = self._get_nonempty(dlsm)
        # unrolling fields
        desc.add_field('string',tableio.types.Int16,'','String number')
        desc.add_field('om',tableio.types.UInt16,'','OM number')
        desc.add_field('index',tableio.types.UInt16,'','Index within the vector')
        # DOMLaunch fields
        desc.add_field('start_time',tableio.types.Float32,'ns','')
        desc.add_field('bin_width',tableio.types.Float32,'ns','')
        desc.add_field('source',tableio.I3Datatype(self.wf.Source),'','')
        desc.add_field('waveform',tableio.types.Float32,'','',len(wf.waveform))
        return desc
    def _get_nonempty(self,dlsm):
        dl = None
        for vec in dlsm.values():
            if len(vec) == 0: continue
            dl = vec[0]
            if dl != None: break
        return dl
    def GetNumberOfRows(self,frameobj):
        nrows = 0
        for vec in frameobj.values():
            nrows += len(vec)
        return nrows
    def Convert(self,wfsm,rows,frame):
        rowno = 0
        for omkey,wf_series in wfsm.items():
            string = omkey.GetString()
            om = omkey.GetOM()
            for i,wf in enumerate(wf_series):
                rows.current_row = rowno
                rowno += 1
                rows['string']           = string
                rows['om']               = om
                rows['index']            = i
                
                rows['start_time']       = wf.start_time
                rows['bin_width']        = wf.bin_width
                rows['source']           = wf.source
                rows['waveform']         = wf.waveform
        return rowno
        

try:
	from icecube import hdfwriter
	have_hdf = True
except ImportError:
	print('hdfwriter is not available, skipping hdf-specific tests')
	have_hdf = False
if have_hdf:
	class I3TableWriterPythonModuleTest(unittest.TestCase):
			"""Test the option-parsing magic."""
			def setUp(self):
				from icecube import icetray,tableio,dataclasses,hdfwriter
				from icecube.tableio import I3TableWriter
				import tempfile
				from I3Tray import I3Tray, load
				from icecube import phys_services 
				
				tray = I3Tray()
				tray.AddModule("I3InfiniteSource","streams", Stream=icetray.I3Frame.Physics)
				self.tray = tray
				self.tempfile = tempfile.NamedTemporaryFile()
				self.hdf_service = hdfwriter.I3HDFTableService(self.tempfile.name,0)
				self.target = I3TableWriter
				self.bookie = DOMLaunchBookie()
				
			def tearDown(self):
				# self.hdf_service.CloseFile()
				self.tempfile.close() # implicit delete
				
			def testNoArgs(self):
				"""TableService is a required argument"""
				self.tray.AddModule(self.target,'scribe')
				
				self.assertRaises(TypeError,self.tray.Execute)
				# self.tray.Execute()
			def testNotATable(self):
				"""Things that are not TableServices are rejected"""
				self.tray.AddModule(self.target,'scribe',
					tableservice = 'foo',
					keys = ['I3EventHeader','InIceRawData']
					)
				
				self.assertRaises(TypeError,self.tray.Execute)
				# self.tray.Execute()
			def testKeyList(self):
				"""A simple list of keys, no error"""
				self.tray.AddModule(self.target,'scribe',
					tableservice = self.hdf_service,
					keys = ['I3EventHeader','InIceRawData']
					)
				
				self.tray.Execute(1)
			def testKeyDict(self):
				"""A dict key: booker"""
				self.tray.AddModule(self.target,'scribe',
				tableservice = self.hdf_service,
				keys = {'InIceRawData': self.bookie}
				)
				
				self.tray.Execute(1)
			def testKeyTuples(self):
				"""Tuples of (key, booker)"""
				self.tray.AddModule(self.target,'scribe',
					tableservice = self.hdf_service,
					keys = [('InIceRawData', self.bookie),('InIceRawData',None)] # repeat with default booker
					)
				
				self.tray.Execute(1)
		
def test(fname='/Users/jakob/Documents/IceCube/nugen_nue_ic80_dc6.001568.000000.hits.001.1881140.domsim.001.2028732.i3.gz'):
	f = dataio.I3File(fname)
	fr = f.pop_physics()
	dl = fr['InIceRawData'].values()[0][0]
	booker = DOMLaunchBookie()

	test.desc = booker.CreateDescription(dl)
	# return desc
	n = booker.GetNumberOfRows(dl)
	rows = tableio.I3TableRow(test.desc,n)
	n_written = booker.Convert(dl,rows,fr)
	f.close()
	return rows
	
if __name__ == "__main__":
	unittest.main()
	
