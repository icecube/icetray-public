#!/usr/bin/env python3

"""
Convert I3 files to hdf5 format
"""

from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
parser = ArgumentParser(description=__doc__, formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument('infile', nargs='+', help='paths to I3 files to read')
parser.add_argument('outfile', help='path to HDF5 file to write')
parser.add_argument('-n', '--num-frames', type=int, default=None,
    help="Number of frames to consume. If unspecified, read all input files in their entirety.")
parser.add_argument('-i', '--imports', nargs='+', default=[],
    help="Import these IceTray projects, e.g. simclasses")
group = parser.add_mutually_exclusive_group(required=True)
group.add_argument('-a', '--all', default=False, action="store_true",
    help="Write everything in the frame")
group.add_argument('-k', '--keys', nargs='+', default=[],
    help="Write frame objects with these names (e.g. MPEFit) to the output file")
group = parser.add_mutually_exclusive_group(required=True)
group.add_argument('--sim', default=False, action="store_true",
    help="Split Q frames into P frames. Use this option if your input file is "
    "un-triggered simulation.")
group.add_argument('--streams', nargs='+', default=['InIceSplit'],
    help="Only consume these trigger splits, i.e. P frames whose "
    " I3EventHeader.sub_event_stream one of STREAMS")

args = parser.parse_args()

from icecube import icetray, dataclasses, dataio
import importlib
from I3Tray import I3Tray

tray = I3Tray()

tray.context['I3FileStager'] = dataio.get_stagers()
tray.Add("I3Reader", filenamelist=args.infile)

for project in args.imports:
    importlib.import_module('icecube.{}'.format(project))

kwargs = {
    'Output': args.outfile,
    'BookEverything': args.all,
    'Keys': args.keys,
}
if not args.sim:
    from icecube.hdfwriter import I3HDFWriter as HDFWriter
    kwargs['SubEventStreams'] = args.streams
else:
    from icecube.hdfwriter import I3SimHDFWriter as HDFWriter

tray.Add(HDFWriter, **kwargs)

if args.num_frames:
    tray.Execute(args.num_frames)
else:
    tray.Execute()

