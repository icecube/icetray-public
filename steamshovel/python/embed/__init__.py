import sys
import os.path

# Use a virtualenv if we have one
if 'VIRTUAL_ENV' in os.environ:
    pass
    activate_script = os.path.join(
        os.environ['VIRTUAL_ENV'], 'bin', 'activate_this.py')
    if os.path.exists(activate_script):
        if sys.version_info[0] < 3:
            execfile(activate_script, {'__file__': activate_script})
        else:
            exec(compile(open(activate_script, "rb").read(), activate_script, 'exec'), {'__file__': activate_script})
    # IPython will check if VIRTUAL_ENV is set, and complain unless
    # sys.executable (i.e. steamshovel) is inside the virtualenv. We've just
    # handled all the necessary setup, so squash the warning.
    del os.environ['VIRTUAL_ENV']
from icecube import icetray
icetray.set_log_level_for_unit('icetray',  icetray.I3LogLevel.LOG_FATAL)

# If a dir named 'ipython' is inside this package, load it first
# This allows a user to stash a newer version of ipython in this directory,
# to take advantage of the 1.0.dev features required for the GUI python widget
#sys.path.insert(0, os.path.join(__path__[0], 'ipython'))


