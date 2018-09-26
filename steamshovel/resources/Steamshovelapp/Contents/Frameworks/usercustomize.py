import sys
import os.path

# Remove paths from sys.path that don't fall within our
# app binary, except if they are system paths;
# this is intended to provide isolation from user python
# packages.

newpath = []
base_path = os.path.dirname(__file__)
for p in sys.path:
    if( p.startswith(base_path) or
        p.startswith('/System/Library/') or
        p.startswith('/usr/lib') ):
    	newpath.append(p)
sys.path = newpath
