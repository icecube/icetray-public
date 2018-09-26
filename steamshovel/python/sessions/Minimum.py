def _dumpScenario():
    from icecube import shovelart
    from icecube.shovelart import OverlaySizeHint
    from icecube.icetray import OMKey
    from icecube.icetray import logging
    scenario = window.gl.scenario
    scenario.clear()
    try:
        artist = scenario.add( 'Ice', [] )
        scenario.setIsActive( artist, False )
        scenario.changeSetting( artist, '3D dust', False )
        scenario.changeSetting( artist, 'Dust density', 1.5 )
        scenario.changeSetting( artist, 'Dust scatter', 0.2 )
        scenario.changeSetting( artist, 'Show bedrock', True )
        scenario.changeSetting( artist, 'Color ice', shovelart.PyQColor(25,25,255,255) )
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
        scenario.changeSetting( artist, 'DOM radius', 1 )
        scenario.changeSetting( artist, 'high quality DOMs', True )
        scenario.changeSetting( artist, 'string cross', False )
        scenario.changeSetting( artist, 'string color', shovelart.PyQColor(255,255,255,76) )
        scenario.changeSetting( artist, 'string width', 1 )
        scenario.changeSetting( artist, 'hide', 0 )
        scenario.changeSetting( artist, 'DOM labels', False )
        scenario.setIsActive( artist, True )
    except RuntimeError as e:
        logging.log_error( "Could not load a saved artist: " + str(e) )
    window.gl.setCameraPivot(0.0, 0.0, 0.0)
    window.gl.setCameraLoc(1000.0, 1000.0, 1000.0)
    window.gl.setCameraOrientation(-0.707106769085, 0.707106769085, 0.0)
    window.gl.perspectiveView = True
    window.gl.backgroundColor = shovelart.PyQColor(38,38,38,255)
    window.timeline.rangeFinder = "Default"
    window.frame_filter.code = ""
_dumpScenario()
del _dumpScenario
