#!/usr/bin/env python3
#
# @copyright (C) 2015 The IceCube Collaboration
# 
# @author Kevin Meagher
# @date August 2015

"""
The azimuth of the sun at noon UTC should be close to the value of the "Equation of Time" 
"""

import math
import unittest
from datetime import datetime,timedelta
from icecube.icetray import I3Units
from icecube import astro,dataclasses

#Equation of time parameters from wikipedia
epsilon=23.45*2*math.pi/360
alpha  =78.5 *2*math.pi/360
kappa  =0.016722

def delta(t,kappa):
  return( 2*math.sin(t)*kappa+5/4*math.sin(2*t)*kappa**2 )

def equation_of_time(t) :
    t = (t -0.5) *2.*math.pi /365.25  #convert to phase
    zaehl= -math.sin(delta(t,kappa))+math.tan(epsilon/2)**2*math.sin(2*(t-alpha)+delta(t,kappa))
    nenn =  math.cos(delta(t,kappa))+math.tan(epsilon/2)**2*math.cos(2*(t-alpha)+delta(t,kappa))
    return( math.atan(zaehl/nenn)*24*60/2/math.pi )

class EqnOfTime(unittest.TestCase):
    def test_eqn_of_time(self):
      
      for n in range(3000):
    
        i3time = dataclasses.make_I3Time(datetime(2010,1,1,12)+timedelta(n))
        sun = astro.I3GetSunDirection(i3time)

        deltat = -(90 - sun.azimuth/I3Units.degree)*4 #convert to minutes
        assert(abs(deltat - equation_of_time(n))<0.3)
    
if __name__=='__main__':
    unittest.main()


