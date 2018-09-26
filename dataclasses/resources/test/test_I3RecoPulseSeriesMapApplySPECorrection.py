#!/usr/bin/env python

import unittest
from icecube import icetray, dataclasses
from I3Tray import I3Units

class I3RecoPulseSeriesMapApplySPECorrection(unittest.TestCase):

    def test_PulseChargeShifting(self):
        frame = icetray.I3Frame()
        omkey = icetray.OMKey(1,1)
        
        # create a calibration object
        calibration = dataclasses.I3Calibration()
        calibration.dom_cal[omkey] = dataclasses.I3DOMCalibration()
        calibration.dom_cal[omkey].mean_atwd_charge = 1.2
        calibration.dom_cal[omkey].mean_fadc_charge = 1.8
        frame["I3Calibration"] = calibration
        
        # create some pulses on our fake DOM
        pulse1 = dataclasses.I3RecoPulse()
        pulse1.flags = dataclasses.I3RecoPulse.PulseFlags.ATWD
        pulse1.time = 1. * I3Units.nanosecond
        pulse1.charge = 10.
        pulse1.width = 1.

        pulse2 = dataclasses.I3RecoPulse()
        pulse2.flags = dataclasses.I3RecoPulse.PulseFlags.FADC
        pulse2.time = 10. * I3Units.nanosecond
        pulse2.charge = 5.
        pulse2.width = 1.
        
        pulse_series = dataclasses.I3RecoPulseSeriesMap()
        pulse_series[omkey] = dataclasses.I3RecoPulseSeries()
        pulse_series[omkey].append(pulse1)
        pulse_series[omkey].append(pulse2)

        # add these pulses to our new frame
        frame["UnshiftedPulses"] = pulse_series
        
        # create a shifter object
        frame["ShiftedPulses"] = dataclasses.I3RecoPulseSeriesMapApplySPECorrection(pulses_key="UnshiftedPulses", calibration_key="I3Calibration")
        
        # retrieve the shifted pulses
        shifted_pulses = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, "ShiftedPulses")
        
        # make sure everything is as expected
        self.assertEqual(pulse_series[omkey][0].time,   shifted_pulses[omkey][0].time, "these should be the same.")
        self.assertEqual(pulse_series[omkey][0].width,  shifted_pulses[omkey][0].width, "these should be the same.")
        self.assertEqual(pulse_series[omkey][0].flags,  shifted_pulses[omkey][0].flags, "these should be the same.")
        self.assertAlmostEqual(
          pulse_series[omkey][0].charge/calibration.dom_cal[omkey].mean_atwd_charge,
          shifted_pulses[omkey][0].charge,
          places=4,msg="these should be the same.")

        self.assertEqual(pulse_series[omkey][1].time,   shifted_pulses[omkey][1].time, "these should be the same.")
        self.assertEqual(pulse_series[omkey][1].width,  shifted_pulses[omkey][1].width, "these should be the same.")
        self.assertEqual(pulse_series[omkey][1].flags,  shifted_pulses[omkey][1].flags, "these should be the same.")
        self.assertAlmostEqual(
          pulse_series[omkey][1].charge/calibration.dom_cal[omkey].mean_fadc_charge,
          shifted_pulses[omkey][1].charge,
          places=4,msg="these should be the same.")

unittest.main()        

