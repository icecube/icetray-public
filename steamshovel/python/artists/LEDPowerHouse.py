from __future__ import division

import os
import time
import struct
import glob
import json

import serial

def find_devices():
    """Find activated serial devices"""
    ret = []
    for f in glob.iglob('/dev/ttyACM*'):
        try:
            s = serial.Serial(f,230400,timeout=0.1)
            s.write('P')
            if s.read(1) == 'P':
                ret.append(f)
            s.close()
        except Exception:
            pass
    return sorted(ret)
devices = find_devices()
if not devices:
    raise ImportError('no Teensy controllers detected')
print(devices)

from icecube import dataclasses
from icecube.shovelart import *


class Teensy:
    """Wrapper for the Teensy when controlling LED strings."""
    def __init__(self, device='/dev/ttyACM0'):
        self.ser = serial.Serial(device, 230400)
        self.strings = {}
    def __enter__(self):
        return self
    def __exit__(self, exc_type, exc_value, traceback):
        try:
            self.clear()
        except Exception:
            pass
    def __getitem__(self,key):
        s,om = key
        if s in self.strings and om in self.strings[s]:
            return self.strings[s][om]
        else:
            return None
    def __setitem__(self,key,value):
        if self.ser is None:
            raise Exception('must use context manager to open serial connection')
        s,om = key
        r,g,b = value
        # write data to serial port
        self.ser.write(struct.pack('<cBcBBBBB','S',s,'L',om/256,om%256,r,g,b))
        # store state
        if s not in self.strings:
            self.strings[s] = {}
        self.strings[s][om] = (r,g,b)
    def clear(self):
        if self.ser is None:
            raise Exception('must use context manager to open serial connection')
        self.ser.write('C')
        self.strings = {}
    def set_size(self,strings,oms):
        if self.ser is None:
            raise Exception('must use context manager to open serial connection')
        self.ser.write(struct.pack('<cBBB','M',strings,oms/256,oms%256))

class LedLayout:
    """
    Ways the Teensy Leds can be laid out.

    The layout config looks like:
    { (icecube string, om) : (teensy device, teensy string, led index) }
    """
    def __init__(self, config=None):
        if not config:
            config = {"0":{"34":[0,0,1],"33":[0,60,0],"32":[0,120,1],"31":[0,180,0],
                           "36":[2,0,1],"35":[2,60,0],"45":[2,120,1],"46":[2,180,0],
                           "44":[4,0,1],"43":[4,60,0],"42":[4,120,1],"41":[4,180,0],
                           "37":[5,0,1],"38":[5,60,0],"39":[5,120,1],"40":[5,180,0],
                           "47":[6,0,1],"48":[6,60,0],"49":[6,120,1],"50":[6,180,0]},
                     "1":{"51":[0,0,1],"60":[0,60,0],"68":[0,120,1],"75":[0,180,0],
                          "52":[1,0,1],"61":[1,60,0],"69":[1,120,1],"76":[1,180,0],
                          "53":[2,0,1],"62":[2,60,0],"70":[2,120,1],"77":[2,180,0],
                          "54":[3,0,1],"63":[3,60,0],"71":[3,120,1],"78":[3,180,0],
                          "55":[4,0,1],"64":[4,60,0],"72":[4,120,1],
                          "56":[5,0,1],"65":[5,60,0],"73":[5,120,1],
                          "57":[6,0,1],"66":[6,60,0],"74":[6,120,1],
                          "58":[7,0,1],"67":[7,60,0],"59":[7,120,1]},
                     "2":{"30":[0,0,1],"21":[0,60,0],"13":[0,120,1],"6":[0,180,0],
                          "29":[1,0,1],"20":[1,60,0],"12":[1,120,1],"5":[1,180,0],
                          "28":[2,0,1],"19":[2,60,0],"11":[2,120,1],"4":[2,180,0],
                          "27":[3,0,1],"18":[3,60,0],"10":[3,120,1],"3":[3,180,0],
                          "26":[4,0,1],"17":[4,60,0],"9":[4,120,1],"2":[4,180,0],
                          "25":[5,0,1],"16":[5,60,0],"8":[5,120,1],"1":[5,180,0],
                          "24":[6,0,1],"15":[6,60,0],"7":[6,120,1],
                          "23":[7,0,1],"14":[7,60,0],"22":[7,120,1]
                     }}
        self.load_layout(config)
        self.teensies = devices
    def load_layout(self,config):
        """
        config details:
        {teensy id: {icecube string: [teensy string, led index start, reversed]} }
        """
        self.layout = {}
        for teensy in config:
            for i_string in config[teensy]:
                t_string,led,rev = config[teensy][i_string]
                for om in range(1,61):
                    led_om = int(led)+(60-om if rev else om)
                    self.layout[(int(i_string),om)] = (int(teensy),int(t_string),led_om)
    def start(self):
        if isinstance(self.teensies[0],Teensy):
            return
        self.teensies = [Teensy(t) for t in self.teensies]
    def stop(self):
        for t in self.teensies:
            t.clear()
    def draw(self,data):
        for s,om in data:
            try:
                layout = self.layout[(s,om)]
                self.teensies[layout[0]][layout[1:3]] = data[(s,om)]
            except:
                pass

class LEDPowerHouse( PyArtist ):

    numRequiredKeys = 1

    def __init__(self):
        PyArtist.__init__(self)

        self.defineSettings( { "brightness": RangeSetting(0.0, 1.0, 100, 0.2),
                               "sleep": RangeSetting(0.0, 100.0, 100, 0.0),
                               "scale": RangeSetting(-2.0, 1.0, 100, -0.65),
                               "power": RangeSetting(.01,.5, 100, 0.157),
                               "colormap": I3TimeColorMap() } )

        # scale and power should be chosen such that all events have
        # a maximum brightness < 1.0

        try:
            config_path = os.path.expandvars('$HOME/.steamshovel_led_config')
            config = json.load(open(config_path))
        except Exception:
            config = None

        try:
            self.led_output = LedLayout(config)
        except Exception:
            print('error setting up led_output')
            raise

    def isValidKey( self, frame, key_idx, key ):
        tn = frame.type_name(key)
        if key_idx == 0:
            return (tn.startswith( "I3RecoPulseSeriesMap" ) or
                    ("OMKey" in str(tn) and "RecoPulse" in str(tn)))
        return False

    def create( self, frame, output ):
        scale = self.setting("scale")
        power = self.setting("power")
        colormap = self.setting("colormap")
        max_brightness = self.setting("brightness")

        frame_key = self.keys()[0]
        pulse_series_map = frame[ frame_key ]
        if not hasattr(pulse_series_map,'keys'):
            pulse_series_map = dataclasses.I3RecoPulseSeriesMap.from_frame( frame, frame_key )

        def rescale( charge ):
            return 10 ** scale * charge ** power

        # Second pass: prepare color and brightness data.
        self.data = [] # store omkey, color, brightness
        max_bright = 0.0
        ncut = 0
        for omkey, pulse_series in pulse_series_map:
            if len(pulse_series) == 0:
                continue

            accumulated_charge = 0.0
            brightness = StepFunctionFloat(0)
            for pulse in pulse_series:
                accumulated_charge += pulse.charge
                br = rescale( accumulated_charge )
                if br > max_bright:
                    max_bright = br
                if br > 1.0:
                    br = 1.0 # limit br to [0.0, 1.0]
                    ncut += 1
                br *= max_brightness
                brightness.add( br, pulse.time )

            t0 = pulse_series[0].time
            color = TimeWindowColor( output, t0, colormap )

            datum = ( omkey.string,
                      omkey.om,
                      color,
                      brightness )
            self.data.append( datum )

        if max_bright > 1.0:
            print("Warning: %i LEDs cut off at maximum brightness -> reduce scale" % ncut)
        #print("Brightest LED was at uncorrected brightness %g" % max_bright)

        output.addPhantom( self.cleanup, self.update )

        self.led_output.start()

    def update( self, vistime ):
        'Put draw code here'
        ret = {}
        for datum in self.data:
            if datum[1] >= 60:
                pass # no icetop here
            col = datum[2].value( vistime )
            br = datum[3].value( vistime )
            ret[(datum[0],datum[1])] = (int(col.red*br), int(col.green*br), int(col.blue*br))
        try:
            self.led_output.draw(ret)
        except Exception:
            pass

    def cleanup( self ):
        'Put cleanup code here'
        try:
            self.led_output.stop()
        except Exception:
            pass
