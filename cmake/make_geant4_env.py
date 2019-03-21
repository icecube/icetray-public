#!/usr/bin/python
'''
Produces POSIX commands to setup the environment variables for Geant4.

Required command line arguments:
1: Location of geant4.sh script.
2: Version of Geant4.
'''

import os
import sys
import re
import subprocess as subp
from codecs import encode,decode

geant4_sh, geant4_version = sys.argv[1:]

# vars and standard directory names
geant4_vars = {
    "G4ABLADATA"       : "G4ABLA",
    "G4LEDATA"         : "G4EMLOW",
    "G4LEVELGAMMADATA" : "PhotonEvaporation",
    "G4NEUTRONHPDATA"  : "G4NDL",
    "G4NEUTRONXSDATA"  : "G4NEUTRONXS",
    "G4PIIDATA"        : "G4PII",
    "G4RADIOACTIVEDATA": "RadioactiveDecay",
    "G4REALSURFACEDATA": "RealSurface",
    "G4ENSDFSTATEDATA" : "G4ENSDFSTATE2.2",
    "G4SAIDXSDATA"     : "G4SAIDDATA1.1"
}

geant4_env = {}

# try to get vars from geant4.sh script
if os.path.isfile(geant4_sh):
    p = subp.Popen("/bin/bash",
                   stdin=subp.PIPE,
                   stdout=subp.PIPE,
                   cwd=os.path.dirname(geant4_sh),
                   env={})
    penv = decode(p.communicate(encode("source geant4.sh && env"))[0].strip())
    for line in penv.split("\n"):
        sep = line.index("=")
        var = line[:sep]
        value = line[sep+1:]
        if var in geant4_vars:
            geant4_env[var] = value

formatted_pairs = []
for var in geant4_vars:

    value = None
    if var in os.environ:
        # warn user that existing environment variables override this script,
        # but don't complain if we are just running inside an env-shell.sh
        value = os.environ[var]
        if not "I3_SHELL" in os.environ:
            sys.stderr.write(("Warning: Geant4 environment variable already set {0}={1}, "
                              "this overrides automatic detection\n")
                              .format(var, value))
    elif var in geant4_env:
        value = geant4_env[var]

    if value is None:
        sys.stderr.write(("Warning: Geant4 environment variable {0} could not be set, "
                          "g4-based modules may crash\n").format(var))
    else:
        formatted_pairs.append("{0}={1}".format(var, value))           

# extra formatting for env-shell.sh        
sys.stdout.write(" \\\n\t".join(formatted_pairs))
