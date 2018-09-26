from __future__ import print_function
import sys
import os, os.path
import tempfile
import subprocess
from icecube.icetray import i3logging

def ffmpeg_flags(fps):
    return ['-framerate', '{0}'.format(fps),
            '-r', '{0}'.format(fps),
            '-y', # erase old files
            # on ffmpeg v1.1.3, theses are required to create quicktime-compatible mp4
            '-vprofile', 'main',
            '-pix_fmt', 'yuv420p',
            # Be as close to lossless as possible for the 'main' x264 profile
            '-crf', '1',
            # noninteractive
            '-nostdin',
            '-tune', 'animation',
            '-preset', 'veryslow']

def avconv_flags(fps):
    return ['-y', # erase old files
            '-pix_fmt', 'yuv420p',
            '-r', '{0}'.format(fps),
            '-format', 'mp4']

def mencoder_flags(fps):
    return ['-mf', 'fps={0}'.format(fps),
            '-ovc', 'lavc',
            '-lavcopts', 'vcodec=mpeg4',
            '-o']

def getMovieEncoders():
    '''
    Try all supported movie encoders (ffmpeg, avconv, mencoder)
    and return a list of the working ones.
    '''

    rval = []
    for encoder in ('ffmpeg', 'avconv', 'mencoder'):
        tmpfile1 = tempfile.NamedTemporaryFile(suffix='.png')
        tmpfile2 = tempfile.NamedTemporaryFile(suffix='.mp4')
        if encoder == 'ffmpeg':
            cmd = ['ffmpeg'] + ffmpeg_flags(25) + ['-version']
        if encoder == 'avconv':
            cmd = ['avconv'] + avconv_flags(25) + ['-version']
        if encoder == 'mencoder':
            cmd = ['mencoder', 'mf://'  + tmpfile1.name] + mencoder_flags(25) + [tmpfile2.name]
        i3logging.log_debug( 'testing encoder: ' + ' '.join(cmd) )
        try:
            proc = subprocess.Popen( cmd,
                                     stderr=subprocess.STDOUT,
                                     stdout=subprocess.PIPE )
            output, _ = proc.communicate()
            retcode = proc.wait()
            if retcode == 0: # encoder is available and accepts all arguments
                rval.append( encoder )
            else:
                i3logging.log_info( '{0} does not pass the test\n{1}\nOutput:\n{2}'
                                    .format(encoder, ' '.join(cmd), output) )
        except OSError as e:
            i3logging.log_debug( encoder + ' is not installed or not in PATH' )
    return rval


def tempImageDir():
    return tempfile.mkdtemp()


class AsyncMovie:
    output = ""

    def __init__( self, encoder, inpdir, outfile, fps ):
        if encoder == 'ffmpeg':
            cmd = [ 'ffmpeg', '-i', '{0}/frame%08d.png'.format( inpdir ) ]
            cmd += ffmpeg_flags(fps)
        elif encoder == 'avconv':
            cmd = [ 'avconv', '-i', '{0}/frame%08d.png'.format( inpdir ) ]
            cmd += avconv_flags(fps)
        elif encoder == 'mencoder':           
            cmd = [ 'mencoder', 'mf://{0}/*.png'.format( inpdir ) ]
            cmd += mencoder_flags(fps)
        else:
            i3logging.log_fatal( "bad encoder argument: {0}".format( encoder )  )
        cmd += [ outfile ]
        i3logging.log_debug( 'Encoding cmdline: ' + ' '.join(cmd) )
        self.proc = subprocess.Popen( cmd )

    def stop( self ):
        return self.proc.terminate()

    def isDone( self ):
        return (self.proc.poll() is not None)
