#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

"""
Test astro's tablio converter
"""

import sys,os.path,csv
import unittest
from I3Tray import *
from icecube.icetray import I3Units
from icecube import icetray,dataclasses,dataio,tableio,astro

TEST_DATA = os.path.expandvars("$I3_BUILD/astro/resources/test/")

class generator(icetray.I3ConditionalModule):
    def __init__(self, context):
        icetray.I3ConditionalModule.__init__(self, context)
        self.AddOutBox("OutBox")
        self.AddParameter("Coords","Coordinates to use",[])
        self.AddParameter("EventHeader","Include An Event Header",True)

                        
    def Configure(self):
        self.lines = iter(self.GetParameter("Coords"))
        self.evthdr = self.GetParameter("EventHeader")

    def Process(self):
        try:
            c =  next(self.lines)
        except StopIteration:
            self.RequestSuspension()
            return
            
        eq = astro.I3Equatorial(c[0]*I3Units.degree,
                                c[1]*I3Units.degree)
        time = dataclasses.I3Time(2015,0)
        direction = astro.I3GetDirectionFromEquatorial(eq,time)

        eqq = astro.I3GetEquatorialFromDirection(direction,time)
        
        particle = dataclasses.I3Particle()
        particle.dir = direction

        header = dataclasses.I3EventHeader()
        header.start_time = time
            
        frame = icetray.I3Frame("P")
        if self.evthdr:
            frame["I3EventHeader"] = header
        frame["Particle"] = particle

        self.PushFrame(frame)

class TableioTest(unittest.TestCase):


    def test_tableio(self):
        csv_dir_name = "tableout"
        csv_filename = os.path.join(csv_dir_name,"Particle.csv")
        if os.path.exists(csv_filename):
            os.unlink(csv_filename)

        coords = []
        with open(TEST_DATA + "simbad_tevcat_galactic.txt") as f:
            for line in f.readlines()[7:-2]:
                line = line.split('|')
                coords.append([ float(i) for l in (line[3],line[6],line[5]) for i in l.split()])


        tray  = I3Tray()
        tray.Add(generator,
                 Coords = coords)
        tray.Add(tableio.I3TableWriter,
                 tableservice = [tableio.I3CSVTableService(csv_dir_name)],
                 keys = {'Particle': [ dataclasses.converters.I3ParticleConverter(),
                                       astro.converters.I3AstroConverter(),
                                       astro.converters.I3GalacticConverter(),
                                       astro.converters.I3SuperGalacticConverter(),
                 ]}
        )

        tray.Execute()
        
        del tray

        with open(csv_filename, 'r') as csvfile:
            reader = csv.reader(csvfile)
            header = next(reader)

            keys = ['ra','dec','gal_l','gal_b','sg_l','sg_b']
            indices = {}
            for k in keys:
                indices[k] = header.index(k+' [radian]') 
            
            #skip info row
            next(reader)
    
            for line_number,row in enumerate(reader):
                row_values = [float(row[indices[k]])/I3Units.degree for k in keys]
                for i in range(len(row_values)):
                    self.assertAlmostEqual(row_values[i],coords[line_number][i],2)
    def tearDown(self):
        import shutil
        shutil.rmtree("tableout")

if __name__=="__main__":
    unittest.main()
