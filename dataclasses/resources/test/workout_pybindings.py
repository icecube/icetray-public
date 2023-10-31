#!/usr/bin/env python3

import math
import contextlib
import unittest
from icecube.icetray import I3Units, logging
from icecube import icetray, dataclasses

# with contextlib.suppress(ModuleNotFoundError):
#     import scipy.constants

#icetray.set_log_level(icetray.I3LogLevel.LOG_TRACE)


class TriggerKeyTestCase(unittest.TestCase):
    def testTriggerKeyBindings(self):
        tk = dataclasses.TriggerKey(dataclasses.IN_ICE, dataclasses.SIMPLE_MULTIPLICITY, 1002)
        self.assertIs(tk.check_config_id(), True, "CheckConfigID fail")
        self.assertEqual(tk.config_id, 1002, "Wrong ConfigID")
        self.assertEqual(tk.source, dataclasses.IN_ICE, "Wrong Trigger Source")
        self.assertEqual(tk.type, dataclasses.SIMPLE_MULTIPLICITY, f"Wrong Trigger Type: {tk.type=}")
        logging.log_debug("tk: %s" % tk)
        tk.reset_config_id()
        self.assertIs(tk.check_config_id(), False, "CheckConfigID fail")


# FIXME: this is a silly test
class TriggerTestCase(unittest.TestCase):
    def testTriggerBindings(self):
        mytrig = dataclasses.I3Trigger()
        mytrig.fired = True
        mytrig.time = 5.0 * icetray.I3Units.microsecond
        mytrig.length = 1.0 * icetray.I3Units.microsecond
        logging.log_debug("mytrig: %s" % mytrig)


# FIXME: this is also a silly test
class TriggerHierarchyTestCase(unittest.TestCase):
    def testTriggerHierarchy(self):
        # TriggerHierarchy is RO right now from the python side.
        my_th = dataclasses.I3TriggerHierarchy()
        logging.log_debug("my_th: %s" % my_th)


class I3ConstantsTestCase(unittest.TestCase):
    def testI3Constants(self):
        # Avogadro's constant
        logging.log_debug("NA        : %s" % dataclasses.I3Constants.NA)
        self.assertEqual(dataclasses.I3Constants.NA, 6.0221415e23)
        # with contextlib.suppress(NameError):
        #     logging.log_debug("NA (scipy): %s" % scipy.constants.N_A)
        #     self.assertEqual(dataclasses.I3Constants.NA, scipy.constants.N_A, "Our Avogadro's number is not equal to scipy's")

        # Conversion between IceCube and AMANDA coordinates
        # from docushare file by Kurt Woschnagg
        # (x,y,z)icecube = (x,y,z)amanda - (x,y,z)shift
        # static const double Coordinate_shift_x = -339.8*I3Units::m
        # static const double Coordinate_shift_y = -117.4*I3Units::m
        # static const double Coordinate_shift_z = -216.0*I3Units::m
        logging.log_debug("Coordinate_shift_x: %s" % dataclasses.I3Constants.Coordinate_shift_x)
        logging.log_debug("Coordinate_shift_y: %s" % dataclasses.I3Constants.Coordinate_shift_y)
        logging.log_debug("Coordinate_shift_z: %s" % dataclasses.I3Constants.Coordinate_shift_z)
        self.assertEqual(dataclasses.I3Constants.Coordinate_shift_x, -339.8, "Unexpected coordinate shift x")
        self.assertEqual(dataclasses.I3Constants.Coordinate_shift_y, -117.4, "Unexpected coordinate shift y")
        self.assertEqual(dataclasses.I3Constants.Coordinate_shift_z, -216.0, "Unexpected coordinate shift z")

        # Icecube elevations -- http://wiki.icecube.wisc.edu/index.php/Coordinate_system
        logging.log_debug("OriginElev:  %s" % dataclasses.I3Constants.OriginElev)
        logging.log_debug("SurfaceElev: %s" % dataclasses.I3Constants.SurfaceElev)
        self.assertEqual(dataclasses.I3Constants.OriginElev, 2900 * I3Units.feet, "Unexpected origin elevation")
        self.assertEqual(dataclasses.I3Constants.SurfaceElev, 9291.31 * I3Units.feet, "Unexpected surface elevation")

        # speed of light in a vacuum
        logging.log_debug("c:         %s" % dataclasses.I3Constants.c)
        with contextlib.suppress(NameError):
            logging.log_debug("c (scipy): %s" % scipy.constants.c)
            self.assertEqual(dataclasses.I3Constants.c, scipy.constants.c * I3Units.m / I3Units.second, "c != c")


class I3DirectionTestCase(unittest.TestCase):
    def testI3Direction(self):
        d = dataclasses.I3Direction(1.0, 1.0, 1.0)
        logging.log_debug("dir: %s" % d)
        self.assertEqual(d.theta, 0.9553166181245092)  # radians
        self.assertEqual(d.phi, 0.7853981633974483)  # radians
        self.assertEqual(d.azimuth, 3.9269908169872414)  # radians
        self.assertEqual(d.zenith, 2.186276035465284)  # radians

        dir2 = dataclasses.I3Direction(1.0, 0.0, 0.0)
        logging.log_debug("dir2: %s" % d)
        self.assertEqual(dir2.azimuth / I3Units.deg, 180)
        self.assertEqual(dir2.zenith / I3Units.deg, 90)

        dir2.rotate_z(90 * icetray.I3Units.deg)
        logging.log_debug("dir2 (rotated): %s" % d)
        self.assertEqual(dir2.azimuth / I3Units.deg, 270)
        self.assertEqual(dir2.zenith / I3Units.deg, 90)


class I3DOMFunctionsTestCase(unittest.TestCase):
    def testI3DOMFunctions(self):
        ds = dataclasses.I3DOMStatus()
        dc = dataclasses.I3DOMCalibration()

        transittime = dataclasses.transit_time(ds, dc)
        logging.log_debug("transittime: %s" % transittime)
        self.assertTrue(math.isnan(transittime))

        dc.mean_fadc_charge = 0.6
        dc.mean_atwd_charge = 0.7

        spe_charge_dist = dataclasses.SPEChargeDistribution()

        spe_charge_dist.exp1_amp = 0.1
        spe_charge_dist.exp1_width = 0.2
        spe_charge_dist.exp2_amp = 0.1
        spe_charge_dist.exp2_width = 0.2
        spe_charge_dist.gaus_amp = 0.3
        spe_charge_dist.gaus_mean = 0.4
        spe_charge_dist.gaus_width = 0.5
        spe_charge_dist.compensation_factor = 0.9
        spe_charge_dist.slc_gaus_mean = 0.5

        dc.combined_spe_charge_distribution = spe_charge_dist
        self.assertIs(dc.combined_spe_charge_distribution.is_valid, True, "This should be true.")

        spe_charge_dist.gaus_width = math.nan
        dc.combined_spe_charge_distribution = spe_charge_dist
        self.assertIs(dc.combined_spe_charge_distribution.is_valid, False, "This should be false.")

        dc.mean_fadc_charge = math.nan
        self.assertIs(dc.is_mean_fadc_charge_valid, False, "This should be false.")
        dc.mean_fadc_charge = 0.0
        self.assertIs(dc.is_mean_fadc_charge_valid, False, "This should be false.")

        dc.mean_atwd_charge = math.nan
        self.assertIs(dc.is_mean_atwd_charge_valid, False, "This should be false.")
        dc.mean_atwd_charge = 0.0
        self.assertIs(dc.is_mean_atwd_charge_valid, False, "This should be false.")


class I3DoubleTestCase(unittest.TestCase):
    def testI3Double(self):
        doub = dataclasses.I3Double(99.9)
        doub2 = dataclasses.I3Double(99.9)
        doub3 = dataclasses.I3Double(doub.value + doub2.value)
        logging.log_debug("doub3: %s" % doub3)
        self.assertEqual(doub3.value, 199.8)


# Testing I3Maps, in this case, I3MapStringInt
class I3MapTestCase(unittest.TestCase):
    def testI3Map(self):
        mapy = dataclasses.I3MapStringInt()
        mapy["foo"] = 1
        mapy["bar"] = 2

        logging.log_debug("mapy: %s" % mapy)
        self.assertTrue("foo" in mapy, "Failed to search the I3Map")
        self.assertEqual(mapy.get("bar"), 2, "Failed to get from I3Map")


class I3VectorTestCase(unittest.TestCase):
    def testI3Vector(self):
        vecy = dataclasses.I3VectorInt([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
        vecy.append(11)

        logging.log_debug("vecy: %s" % vecy)
        self.assertEqual(len(vecy), 11)
        self.assertEqual(vecy[-1], 11)


class I3PositionTestCase(unittest.TestCase):
    def testI3Position(self):
        pos = dataclasses.I3Position(1.1, 2.2, 3.3)
        logging.log_debug("pos: %s" % pos)

        pos2 = dataclasses.I3Position(0, 0, 0)
        distance = abs(pos - pos2)
        logging.log_debug(f"{pos.r=}")
        logging.log_debug(f"{distance=}")
        self.assertLess(abs(pos.r - distance), 1e-5 * I3Units.m, "Distances don't match")

        pos3 = dataclasses.I3Position(1, 2, 3)
        pos3[2] = 4
        self.assertEqual(pos3[2], 4, "Index lookup failed")
        self.assertEqual(pos3.z, 4, "Member lookup failed")
        try:
            pos3[3] = 5
            raise RuntimeError("I3Position did not correctly throw an exception!")
        except IndexError:
            logging.log_debug("Position correctly sent index error for index 3")


class I3TimeTestCase(unittest.TestCase):
    def testI3Time(self):
        time = dataclasses.I3Time()
        time.set_daq_time(2010, 111111111123456789)
        logging.log_debug("time: %s" % time)
        logging.log_debug("time.mod_julian_day_double: %s" % time.mod_julian_day_double)
        logging.log_debug("time.mod_julian_day: %s" % time.mod_julian_day)
        logging.log_debug("time.mod_julian_sec: %s" % time.mod_julian_sec)
        logging.log_debug("time.mod_julian_nano_sec: %s" % time.mod_julian_nano_sec)

        time2 = dataclasses.I3Time(2009, 111111111123456789)
        logging.log_debug("time2: %s" % time2)

        self.assertGreater(time, time2, "Time order not correct")
        self.assertEqual(time.unix_time - time2.unix_time, 31536000, "Time difference is not exactly one year (to the second)")


class I3DetectorStatus(unittest.TestCase):
    def testI3DetectorStatus(self):
        ds = dataclasses.I3DetectorStatus()
        dmstat = ds.dom_status
        adom = dataclasses.I3DOMStatus()
        adom.pmt_hv = 1500 * icetray.I3Units.V
        dmstat[icetray.OMKey(1, 1)] = adom
        mykey = icetray.OMKey(1, 1)
        self.assertTrue(mykey in dmstat, "Can not find my new dom status")


class I3GeometryTestCase(unittest.TestCase):
    def testI3Geometry(self):
        omg = dataclasses.I3OMGeo()
        omg.position.x = 100.0
        omg.position.y = 101.0
        omg.position.z = 102.0
        logging.log_debug("%s %s" % (type(omg.position), omg.position))
        i3g = dataclasses.I3Geometry()
        i3g.omgeo[icetray.OMKey(1, 1)] = omg
        self.assertTrue(icetray.OMKey(1, 1) in i3g.omgeo, "can not find my omkey in i3geo")
        self.assertFalse(icetray.OMKey(1, 2) in i3g.omgeo, "found a bad omkey in i3geo")
        newom = i3g.omgeo[icetray.OMKey(1, 1)]
        logging.log_debug("%s %s %s" % (type(newom.position), vars(newom.position), newom.position))
        self.assertGreater(newom.position.x, 99.0, "Failed to get the right OMGeo position back")

    # The "GetTankGeo" functions of I3Geometry
    ## We'll make this a "swapped tanks" entry in the StationGeo where TankB comes first,
    ## just to make sure it works under those conditions.
        tank1 = dataclasses.I3TankGeo()
        tank1.position.x = 1.0
        tank1.position.y = 2.0
        tank1.position.z = 3.0
        tank1.snowheight = 4.0
        tank1.omkey_list = [icetray.OMKey(1, 63), icetray.OMKey(1, 64)]

        tank2 = dataclasses.I3TankGeo()
        tank2.position.x = 6.0
        tank2.position.y = 7.0
        tank2.position.z = 8.0
        tank2.snowheight = 5.0
        tank2.omkey_list = [icetray.OMKey(1, 61), icetray.OMKey(1, 62)]

        logging.log_debug("Tank1 = %s %s %s contains OMKeys: %s" % (type(tank1), tank1.position, tank1.snowheight, tank1.omkey_list))
        logging.log_debug("Tank2 = %s %s %s contains OMKeys: %s" % (type(tank2), tank2.position, tank2.snowheight, tank2.omkey_list))

        i3g.stationgeo[1] = [tank1, tank2]
        get1_byom = i3g.tankgeo(icetray.OMKey(1, 63))
        get2_byom = i3g.tankgeo(icetray.OMKey(1, 62))  # 61 would've worked too
        self.assertEqual(get1_byom, tank1, "We didn't grab Tank1 (OMKey)")
        self.assertEqual(get2_byom, tank2, "We didn't grab Tank2 (OMKey)")

        get1_bytank = i3g.tankgeo(dataclasses.TankKey(1, dataclasses.TankKey.TankB))
        get2_bytank = i3g.tankgeo(dataclasses.TankKey(1, dataclasses.TankKey.TankA))
        self.assertEqual(get1_bytank, tank1, "We didn't grab Tank1 (TankKey)")
        self.assertEqual(get2_bytank, tank2, "We didn't grab Tank2 (TankKey)")


class I3DOMLaunchTestCase(unittest.TestCase):
    def testI3DOMLaunch(self):
        dl = dataclasses.I3DOMLaunch()
        fadc = icetray.vector_int()
        for i in range(10):
            fadc.append(i)
        dl.raw_fadc = fadc
        logging.log_debug("fadc: %s" % fadc)

        vc = dataclasses.I3VectorChar()
        for c in "I am an I3Vector<char>":
            vc.append(c)
        logging.log_debug("vc: %s" % vc)

        atwd0 = icetray.vector_int()
        [atwd0.append(i) for i in range(100)]
        dl.raw_atwd[0] = atwd0
        dl.lc_bit = True
        self.assertIs(dl.lc_bit, True, 'Got wrong LCBit')

        dl.trigger_type = dataclasses.I3DOMLaunch.SPE_DISCRIMINATOR_TRIGGER
        dl.trigger_mode = dataclasses.I3DOMLaunch.LC_UPPER
        dl.which_atwd = dataclasses.I3DOMLaunch.ATWDb

        launch_time = 10 * I3Units.microsecond
        dl.time = launch_time
        self.assertEqual(dl.time, launch_time, "Got the wrong time")

        dl.lc_bit = True
        self.assertIs(dl.lc_bit, True, 'Got wrong LCBit')

        dl.trigger_type = dataclasses.I3DOMLaunch.SPE_DISCRIMINATOR_TRIGGER
        dl.trigger_mode = dataclasses.I3DOMLaunch.LC_UPPER
        dl.which_atwd = dataclasses.I3DOMLaunch.ATWDb

        logging.log_debug("dl: %s" % dl)


class I3EventHeaderTestCase(unittest.TestCase):
    def testI3EventHeader(self):
        header_t = dataclasses.I3Time(2008, 87695866111263130)
        header = dataclasses.I3EventHeader()
        header.run_id = 110811
        header.event_id = 0
        header.start_time = header_t
        header.end_time = header_t + 10 * icetray.I3Units.microsecond
        header.sub_event_stream = 'MyFirstSplit'
        # the str() of the I3Header we just createad should look like this
        h = """[ I3EventHeader:
        StartTime: 2008-04-11 11:59:46.611,126,313,0 UTC
         EndTime : 2008-04-11 11:59:46.611,136,313,0 UTC
           RunID : 110811
        SubrunID : 4294967295
         EventID : 0
      SubEventID : 0
  SubEventStream : MyFirstSplit
]"""
        self.assertEqual(str(header), h)


class I3ParticleTestCase(unittest.TestCase):
    def testI3Particle(self):
        mypart = dataclasses.I3Particle()
        mypart.dir = dataclasses.I3Direction(90.0 * icetray.I3Units.deg, 180 * icetray.I3Units.deg)
        mypart.pos = dataclasses.I3Position(10.0 * icetray.I3Units.m, 10.0 * icetray.I3Units.m, 10.0 * icetray.I3Units.m)
        mypart.time = 0.0 * icetray.I3Units.ns
        mypart.energy = 1000.0 * icetray.I3Units.GeV
        mypart.shape = dataclasses.I3Particle.InfiniteTrack
        mypart.fit_status = dataclasses.I3Particle.OK
        my_other_particle = mypart.clone()
        # everything but the minor ID should be the same
        # we could really use a compare clone method
        self.assertNotEqual(my_other_particle, mypart, "Clone is equal to original (minor ID should differ)")

        logging.log_debug("mypart: %s" % mypart)
        logging.log_debug("my_other_particle: %s" % my_other_particle)

        mypartvec = dataclasses.I3VectorI3Particle()
        mypartvec.append(mypart)

        self.assertEqual(len(mypartvec), 1, "vector should only have one particle")
        self.assertEqual(mypartvec[0], mypart, "particle retrieved is not the same as the one inserted")


# FIXME: this is also a silly test. it does nothing but construct, stuff, and print
class I3RecoPulseTestCase(unittest.TestCase):
    def testI3RecoPulse(self):
        rp = dataclasses.I3RecoPulse()
        rp.charge = 1.5  ## PEs
        rp.time = 100.0 * icetray.I3Units.ns
        rp.flags = dataclasses.I3RecoPulse.PulseFlags.ATWD

        logging.log_debug("rp: %s" % rp)
        trigger_vect = dataclasses.I3VectorI3Trigger()
        trigger_vect.append(dataclasses.I3Trigger())
        logging.log_debug("trigger_vect: %s" % trigger_vect)

        rps = dataclasses.I3RecoPulseSeries()
        rps.append(rp)

        for pulse in rps:
            logging.log_debug("pulse: %s" % pulse)

        rpsm = dataclasses.I3RecoPulseSeriesMap()
        rpsm[icetray.OMKey(1, 1)] = rps

        for key, pseries in rpsm:
            logging.log_debug("key: %s" % key)
            for pulse in pseries:
                logging.log_debug("pulse: %s" % pulse)


class I3WaveformTestCase(unittest.TestCase):
    def testI3Waveform(self):
        my_wf = dataclasses.I3Waveform()
        my_wf.time = 100.0 * icetray.I3Units.ns
        my_wf.bin_width = 3.3 * icetray.I3Units.ns
        awave = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]
        my_wf.waveform = awave
        my_wf.digitizer = dataclasses.I3Waveform.Source.FADC
        my_wf.hlc = True

        wv_series = dataclasses.I3WaveformSeries()
        wv_series.append(my_wf)
        for wv in wv_series:
            logging.log_debug("wv.time: %s" % wv.time)

        wv_ser_map = dataclasses.I3WaveformSeriesMap()
        wv_ser_map[icetray.OMKey(2, 2)] = wv_series

        self.assertTrue(icetray.OMKey(2, 2) in wv_ser_map, 'I3Waveform not present in map!')
        self.assertFalse(icetray.OMKey(2, 3) in wv_ser_map, 'mystery I3waveform in map!')

        logging.log_debug("my_wf: %s" % my_wf)


if __name__ == "__main__":
    unittest.main()
