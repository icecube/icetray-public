.. _icetray-inspect:

icetray-inspect
---------------
Interrogate library in your compiled workspace to get a list of all available modules and services.  Dumps all available parameters as well.


::

  I3 Library Inspector:
    -h [ --help ]         this message
    -a [ --all ] arg      Examine all available projects/libraries in DIRECTORY
    -x [ --xml ]          Output xml
    -s [ --svn ]          Show svn url/revision information (affects plaintext 
			  mode only)
    -p [ --projects ] arg Projects to inspect

Usage:

 % ./env-shell.sh icetray-inspect dataio

