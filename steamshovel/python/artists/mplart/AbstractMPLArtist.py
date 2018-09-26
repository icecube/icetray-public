from icecube.shovelart import PyArtist, PyQColor, RangeSetting, TimeWindowColor
from matplotlib.figure import SubplotParams
from icecube.icetray import logging
import warnings

class MPLArtist( PyArtist ):
    '''A PyArtist representing a matplotlib figure, intended for any backend'''

    class _manager_cleanup:
        ''' 
        Figure manager cleanup action, to be passed to PyArtist.addCleanupAction
        
        This action will trigger when this MPLArtist is deleted.  It should not be
        confused with the get_canvas_cleanup() method which returns an action to be
        called whenever the previous outputs the artists are invalided, i.e. when
        the currently selected frame changes.
        '''
        def __init__( self, mgr ):
            self.mgr = mgr

        def __call__(self):
            if( self.mgr ):
                # matplotlib version 1.1.0 stashes self into lambda,
                # causing "RuntimeError: wrapped C/C++ object of %S has been deleted"
                # when using QtMPLArtist and opening and closing windows.
                # It appears this will go away with version 1.2
                # the below line was an unsuccessful attempt to flush the error.
                #self.mgr.canvas.close_event()
                self.mgr.destroy()

    _nextfig = 1
    @staticmethod
    def unique_figure():
        '''Get a program-unique integer ID to use for this artist's MPL figure'''
        f = MPLArtist._nextfig
        MPLArtist._nextfig += 1
        return f


    def __init__(self):
        PyArtist.__init__(self)
        # Do not call new_figure_manager at this point, since the artist
        # may be deleted without ever generating output
        self.manager = None

        # An MPLArtist with an update_plot method can be animated; expose this as a setting
        self.is_animated = hasattr( self, 'update_plot' )
        if( self.is_animated ):
            self.addSetting( "Animated", False )

        # An MPLArtist with a time_to_color method can use the steamshovel colormap
        # and the plot is going to be re-drawn whenever the color range in the time line changes
        if hasattr( self, 'time_to_color' ):
            self.addSetting( "TWUpdate", True )

        self.addSetting( "Plot density", RangeSetting(.25,2.0,100,1.0) )


    def autosize(self, fig, output):
        '''Adjust the size of the figure and axes as best as possible'''

        sizehints = self.overlaySizeHints( output )
        if len(sizehints):
            hint = sizehints[0]
            dpi = self.working_dpi * self.overlayScalingFactor( output )
            fig.set_size_inches( hint.width / dpi, hint.height / dpi )
            fig.set_dpi(dpi)


    def create( self, frame, output ):
        '''Primary PyArtist.create() method'''
        if self.manager == None:
            self.manager = self.get_figure_manager( MPLArtist.unique_figure() )
            self.base_dpi = self.manager.canvas.figure.get_dpi()

        self.working_dpi = self.base_dpi * self.setting('Plot density')

        self._overlayLines = []
        fig = self.manager.canvas.figure

        # Call the subclass's primary create method
        self.autosize( fig, output )
        self.create_plot( frame, fig )

        # Set up cleanup / update actions, if any are needed
        cleanup = self.get_canvas_cleanup(fig)
        update = self.get_canvas_update(fig)
        output.addPhantom( cleanup, update )

        self.manager.canvas.draw()

        # silence warnings from matplotlib
        warnings.filterwarnings("ignore")
        try:
            fig.tight_layout(None,0.3)
        except ValueError as e:
            logging.log_debug("fig.tight_layout raised ValueError")
        warnings.resetwarnings()

        self.create_output( output )


    def addOverlayLine(self, position, color ):
        '''
        Request a line be drawn from out plot to the given shovelart.vec3d with the given color

        This will only have an effect for in-scene graphics
        '''
        self._overlayLines.append( (position,color) )

