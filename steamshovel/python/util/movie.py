# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
import os, os.path
import tempfile
import subprocess
from icecube.icetray import i3logging

def ffmpeg_flags(fps):
    return ['-framerate', f'{fps}',
            '-r', f'{fps}',
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
            '-r', f'{fps}',
            '-format', 'mp4']

def mencoder_flags(fps):
    return ['-mf', f'fps={fps}',
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
            cmd = ['mencoder', f"mf://{tmpfile1.name}"] + mencoder_flags(25) + [tmpfile2.name]
        i3logging.log_debug( f"testing encoder: {' '.join(cmd)}" )
        try:
            proc = subprocess.Popen( cmd,
                                     stderr=subprocess.STDOUT,
                                     stdout=subprocess.PIPE )
            output, _ = proc.communicate()
            retcode = proc.wait()
            if retcode == 0: # encoder is available and accepts all arguments
                rval.append( encoder )
            else:
                i3logging.log_info( f'{encoder} does not pass the test\n{" ".join(cmd)}\nOutput:\n{output.decode()}')
        except OSError as e:
            i3logging.log_debug( f"{encoder} is not installed or not in PATH" )
    return rval


def tempImageDir():
    return tempfile.mkdtemp()


class AsyncMovie:
    output = ""

    def __init__( self, encoder, inpdir, outfile, fps ):
        if encoder == 'ffmpeg':
            cmd = [ 'ffmpeg', '-i', f'{inpdir}/frame%08d.png' ]
            cmd += ffmpeg_flags(fps)
        elif encoder == 'avconv':
            cmd = [ 'avconv', '-i', f'{inpdir}/frame%08d.png' ]
            cmd += avconv_flags(fps)
        elif encoder == 'mencoder':
            cmd = [ 'mencoder', f'mf://{inpdir}/*.png' ]
            cmd += mencoder_flags(fps)
        else:
            i3logging.log_fatal( f"bad encoder argument: {encoder}"  )
        cmd += [ outfile ]
        i3logging.log_debug( f"Encoding cmdline: {' '.join(cmd)}" )
        self.proc = subprocess.Popen( cmd )

    def stop( self ):
        return self.proc.terminate()

    def isDone( self ):
        return (self.proc.poll() is not None)
