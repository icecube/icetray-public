#!/usr/bin/env python2.7
from __future__ import print_function
import subprocess as subp
import os
import sys

def cmd( x, *args, **kwargs ):
    x = x.format( *args )
    print( x )
    return subp.Popen( x, shell=True,
                       stderr=subp.STDOUT,
                       stdout=subp.PIPE, **kwargs ).communicate()[0].strip()

for x in sys.argv[1:]:
	cmd('chmod u+w {0}', x)
	cout = cmd('otool -L {0}', x)
	old_paths = set()
	for line in cout.split('\n'):
		line = line.strip()
		old_paths.add(line[:line.find(' ')])
	for old_path in old_paths:
		if( old_path.startswith('/usr/lib') or
			old_path.startswith('/System/Library') or 
			old_path.startswith('@executable_path') ):
			continue
		base = os.path.basename(old_path)
		new_path = '@executable_path/../Frameworks/' + base
		cmd('install_name_tool -change {0} {1} {2}', old_path, new_path, x)