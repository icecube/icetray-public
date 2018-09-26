def _dumpScenario():
    from icecube import shovelart
    from icecube.shovelart import OverlaySizeHint
    from icecube.icetray import OMKey
    from icecube.icetray import logging
    scenario = window.gl.scenario
    scenario.clear()
    try:
        artist = scenario.add( 'TextSummary', ['I3EventHeader', ] )
        scenario.setIsActive( artist, False )
        scenario.changeSetting( artist, 'short', True )
        scenario.changeSetting( artist, 'font', shovelart.PyQFont.fromString('Ubuntu,11,-1,5,50,0,0,0,0,0') )
        scenario.changeSetting( artist, 'size', 12 )
        scenario.setOverlaySizeHints( artist, [OverlaySizeHint(10,10,230,60), ] )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    try:
        artist = scenario.add( 'IceTopLDF', ['I3Geometry', 'Laputop', ] )
        scenario.setIsActive( artist, False )
        scenario.changeSetting( artist, 'Markersize', 8 )
        scenario.changeSetting( artist, 'Colormap', shovelart.I3TimeColorMap() )
        scenario.changeSetting( artist, 'No hits', True )
        scenario.changeSetting( artist, 'Show', 5 )
        scenario.changeSetting( artist, 'TWUpdate', True )
        scenario.changeSetting( artist, 'Top', 0.95 )
        scenario.changeSetting( artist, 'Fit Info', False )
        scenario.changeSetting( artist, 'Logx', False )
        scenario.changeSetting( artist, 'Display', 0 )
        scenario.changeSetting( artist, 'Right', 0.95 )
        scenario.changeSetting( artist, 'SLC pulses', '' )
        scenario.changeSetting( artist, 'Time Limit', '' )
        scenario.changeSetting( artist, 'HLC pulses', 'CleanedHLCTankPulses' )
        scenario.changeSetting( artist, 'Bottom', 0.1 )
        scenario.changeSetting( artist, 'Logy', True )
        scenario.changeSetting( artist, 'HSpace', 0.2 )
        scenario.changeSetting( artist, 'Plot density', 1.0 )
        scenario.changeSetting( artist, 'Left', 0.2 )
        scenario.setOverlaySizeHints( artist, [OverlaySizeHint(770,10,485,780), ] )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    try:
        artist = scenario.add( 'Ice', [] )
        scenario.setIsActive( artist, False )
        scenario.changeSetting( artist, '3D dust', False )
        scenario.changeSetting( artist, 'Dust density', 1.5 )
        scenario.changeSetting( artist, 'Dust scatter', 0.2 )
        scenario.changeSetting( artist, 'Show bedrock', False )
        scenario.changeSetting( artist, 'Color ice', shovelart.PyQColor(255,255,255,55) )
        scenario.changeSetting( artist, 'Color bedrock', shovelart.PyQColor(128,102,102,255) )
        scenario.changeSetting( artist, 'Plane width', '2200 m' )
        scenario.changeSetting( artist, 'Show ice', True )
        scenario.changeSetting( artist, 'Line width', 1 )
        scenario.changeSetting( artist, 'Show dust', False )
        scenario.changeSetting( artist, 'Color dust', shovelart.PyQColor(100,100,100,50) )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    try:
        artist = scenario.add( 'Detector', ['I3Geometry', ] )
        scenario.setIsActive( artist, False )
        scenario.changeSetting( artist, 'DOM color', shovelart.PyQColor(255,255,255,255) )
        scenario.changeSetting( artist, 'DOM radius', 5 )
        scenario.changeSetting( artist, 'high quality DOMs', True )
        scenario.changeSetting( artist, 'string cross', False )
        scenario.changeSetting( artist, 'string color', shovelart.PyQColor(255,255,255,76) )
        scenario.changeSetting( artist, 'string width', 1 )
        scenario.changeSetting( artist, 'hide', 1 )
        scenario.changeSetting( artist, 'DOM labels', False )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    try:
        artist = scenario.add( 'ChargeBubbles', ['I3Geometry', 'CleanedHLCTankPulses', ] )
        scenario.setIsActive( artist, False )
        scenario.changeSetting( artist, 'delay', 0 )
        scenario.changeSetting( artist, 'scale', 1.63 )
        scenario.changeSetting( artist, 'colormap', shovelart.I3TimeColorMap() )
        scenario.changeSetting( artist, 'power', 0.27901 )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    try:
        artist = scenario.add( 'CoordinateSystem', [] )
        scenario.setIsActive( artist, False )
        scenario.changeSetting( artist, 'opacity', 1 )
        scenario.changeSetting( artist, 'x length', '1 km' )
        scenario.changeSetting( artist, 'z length', '1 km' )
        scenario.changeSetting( artist, 'line width', 1 )
        scenario.changeSetting( artist, 'head angle', 22.1 )
        scenario.changeSetting( artist, 'position', '-600m, -550m, 1950m' )
        scenario.changeSetting( artist, 'y length', '1 km' )
        scenario.changeSetting( artist, 'head length', 30 )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    try:
        artist = scenario.add( 'Particles', ['Laputop', ] )
        scenario.setIsActive( artist, False )
        scenario.changeSetting( artist, 'incoming/outgoing', True )
        scenario.changeSetting( artist, 'colormap', shovelart.I3TimeColorMap() )
        scenario.changeSetting( artist, 'power', 0.3 )
        scenario.changeSetting( artist, 'color', shovelart.PyQColor(243,51,51,153) )
        scenario.changeSetting( artist, 'vertex size', 20 )
        scenario.changeSetting( artist, 'labels', True )
        scenario.changeSetting( artist, 'Cherenkov cone size', 0 )
        scenario.changeSetting( artist, 'light fronts', False )
        scenario.changeSetting( artist, 'arrow head size', 120 )
        scenario.changeSetting( artist, 'linewidth', 3 )
        scenario.changeSetting( artist, 'min. energy', '' )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    window.gl.setCameraPivot(415.0, -4.0, 2000.0)
    window.gl.setCameraLoc(415.0, -4.0, 4400.0)
    window.gl.setCameraOrientation(1.0, 0.0, 0.0)
    window.gl.cameraLock = True
    window.gl.perspectiveView = False
    window.gl.backgroundColor = shovelart.PyQColor(38,38,38,255)
    window.timeline.rangeFinder = "CleanedHLCTankPulses"
    window.frame_filter.code = "p = frame[\"Laputop\"]\nif p.fit_status != p.FitStatus.OK:\n    return False\ns = frame[\"CleanedHLCTankPulses\"]\nn = 0\nfor x in s.bits:\n    if x: n += 1\nreturn n > 50"
_dumpScenario()
del _dumpScenario
