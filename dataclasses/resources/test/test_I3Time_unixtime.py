#!/usr/bin/env python

from icecube.dataclasses import I3Time
from icecube.icetray import I3Units
from datetime import datetime,timedelta,timezone

import os.path
from pprint import pprint

dates = []
for year in range(2004,2022):
    for month in range(1,13):    

        t0=I3Time()
        t0.set_utc_cal_date(year,month,1,0,0,0,0.)    
        dt0 = datetime(year,month,1,0,0,0,0,timezone.utc)
        
        dt1=dt0-timedelta(0,1)
        tup = tuple(dt1.timetuple())[:6]
        
        t1=I3Time()
        t1.set_utc_cal_date(*tup,0.)
        
        assert t0.unix_time==dt0.timestamp()
        assert t1.unix_time==dt1.timestamp()
        assert t0.unix_time - t1.unix_time
        assert dt0.timestamp() - dt1.timestamp()
        
        t3=I3Time()
        t3.set_unix_time(int(dt0.timestamp()),0)    
        assert t3.unix_time==dt0.timestamp()
        t3.set_unix_time(int(dt1.timestamp()),0)    
        assert t3.unix_time == dt1.timestamp()

        t2=t1+1*I3Units.s        
        if (t2.is_leap_second):
            assert t0 - t1 == 2.*I3Units.s
            assert t2.unix_time + 1 == t0.unix_time
            assert t2.unix_time == t1.unix_time
        else:
            assert t0-t1 == 1.*I3Units.s
            assert t0 == t2


