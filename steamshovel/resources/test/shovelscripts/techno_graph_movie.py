import sys
import operator

from icecube import shovelart
from icecube.dataclasses import I3DOMLaunch

class GraphInsetMovie:

	def __init__( self, window, app ):
		self.window = window
		self.files = app.files

	def setup( self ):
		fcount = self.window.movieEngine.nframes
		tmin = self.window.movieEngine.starttime
		tmax = self.window.movieEngine.endtime
		# Add an uncalibrated waveform graph to the scenario, disabled for now
		scenario = self.window.gl.scenario
		self.arthandle = scenario.add('Uncalibrated waveform', ['InIceRawData'])
		scenario.setIsActive( self.arthandle, False )
		windims = (self.window.movieEngine.width, self.window.movieEngine.height)
		scenario.setOverlaySizeHints( self.arthandle, [shovelart.OverlaySizeHint(0,windims[1]/2, windims[0]/2, windims[1]/2)])

		# Find the five most heavily charge DOMs
		frame = self.files.selectedFrame
		waves = frame['OfflinePulses']
		activity_list = []
		for (dom, contents) in waves:
			if len(contents) == 0: continue
			charge = sum( [ d.charge for d in contents] )
			time = contents[0].time
			activity_list.append( (charge, time, dom) )
		
		# Sort the top 5 DOMs by the order of their appearance
		top5 = sorted( activity_list, key=operator.itemgetter(0), reverse = True )[0:5]
		# convert to ( frameid, dom) pairs 
		top5 = [ ( int( ((time-tmin)/(tmax-tmin) * fcount ) ), dom) for (charge,time,dom) in top5 ]
		# re-sort by frame ids
		self.top5 = sorted( top5 )
		self.doms = []


	def __call__( self, frame ):
		print >>sys.stderr, frame
		if( frame == 0 ):
			self.setup()

		while( len(self.top5) > 0 and frame == self.top5[0][0] ):
			self.window.gl.scenario.setIsActive( self.arthandle, True )
			self.doms.append(self.top5[0][1])
			self.window.gl.scenario.changeSetting( self.arthandle, "OMKeys", self.doms )
			del self.top5[0]




