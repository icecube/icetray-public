from matplotlib.backends.backend_agg import new_figure_manager

from .AbstractMPLArtist import MPLArtist

from icecube.shovelart import PyQColor

class AggMPLArtist( MPLArtist ):
    '''
    A PyArtist representing a matplotlib figure, using the agg backend without any gui integration
    '''

    def get_figure_manager( self, fignum ):
        return new_figure_manager( fignum )

    def get_canvas_cleanup( self, figure ):
        '''
        Return the cleanup action that fires whenver this artist's current output invalidates
        
        This just clears the output
        '''
        def impl():
            figure.clear()
            self.manager.canvas.draw()
        return impl

    def get_canvas_update( self, figure ):
        '''Return the update action that fires whenever the time changes'''
        return None

    def export_image( self ):
        '''Export self.manager.canvas to a tuple (raw_data_string, width, height)'''
        buf, (w, h) = self.manager.canvas.print_to_buffer()
        return buf, w, h

    def active_redraw( self, time ):
        '''redraw action to pass to ActivePixmapOverlay'''
        self.update_plot(time)
        return self.export_image()

    def create_output( self, output ):
        '''Export this artist's output to OpenGL'''

        if self.is_animated and self.setting("Animated"):
            tex = output.addActivePixmapOverlay( lambda x:self.active_redraw(x) )
        else:
            raw_data, w, h = self.export_image()
            tex = output.addPixmapOverlay( w, h, raw_data )

        for pos, color in self._overlayLines:
            line = output.addOverlayLine( tex, pos )
            line.setLineWidth( 2 )
            line.setColor( PyQColor.fromRgbF(*color) )

