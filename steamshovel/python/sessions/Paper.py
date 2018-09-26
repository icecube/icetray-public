def _dumpScenario():
    from icecube import shovelart
    from icecube.shovelart import OverlaySizeHint
    from icecube.icetray import OMKey
    from icecube.icetray import logging
    scenario = window.gl.scenario
    scenario.clear()
    try:
        artist = scenario.add( 'Bubbles', ['I3Geometry', 'InIcePulses', ] )
        scenario.changeSetting( artist, 'scale', 10 )
        scenario.changeSetting( artist, 'static', shovelart.PyQColor(255,0,255,255) )
        scenario.changeSetting( artist, 'colormap', shovelart.I3TimeColorMap() )
        scenario.changeSetting( artist, 'power', 0.157 )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    try:
        artist = scenario.add( 'Detector', ['I3Geometry', ] )
        scenario.changeSetting( artist, 'DOM color', shovelart.PyQColor(115,115,115,255) )
        scenario.changeSetting( artist, 'DOM radius', 1 )
        scenario.changeSetting( artist, 'string cross', True )
        scenario.changeSetting( artist, 'string color', shovelart.PyQColor(115,115,115,255) )
        scenario.changeSetting( artist, 'string width', 2 )
        scenario.changeSetting( artist, 'hide', 2 )
        scenario.changeSetting( artist, 'DOM labels', False )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    try:
        artist = scenario.add( 'Ice', [] )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    window.gl.setCameraPivot(7.17023658752, 36.1024436951, -63.2644119263)
    window.gl.setCameraLoc(1100.0, 1100.0, 900.0)
    window.gl.setCameraOrientation(-0.697558283806, 0.716528058052, 0.0)
    window.gl.perspectiveView = True
    window.gl.backgroundColor = shovelart.PyQColor(255,255,255,255)
    window.timeline.rangeFinder = "TimeWindow"
    window.frame_filter.code = ""
_dumpScenario()
del _dumpScenario

