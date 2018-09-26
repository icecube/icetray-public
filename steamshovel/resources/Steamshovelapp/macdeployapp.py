#!/usr/bin/env python2.7
from __future__ import print_function
import subprocess as subp
import os
import sys
import re
import glob

realpath = os.path.realpath

def cmd( x, *args, **kwargs ):
    x = x.format( *args )
    print( x )
    try:
        r = subp.Popen( x, shell=True,
                       stderr=subp.STDOUT,
                       stdout=subp.PIPE, **kwargs ).communicate()[0].strip()
    except OSError as err:
        print('Calling {0} failed: '.format(err))
    except:
        print("Unexpected error:", sys.exc_info()[0])
    else:
        return r

def dyld(lib):
    if os.path.exists(lib):
        return realpath(lib)
    searchpaths=[
        "/usr/lib",
        "/usr/local/lib",
        "~/lib"
    ]
    user=os.getenv("DYLD_LIBRARY_PATH",None)
    if user != None:
        for path in user.split(':'):
            searchpaths.append(path)
    for path in searchpaths:
        test=path+"/"+lib
        if os.path.exists(test):
            return realpath(test)
    return None

class LibraryCollector(set):

    def collect(self, files):
        files = [x for x in files if self.isLib(x)]
        self._collect(set(files))

    def _collect(self, files):
        # recursively collect all dependencies
        for fn in files:
            fn = realpath(fn)
            self.add(fn)
            linked = self.getLinkedLibs(fn)
            new_libs = linked - self
            if new_libs:
                self._collect(new_libs)

    @staticmethod
    def getLinkedLibs(fn):
        cout = cmd( 'otool -L {0} | cut -f 1 -d " "', fn )
        result = set()
        for x in cout.split('\n'):
            x = x.strip()
            x = dyld(x)
            if x == None:
                continue
            x = realpath(x)
            if( x.lower().startswith('/System/Library'.lower()) or
                x.lower().startswith('/usr/lib'.lower()) or
                x.lower().startswith('/var'.lower()) or
                x.lower().startswith(build_dir.lower()) ):
                continue
            result.add(x)
        return result

    @staticmethod
    def isLib(fn):
        return 'shared library' in subp.check_output( ('file', fn) )

build_dir, svn_rev = sys.argv[1:]
build_dir = realpath( build_dir )
os.chdir( build_dir )
app_dir = build_dir + "/Steamshovel.app"
framework_dir = app_dir + "/Contents/Frameworks"

# Remove existing app dir
if os.path.exists( app_dir ):
    cmd( 'rm -rf {0}', app_dir )

# Create clean template
cmd( 'cp -L -R {0}/steamshovel/resources/Steamshovelapp {1}', build_dir, app_dir )
cmd( 'find {0} -name \.svn -print0 | xargs -0 rm -rf', app_dir )
cmd( 'sed -i -e "s/SVNREV/{0}/" {1}/Contents/Info.plist', svn_rev, app_dir )

# Copy documentation
cmd( 'mkdir -p {0}/Contents/Resources/docs', app_dir )
cmd( 'cp -L -R {0}/docs/* {1}/Contents/Resources/docs',
     build_dir, app_dir )

# Copy steamshovel binaries
cmd( 'cp -L {0}/bin/steamshovel* {1}/Contents/MacOS',
     build_dir, app_dir )

# Copy ffmpeg binary to make movies, is allowed to fail
cmd( 'cp -L `which ffmpeg` {0}/Contents/MacOS', app_dir )

# Copy all linked libs and all python modules seen by icetray

# Run macdeploy qt once...
print( cmd( 'macdeployqt {0} -executable={0}/Contents/MacOS/steamshovel-logviewer -executable={0}/Contents/MacOS/steamshovel', app_dir ) )

# ... then chmod the frameworks it copied, since it's dumb about that...
print( 'Do not be alarmed by permissions errors from install-name-tool above this line' )
cmd( 'chmod -R u+w {0}', framework_dir )

# ... then run it again, to ensure any previously unwritable frameworks get fixed
cmd( 'macdeployqt {0} -executable={0}/Contents/MacOS/steamshovel-logviewer -executable={0}/Contents/MacOS/steamshovel', app_dir )

## Make a list of what we already have from macdeployqt
cout = cmd( 'find {0} -type f', framework_dir )
skip = set()
for x in cout.split('\n'):
    skip.add(os.path.basename(x))

## Copy icecube libraries
cmd( 'mkdir -p {0}/icecube', framework_dir )
cmd( 'cp -L -R {0}/lib/* {1}/icecube', build_dir, framework_dir )

## Get libs that were linked into the binaries and libs that we have so far from otool
libs = LibraryCollector()
libs.collect( glob.glob('{0}/Contents/MacOS/*'.format(app_dir))
              + cmd('find {0} -type f', framework_dir).split('\n') )

## Get icetray environment
env = {}
for line in cmd( './env-shell.sh env', build_dir ).split( '\n' ):
    m = re.match('(.+)=(.*)', line)
    if m is None: continue
    key = m.group(1)
    val = m.group(2)
    env[key] = val

## Execute steamshovel to get dynamically loaded libs and the python modules in sys.path
env['DYLD_PRINT_LIBRARIES_POST_LAUNCH'] = '1'
cout = cmd( '{0}/bin/steamshovel --testmode --batch --noconsole --script '
            '{1}/shovellibs.py',
            build_dir, app_dir, env=env )
## Filter the output, ignore references inside build_dir and system libs
pypaths = set()
for x in cout.split('\n'):
    pypath = x.startswith( 'python:' )
    x = x[ x.find('/'): ]
    if not os.path.exists( x ):
        continue
    x = realpath(x)
    if pypath:
        pypaths.add( x )
    else:
        libs.add( x )
## Copy python modules
cmd( 'mkdir {0}/python', framework_dir )
for x in pypaths:
    if( x.lower().startswith('/System/Library'.lower()) or
        x.lower().startswith('/usr/lib'.lower()) or
        x.lower().startswith('/var'.lower()) or
        x.lower().startswith('/tmp'.lower()) or
        x.lower().startswith('/private'.lower()) or
        x.lower().startswith(build_dir.lower()) ):
        continue
    cmd( 'cp -L -R {0}/* {1}/python', x, framework_dir )
## Copy shared libs, but skip libs that were already copied
for x in sorted(libs):
    if( x.lower().startswith('/System/Library'.lower()) or
        x.lower().startswith('/usr/lib'.lower()) or
        x.lower().startswith('/var'.lower()) or
        x.lower().startswith('/tmp'.lower()) or
        x.lower().startswith('/private'.lower()) or
        x.lower().startswith(build_dir.lower()) ):
        continue
    for p in pypaths:
        if x.startswith(p):
            break
    else:
        if not os.path.basename(x) in skip:
            cmd( 'cp -L {0} {1}', x, framework_dir )

# Clean up .pyc, .pyo files copied in
cmd( 'find {0} -name "*.py[co]" -print0 | xargs -0 rm', app_dir )

# Change lib paths
cmd( '{0}/change_lib_path.py {0}/Contents/MacOS/ffmpeg', app_dir )

# Create version symlinks for libs
cmd( '{0}/make_lib_symlinks.py {1}/*', app_dir, framework_dir )
