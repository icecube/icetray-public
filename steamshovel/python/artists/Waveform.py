from .mplart import MPLArtist
from icecube.shovelart import *
from icecube import dataclasses
from icecube.icetray import I3Units
import matplotlib.gridspec as gridspec
from matplotlib.colors import colorConverter as mplcol
from matplotlib import rcParams
from cycler import cycler
from itertools import cycle
import numpy

class Waveform(MPLArtist):

    numRequiredKeys = 1
    def isValidKey(self, frame, key_idx, key):
        'Accept I3DOMLaunchSeriesMap'
        tn = frame.type_name(key)
        return ("I3DOMLaunch" in tn or "I3Waveform" in tn) and "Map" in tn

    def __init__(self):
        MPLArtist.__init__(self)
        self.defineSettings( {'OMKeys':OMKeySet(), 'legend':False } )

    @staticmethod
    def _stepped_plot( axis, time, step, trace, **kw ):

        def stepped_iter():
            t = time
            for y in trace:
                yield t, y
                t += step
                yield t, y
        x, y = list(zip( *stepped_iter() ))

        axis.plot( x, y, **kw )


    def _plot_atwd( self, axis, time, readout, **kw ):
        if( len(readout) > 0 ):
            self._stepped_plot( axis, time, 3.33333, readout, **kw )

    def _plot_fadc( self, axis, launch, **kw ):
        time = launch.time
        if( launch.lc_bit ):
            readout = launch.raw_fadc
            if( len(readout) > 0 ):
                self._stepped_plot( axis, time, 25, readout, **kw )
        else:
            max_sample = launch.charge_stamp_highest_sample
            rawcharge = launch.raw_charge_stamp
            starttime = time + 25*(max_sample-1)
            self._stepped_plot( axis, starttime, 25, rawcharge, **kw )

    def create_plot( self, frame, fig ):
        launchmap = frame[list(self.keys())[0]]
        if isinstance(launchmap, dataclasses.I3DOMLaunchSeriesMap):
            self.create_plot_raw( frame, fig )
        else:
            self.create_plot_calibrated( frame, fig )

    def create_plot_calibrated( self, frame, fig ):
        waveformmap = frame[list(self.keys())[0]]
        oms = self.setting( 'OMKeys' )
        
        axis = fig.add_subplot(111)
        
        geom = None
        if( 'I3Geometry' in frame.keys() ):
            geom = frame['I3Geometry']
        
        props = cycle(cycler(rcParams['axes.prop_cycle']))
        for omkey in oms:
            if omkey not in waveformmap:
                continue
            waveforms = waveformmap[omkey]

            prop = next(props)
            label = 'OM({},{})'.format(omkey.string, omkey.om)
            for wf in waveforms:
                if wf.status != wf.VIRGINAL:
                    pass
                if not ((wf.source == wf.ATWD) and (wf.channel == 0)):
                    continue
                trace = numpy.array(wf.waveform) / I3Units.mV
                self._stepped_plot( axis, wf.time, wf.bin_width, trace, label=label, **prop)
                label = "__nolabel"
                break
            
            if( geom ):
                self.addOverlayLine( geom.omgeo[omkey].position, mplcol.to_rgb(prop['color']) )
        if( self.setting('legend') ):
            axis.legend()
            

    def create_plot_raw( self, frame, fig ):
        
        launchmap = frame[list(self.keys())[0]]
        oms = self.setting( 'OMKeys' )
        legend = ''

        # Determine ahead of time how many subplots are needed
        numchannels = 0
        def maxchan( launch ):
            return max( [0] + [i for i, d in enumerate( launch.raw_atwd ) if len(d) > 0 ] )

        for omkey in oms:
            if omkey not in launchmap:
                continue
            launches = launchmap[omkey]
            for launch in launches:
                numchannels = max( numchannels, maxchan(launch) + 1 )

        # create one subplot for FADC, N subplots for ATWD, and one for the legend, if requested
        speclen = numchannels + 1
        if( self.setting('legend') ):
            speclen += 1
        gs = gridspec.GridSpec(speclen,1)
        legendary = []

        fadc = fig.add_subplot(gs[0,0])
        fadc.set_title( 'FADC' )

        atwd = [None]*numchannels
        for i in range(0,numchannels):
            atwd[i] = fig.add_subplot(gs[i+1,0], sharex=fadc)
            atwd[i].set_title( 'ATWD Ch. ' + str(i) )

        geom = None
        if( 'I3Geometry' in frame.keys() ):
            geom = frame['I3Geometry']

        props = cycle(cycler(rcParams['axes.prop_cycle']))
        for omkey in oms:
            if omkey not in launchmap:
                continue
            launches = launchmap[omkey]

            prop = next(props)
            label = str(omkey)
            for launch in launches:
                self._plot_fadc( fadc, launch, label=label, **prop)
                # change the label to None so that only one label appears for any omkey
                label = None
                for i in range(0,numchannels):
                    self._plot_atwd( atwd[i], launch.time, launch.raw_atwd[i],
                                     **prop )

            if( geom ):
                self.addOverlayLine( geom.omgeo[omkey].position, mplcol.to_rgb(prop['color']) )

        if( self.setting('legend') ):
            fadc.legend(bbox_to_anchor=(0, 0, 1, 1),
                        bbox_transform=fig.transFigure,
                        loc=3, ncol=2, mode='expand', prop={'size':'small'})
        

