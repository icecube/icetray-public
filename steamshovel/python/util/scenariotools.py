from contextlib import contextmanager


class PythonWriter:

    "Convenience class for generating python code."

    def __init__(self, s=None):
        self.dents = 0
        self.code = s.split("\n") if s else []

    @contextmanager
    def block(self, string):
        "Context manager for indentation."
        self(string)
        self.dents += 1
        yield
        self.dents -= 1

    def __call__(self, string, *args):
        "Add a code line."
        self.code.append(' ' * 4 * self.dents + string.format(*args))

    def __str__(self):
        "Return code as string."
        return '\n'.join(self.code)

    def __repr__(self):
        return "PythonWriter(\"\"\"" + str(self) + "\"\"\")"


def dumpScenarioState(code, scenario):
    import re

    for a in scenario.getArtists():
        with code.block('try:'):
            code("artist = scenario.add( '{0}', {1} )",
                 a.description, a.keys)

            # deactivate artist before changing settings, so that
            # unnecessary updates to the scene are avoided
            code("scenario.setIsActive( artist, False )")

            for key, s in a.settings.items():
                if key == "OMKeys": continue # we don't store selections
                srepr = repr(s)
                code("scenario.changeSetting( artist, '{0}', {1} )",
                     key, srepr)

            hints = scenario.overlaySizeHints(a)
            if len(hints):
                code("scenario.setOverlaySizeHints( artist, {0} )",
                     hints)

            if a.isActive:
                code("scenario.setIsActive( artist, True )")

        with code.block('except StandardError as e:'):
            code('logging.log_error( e.__class__.__name__ + " occured '
                 'while loading saved state of {0}: " + str(e) )',
                 a.description)
        with code.block('except:'):
            code('logging.log_error( "Unknown error occured '
                 'while loading saved state of {0}: " + str(e) )',
                 a.description)

def exectableScenarioDump():
    from __main__ import window

    code = PythonWriter()
    with code.block('def _dumpScenario():'):
        # save current state of artists
        from icecube import shovelart
        shovelart_classes = ', '.join([x for x in dir(shovelart) if not x.startswith("_")])
        code('from icecube.shovelart import ' + shovelart_classes)
        code('from icecube.icetray import OMKey')
        # dumpScenarioState also expects to have access to log_error
        code('from icecube.icetray import logging')
        code('scenario = window.gl.scenario')
        code('scenario.clear()')
        dumpScenarioState(code, window.gl.scenario)

        # save camera settings as part of scenario
        for s, v in (("Pivot", window.gl.cameraPivot),
                     ("Loc", window.gl.cameraLoc),
                     ("Orientation", window.gl.cameraOrientation)):
            code("window.gl.setCamera{0}({1}, {2}, {3})",
                 s, v.x, v.y, v.z)
        code("window.gl.cameraLock = {0}",
             window.gl.cameraLock)
        code("window.gl.perspectiveView = {0}",
             window.gl.perspectiveView)
        code("window.gl.backgroundColor = {0}",
             window.gl.backgroundColor)

        # save rangeFinder setting
        code('window.timeline.rangeFinder = "{0}"',
             window.timeline.rangeFinder)

        # save filter code, escape quotes
        code('window.frame_filter.code = "{0}"',
             window.frame_filter.code
                .replace('"',r'\"')
                .replace('\n', r'\n')
        )

        code('window.activeView = {0}', window.activeView)

    code('_dumpScenario()')
    code('del _dumpScenario')

    return str(code)
