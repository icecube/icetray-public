from .mplart import MPLArtist
from icecube.shovelart import *
from icecube import dataclasses
from matplotlib.colors import colorConverter as mplcol

from icecube.wavereform.plotter import WaveformPlotter

def colorize(sequence, cmap='jet'):
    from matplotlib.colors import Normalize
    from matplotlib import cm
    norm = Normalize(vmin=0, vmax=len(sequence)-1)
    cmap = getattr(cm, cmap)
    for i, s in enumerate(sequence):
        yield cmap(norm(i)), s

class RecoPulseWaveform( MPLArtist ):

    numRequiredKeys = 1
    def isValidKey( self, frame, key_idx, key ):
        '''Accept I3RecoPulseSeriesMap and I3RecoPulseSeriesMapMask'''
        tn = frame.type_name(key)
        return "I3RecoPulse" in tn and "Map" in tn
   
    def __init__(self):
        MPLArtist.__init__(self)
        self.defineSettings( {'OMKeys':OMKeySet(),
                              'legend': False,
                              'logscale': False
                          } )

    
    def create_plot( self, frame, fig ):
        dkey = list(self.keys())[0]
        pmap = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, dkey)
        calib = frame['I3Calibration']
        status = frame['I3DetectorStatus']
        keys = self.setting( 'OMKeys' )
        
        axes = fig.add_subplot(1,1,1)
        # protect against missed clicks
        keys = [k for k in keys if k in pmap]
        # FIXME: think of a better way to pick colors and line styles
        for color, k in colorize(keys):
            pulses = pmap[k]
            cal = calib.dom_cal[k]
            stat = status.dom_status[k]
            label = 'OM(%d,%d): %.1f PE' % (k.string, k.om, sum(p.charge for p in pulses))
            
            trange = (min(p.time for p in pulses), max(p.time for p in pulses))
            wf = dataclasses.I3Waveform()
            WaveformPlotter.plot_pulses(axes, trange, wf, pulses, cal, stat, color=color, label=label)
            if( 'I3Geometry' in frame ):
                self.addOverlayLine( frame['I3Geometry'].omgeo[k].position, mplcol.to_rgb(color) )
            
        axes.set_xlabel('Time [microseconds]', fontsize='smaller')
        axes.set_ylabel('Toroid output voltage [mV]', fontsize='smaller')
        if (self.setting('logscale')) :
            axes.set_yscale('log')
        
        if( self.setting("Animated") ):
            mintime = min(pulses[0].time for pulses in pmap.values())-100
            self.timeline = axes.axvline( mintime/1e3, color='black' )
        
        if( self.setting('legend') ):
            axes.legend(loc='best', prop={'size':'small'})
    
    def update_plot( self, time ):
        self.timeline.set_xdata(time/1e3)
