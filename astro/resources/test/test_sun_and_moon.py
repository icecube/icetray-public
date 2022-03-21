#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

"""
Test astro's sun and moon converter
"""

import sys,os.path,csv
from datetime import datetime
import unittest
from I3Tray import *
from icecube.icetray import I3Units
from icecube import icetray,dataclasses,dataio,tableio,astro
import test_horizons

TEST_DATA = os.path.expandvars("$I3_BUILD/astro/resources/test/")

class generator(icetray.I3ConditionalModule):
    def __init__(self, context):
        icetray.I3ConditionalModule.__init__(self, context)
        self.AddOutBox("OutBox")
        self.AddParameter("Times","Coordinates to use",[])

                        
    def Configure(self):
        self.times = iter(self.GetParameter("Times"))

    def Process(self):
        try:
            t =  next(self.times)
        except StopIteration:
            self.RequestSuspension()
            return

        header = dataclasses.I3EventHeader()
        header.start_time = dataclasses.make_I3Time(t)
            
        frame = icetray.I3Frame("P")
        frame["I3EventHeader"] = header

        self.PushFrame(frame)

class TableioTest(unittest.TestCase):


    def run_booker(self,horizons_filename,body):

        ephem =  test_horizons.read_horizons(horizons_filename)

        csv_dir_name = "tableout"
        csv_filename = os.path.join(csv_dir_name,"I3EventHeader.csv")
        
        tray  = I3Tray()
        tray.Add(generator,
                 times =ephem['date'])
        tray.Add(tableio.I3TableWriter,
                 tableservice = [tableio.I3CSVTableService(csv_dir_name)],
                 keys = {'I3EventHeader': [astro.converters.I3SunAndMoonConverter(),
                                           dataclasses.converters.I3EventHeaderConverter()],
                         }
        )

        tray.Execute()
        
        del tray



        with open(csv_filename, 'r') as csvfile:
            reader = csv.reader(csvfile)
            header = next(reader)

            keys = ['time_start_mjd [days]', body+'_zenith [radian]',body+'_azimuth [radian]']
            indices = {}
            for k in keys:
                indices[k]=header.index(k)

            #skip info row
            next(reader)
    
            for line_number,row in enumerate(reader):
                
                t = dataclasses.I3Time(float(row[indices['time_start_mjd [days]']]))

                assert(abs(t-dataclasses.make_I3Time(ephem['date'][line_number])) <400)
                                  
                assert(abs(90 - float(row[indices[body+'_zenith [radian]']])/I3Units.degree
                           - ephem['el'][line_number]) < 0.003)

                assert(test_horizons.azimuth_distance(
                    90 - (float(row[indices[body+'_azimuth [radian]']]) + astro.ICECUBE_LONGITUDE)/I3Units.degree,
                    ephem['az'][line_number])< 1.)
                    
                
    def test_moon(self):
        self.run_booker(TEST_DATA + "horizons_moon_1dy.txt",'moon')
        self.run_booker(TEST_DATA + "horizons_moon_1yr.txt",'moon')
        self.run_booker(TEST_DATA + "horizons_moon_30yr.txt",'moon')


    def test_sun(self):
        self.run_booker(TEST_DATA + "horizons_sun_1dy.txt",'sun')
        self.run_booker(TEST_DATA + "horizons_sun_1yr.txt",'sun')
        self.run_booker(TEST_DATA + "horizons_sun_30yr.txt",'sun')


    def tearDown(self):
        import shutil
        shutil.rmtree("tableout")

        
if __name__ == "__main__":
    unittest.main()
