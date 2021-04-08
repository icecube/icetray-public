#!/usr/bin/env python
"""
----- IceTop standalone -----

how to run:
activate the IceTop environment
python3 icetopviewer.py GCDfile.i3(.gz) dataFile.i3(.gz)
    make sure to have first the GCD file first and then the file(s) with Q and P frames.

In the MainLoop the files fed are read and the canvas in surface_canvas.py is called where all the plots are made.

__authors__ = Alan Coleman
              Federico Bontempo
              Alex Olivas

Contact us on slack

TODO list: 
Implement waveforms, particularly for radio (might be hard to smoothly allow selecting)
Issues with indexing python dicts using Scint/Ant/OM keys, may require updating pybindings
"""

import argparse
from icecube.icetray.i3logging import log_fatal
from icecube import icetray, dataio

parser = argparse.ArgumentParser(description='Render IceTop in matplotlib independent of steamshovel.')
parser.add_argument('infile', nargs="+", help='Input I3File(s)')
args = parser.parse_args()

from util import surface_canvas

#Load the detector types
from util.Scintillator import Scintillator
from util.IceTop import IceTop
from util.Antenna import Antenna

"""
This are the main 3 detectors on IceTop. 
In each class all the information are stored and the plots are updated.
e.g. IceTop are the all cherenkov tanks and in the class are stored infos (geometry or the recorded pulses in the frame)
and plots like ldf are updated.
"""
detectors = [Scintillator(), IceTop(), Antenna()]
particleKeys = ["Laputop", 'LaputopSmall', 'ScintReco', 'MCPrimary', 'CoREASPrimary']


def MainLoop():
    canvas = surface_canvas.SurfaceCanvas(detectors, particleKeys)
    framesToView = [icetray.I3Frame.Geometry, icetray.I3Frame.DAQ, icetray.I3Frame.Physics]
    canvas.fig.show()
    gFrameSeen = False
    for file in args.infile:
        for frame in dataio.I3File(file):
            if frame.Stop == icetray.I3Frame.Geometry:
                gFrameSeen = True
                canvas.update_geometry_frame(frame)
            elif frame.Stop in framesToView:
                if not gFrameSeen:
                    log_fatal("While reading {}, hit a {} frame before finding a Geometry frame. \n"
                              "Please feed a GCD file first.".format(file, frame.Stop))
                canvas.update_DAQ_or_P_frame(frame)
            else:
                continue

            canvas.fig.canvas.draw()

            while(True):
                user_response = input('Enter:\n '
                                      'q to quit,\n '
                                      'return to continue,\n '
                                      'r to refresh,\n '
                                      'or o for options: ')

                if user_response.lower() == 'q':
                    exit()
                elif user_response.lower() == 'o':
                    ParseOptions(frame, canvas.particleKeys, canvas.detectors, framesToView)
                elif user_response.lower() == 'r':
                    canvas.update_DAQ_or_P_frame(frame)
                    canvas.fig.canvas.draw()
                else:
                    break


def ParseOptions(frame, particleKeys, detectors, framesToView):
    print("Object to change:")
    print("0: particles")
    print("1: frames")

    nNonDetectorOptions = 2
    for idet, det in enumerate(detectors):
        print(idet+nNonDetectorOptions, det.GetKeyName())

    user_response = input("Enter number: ")

    print("")

    choice = int(user_response)

    if choice == 0:
        print("Available particles:")
        for line in str(frame).split('\n'):
            if "I3Particle" in line:
                print("\t", line.split()[0])
        print("Current particles are", particleKeys)
        user_response = input("Enter desired keys. Note first particle will be used to draw LDFs: ")
        partKeys = user_response.split()
        particleKeys.clear()
        for key in partKeys:
            if key != "":
                particleKeys.append(key)
        print("Particle keys set to", particleKeys)
    elif choice == 1:
        print("Currently viewing frames:", framesToView)
        user_response = input("Enter 1 for QFrames, 2 for PFrames, 3 for both: ")
        framesToView = [[icetray.I3Frame.DAQ], [icetray.I3Frame.Physics], [icetray.I3Frame.Geometry, icetray.I3Frame.DAQ, icetray.I3Frame.Physics]]
    elif nNonDetectorOptions <= choice < len(detectors)+nNonDetectorOptions:
        detectors[choice-nNonDetectorOptions].GetDrawOptions(frame)
    else:
        return

MainLoop()
print("Last frame of last file completed!")