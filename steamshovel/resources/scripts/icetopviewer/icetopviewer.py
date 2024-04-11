#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
----- IceTop standalone -----
how to run:
activate the IceTop environment
python3 icetopviewer.py GCDfile.i3(.gz) dataFile.i3(.gz)
    make sure to have the GCD file first and then the file(s) with Q and P frames.
In the MainLoop the files fed are read and the canvas in surface_canvas.py is called where all the plots are made.
__authors__ = Alan Coleman
              Federico Bontempo
              Alex Olivas
Contact us on slack
TODO list:
M. Plum has sent me this suggestion:
1. If Laputop is show don't show the energy from the particle!!!
If that one is not nan(which can happen) this energy is in 99% of the cases completly wrong.
Show the laputopParams instead, e.g. s125, beta,.... like the old steamshovel was doing it
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
e.g. IceTop are the all cherenkov tanks and in the class are stored info (geometry or the recorded pulses in the frame)
and plots like ldf are updated.
"""
detectors = [Scintillator(), IceTop(), Antenna()]
particleKeys = ["Laputop", 'LaputopSmall', 'ScintReco', 'MCPrimary', 'CoREASPrimary']
paramsKeys = ["LaputopParams", "LaputopSmallParams"]
framesToView = [icetray.I3Frame.DAQ, icetray.I3Frame.Physics]

def MainLoop():
    canvas = surface_canvas.SurfaceCanvas(detectors, particleKeys, paramsKeys)
    canvas.fig.show()
    cid = canvas.fig.canvas.mpl_connect('button_press_event', canvas.ArrayOnClick)
    gFrameSeen = False
    for file in args.infile:
        for frame in dataio.I3File(file):
            if frame.Stop == icetray.I3Frame.Geometry:
                if "I3GeometryDiff" in frame.keys(): continue
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
                                      'or o for options:\n')

                if user_response.lower() == 'q':
                    exit()
                elif user_response.lower() == 'o':
                    ParseOptions(frame, canvas.particleKeys, canvas.paramsKeys, canvas.detectors, framesToView)
                elif user_response.lower() == 'r':
                    if frame.Stop == icetray.I3Frame.Geometry:
                        canvas.update_geometry_frame(frame)
                    elif frame.Stop in framesToView:
                        canvas.update_DAQ_or_P_frame(frame)
                    canvas.fig.canvas.draw()
                else:
                    break


def ParseOptions(frame, particleKeys, paramsKeys, detectors, framesOfChoise):
    print("Option to change:")
    print("0: keys in frame")
    print("1: particles and parameters")
    print("2: frames")

    nNonDetectorOptions = 3
    for idet, det in enumerate(detectors):
        print("{}: {}".format(idet + nNonDetectorOptions, det.GetKeyName()))

    user_response = input("Enter number: ")
    print("")

    choice = int(user_response)

    if choice == 0:
        print(frame.keys())
    elif choice == 1:
        print("0: particles" +
              "\n1: parameters")
        user_response = input("Enter number: ")
        if int(user_response) == 0:
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
        elif int(user_response) == 1:
            print("Current parameters keys are", paramsKeys)
            user_response = input("Enter desired parameters keys: ")
            parKeys = user_response.split()
            paramsKeys.clear()
            for key in parKeys:
                if key != "":
                    print(key)
                    paramsKeys.append(key)
            print("Parameters keys set to", paramsKeys)
        else:
            return
    elif choice == 2:
        print("Currently viewing frames:", framesOfChoise)
        user_response = input("Chose:\n1: (Q) Frames\n2: (P) Frames\n3: (Q) and (P) Frames\nEnter number: ")
        framesOfChoise = [[icetray.I3Frame.DAQ],
                          [icetray.I3Frame.Physics],
                          [icetray.I3Frame.DAQ, icetray.I3Frame.Physics]]
        global framesToView
        framesToView = framesOfChoise[int(user_response)-1]
    elif nNonDetectorOptions <= choice < len(detectors)+nNonDetectorOptions:
        detectors[choice-nNonDetectorOptions].GetDrawOptions(frame)
    else:
        return


MainLoop()
print("Last frame of last file completed!")
