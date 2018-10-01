#!/usr/bin/env python
"""Strip frame types from an i3 file."""

import sys
import os

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
    
    usage = 'usage: %prog [options] input_file [input_file2 ...] output_file\n\n'
    usage += 'Strip frame types from an i3 file.'
    parser = OptionParser(usage=usage)
    parser.add_option('-g','--geometry',default=False,action='store_true',
                      help='Strip Geometry frames')
    parser.add_option('-c','--calibration',default=False,action='store_true',
                      help='Strip Calibration frames')
    parser.add_option('-d','--detector',default=False,action='store_true',
                      help='Strip Detector frames')
    parser.add_option('--no-trayinfo',default=True,action='store_false',
                      dest='trayinfo',help='Do not strip TrayInfo frames')
    
    (options,args) = parser.parse_args()
    
    if len(args) < 2:
        print('ERROR: require an input and output file')
        print('')
        parser.print_help()
    
    elif os.path.exists(args[-1]):
        print('ERROR: output file',args[-1],'already exists!')
        print('')
        parser.print_help()
    
    else:
        count = 0
        outfile = dataio.I3File(args[-1],'w')
        try:
            for frame in files(args[:-1]):
                if options.geometry and frame.Stop == icetray.I3Frame.Geometry:
                    continue
                elif options.calibration and frame.Stop == icetray.I3Frame.Calibration:
                    continue
                elif options.detector and frame.Stop == icetray.I3Frame.Detector:
                    continue
                elif options.trayinfo and frame.Stop == icetray.I3Frame.TrayInfo:
                    continue
                outfile.push(frame)
        finally:
            outfile.close()

if __name__ == '__main__':
    main()
