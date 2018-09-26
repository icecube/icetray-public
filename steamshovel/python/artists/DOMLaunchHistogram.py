from icecube.shovelart import *
from icecube import dataclasses
from icecube.steamshovel.artists.mplart import MPLArtist

class DOMLaunchHistogram( MPLArtist ):

    requiredTypes = [ dataclasses.I3DOMLaunchSeriesMap ]

    def create_plot( self, frame, fig ):
        dkey = list(self.keys())[0]
        domlaunches = frame[dkey]
        launchtimes = [ launchlist[0].time for omkey, launchlist in domlaunches ]
        axes = fig.add_subplot(1,1,1)
        axes.hist( launchtimes, 100 )
        axes.set_xlabel( 'time (ns)', fontsize='smaller' )
        axes.set_ylabel( 'DOMLaunches', fontsize='smaller' )
        axes.set_title( 'Timing of initial DOMLaunches (' + dkey + ')', fontsize='smaller' )
        if( self.setting("Animated") ):
            self.timeline = axes.axvline( 0, color='black' )

    def update_plot( self, time ):
        self.timeline.set_xdata(time)

