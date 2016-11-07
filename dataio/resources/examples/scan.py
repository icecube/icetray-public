#!/usr/bin/env python
"""Scan an i3 file, outputting in various ways."""

import sys

try:
    from icecube import icetray,dataio
except ImportError:
    print('You must be inside an IceCube metaproject environment.')
    sys.exit(1)

def files(args):
    """A frame generator that can continue over multiple files"""
    for a in args:
        for f in dataio.I3File(a):
            yield f

def main():
    from optparse import OptionParser,OptionGroup
    
    usage = 'usage: %prog [options] input_file [input_file2 ...]\n\n'
    usage += 'Scan an i3 file, outputting in various ways.'
    parser = OptionParser(usage=usage)
    group = OptionGroup(parser,'Output Options','Select one of the following')
    group.add_option('-w','--whole-frames',default=False,action='store_true',
                     dest='frames',help='Print whole frames')
    group.add_option('-s','--streams-only',default=False,action='store_true',
                     dest='streams',help='Print stream ids')
    group.add_option('-c','--count',default=False,action='store_true',
                     dest='count',help='Print total number of frames')
    parser.add_option_group(group)
    
    (options,args) = parser.parse_args()
    
    if len(args) < 1:
        print('ERROR: no input file')
        print('')
        parser.print_help()
    
    # make sure only one of frames, streams, and count are selected
    elif ((options.frames and (options.streams or options.count)) or
        (options.streams and (options.frames or options.count)) or
        (options.count and (options.frames or options.streams))):
        print('ERROR: only select one of --whole-frames, --streams-only, or --count')
        print('')
        parser.print_help()
    
    else:
        count = 0
        for frame in files(args):
            if options.frames:
                print(frame)
            elif options.streams:
                sys.stdout.write(frame.Stop.id)
            elif options.count:
                count += 1
        if options.count:
            print('Total:',count,'frames')

if __name__ == '__main__':
    main()
