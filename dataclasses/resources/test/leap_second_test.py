#!/usr/bin/env python
"""
Leap Second Unit Test Suite 

This file tests I3Time for leap second awareneness.
It uses a file of days with leap seconds from the Navy and tests to
make sure I3Time works well with leap seconds.

It depends on http://maia.usno.navy.mil/ser7/tai-utc.dat, which is
kept in ${I3_TESTDATA}. The USNO updates this file every six
months, hence the age test.

"""

import os,datetime,random
from icecube import dataclasses
from icecube.icetray import I3Units

random.seed(42)	# Make repeatable

def approx_Equal(x, y, tolerance=0.001):
    return abs(x-y) <= 0.5 * tolerance * (x + y)

now = datetime.datetime.now()

filename = os.path.join(os.getenv("I3_TESTDATA"),'dataclasses','tai-utc.dat')

#store Modified julian date of days with leap seconds here
leap_sec_mjd = []

#The first 13 lines of this file contian an older way to do leapseconds which arn't supported
for line in open(filename).readlines()[13:]:
    #column 17-23 contian the julian date of the day after a leap second
    #subract 2400000 to obtian the modified julian date
    leap_sec_mjd.append(int(line[17:24])-2400000)

#first make sure first leap second is JAN 1, 1972
assert(leap_sec_mjd[0]==41317)

#test every day from 1970 to 
for mjd in range(40587,max(dataclasses.make_I3Time(now).mod_julian_day,leap_sec_mjd[-1])+3*365):
    if mjd+1 in leap_sec_mjd:
        if not dataclasses.leap_second_on_mjd(mjd):
            err_str = "MJD "+str(mjd)+" not in I3Time list of leap seconds, "\
                "possibly a leap second was added and I3Time.cxx needs to be updated"
            raise Exception(err_str)
    elif dataclasses.leap_second_on_mjd(mjd):
        err_str = str(mjd)+" incorrectly registers as a leap seconds"
        raise Exception(err_str)

#instantiate I3Time for leapsecond testing
t1=dataclasses.I3Time()
t2=dataclasses.I3Time()

#check every year from 1970 to 3 years in the future
for year in range (1970,max(dataclasses.year_of(leap_sec_mjd[-1]),now.year)+3):
    #twleve hours before tranition to July
    t1.set_utc_cal_date(year,6,30,12, 0, 0,0)
    #twleve hours after  tranition to July
    t2.set_utc_cal_date(year,7, 1,12, 0, 0,0)

    #true if this june-july transition has leap second 
    leap_sec = t2.mod_julian_day in leap_sec_mjd

    #test I3Time-I3Time
    #difference between t1 and t2 should be 86400 seconds +/- one leap second
    assert (t1-t2)/I3Units.second+86400 == -leap_sec
    assert (t2-t1)/I3Units.second-86400 ==  leap_sec

    #test I3Time+double and I3Time-double
    if leap_sec:        
        #if leap second is present adding one day and one second should get 
        #you noon the next day
        assert(t1+86401*I3Units.second==t2)
        assert(t2-86401*I3Units.second==t1)
    else:
        #with no leap second present adding exactly one day should get 
        #you noon the next day
        assert(t1+86400*I3Units.second==t2)
        assert(t2-86400*I3Units.second==t1)

    #re-initilize t1, t2, and t3
    t1=dataclasses.I3Time()
    t2=dataclasses.I3Time()
    t3=dataclasses.I3Time()

    if leap_sec:

        #set to leap second itsself
        nanosec = random.randint(0,1e9)
        t1.set_utc_cal_date(year,6,30,23,59,59,nanosec)
        t2.set_utc_cal_date(year,6,30,23,59,60,nanosec)
        t3.set_utc_cal_date(year,7, 1, 0, 0, 0,nanosec)       

        assert not t1.is_leap_second
        assert t2.is_leap_second
        assert not t3.is_leap_second
             
        # Python's date time doesn't handle leap seconds, 
        # so it sets the time to 23:59:59.999999

        dt1=t1.date_time
        dt2=t2.date_time
        dt3=t3.date_time
        
        assert dt1                          == datetime.datetime(year,6,30,23,59,59,int(nanosec/1000.))
        assert dt1+datetime.timedelta(0, 1) == datetime.datetime(year,7, 1, 0, 0, 0,int(nanosec/1000.))
        assert dt2                          == datetime.datetime(year,6,30,23,59,59,999999)
        assert dt3                          == datetime.datetime(year,7, 1, 0, 0, 0,int(nanosec/1000.))
        assert dt3+datetime.timedelta(0,-1) == datetime.datetime(year,6,30,23,59,59,int(nanosec/1000.))

        #although the invalid python leap second object return true when compared to eachother
        assert (t1+1*I3Units.second).date_time == dt2
        assert (t1+2*I3Units.second).date_time == dt3
        assert (t2+1*I3Units.second).date_time == dt3
        assert (t2-1*I3Units.second).date_time == dt1
        assert (t3-1*I3Units.second).date_time == dt2
        assert (t3-2*I3Units.second).date_time == dt1


        #mjd = t1.mod_julian_day
        assert t1.mod_julian_day    == t2.mod_julian_day 
        assert t1.mod_julian_day +1 == t3.mod_julian_day
        assert t1.mod_julian_day not in leap_sec_mjd
        assert t3.mod_julian_day in leap_sec_mjd

        assert int(t1.mod_julian_day_double) == t1.mod_julian_day
        assert int(t2.mod_julian_day_double) == t2.mod_julian_day
        assert int(t3.mod_julian_day_double) == t3.mod_julian_day

        assert approx_Equal(t1.mod_julian_day_double           , t2.mod_julian_day_double,1e-10)
        assert approx_Equal(t1.mod_julian_day_double + 1/86400., t3.mod_julian_day_double,1e-10)

        assert t1.mod_julian_nano_sec == nanosec
        assert t2.mod_julian_nano_sec == nanosec
        assert t3.mod_julian_nano_sec == nanosec

        assert t1.mod_julian_sec == 86399 
        assert t2.mod_julian_sec == 86400
        assert t3.mod_julian_sec ==     0

        assert t2.unix_time + 1 == t3.unix_time
        assert t1.unix_time + 1 == t3.unix_time
        assert t1.unix_time == (t3.mod_julian_day-40587)*86400 -1 
        assert t2.unix_time == (t3.mod_julian_day-40587)*86400 -1
        assert t3.unix_time == (t3.mod_julian_day-40587)*86400 


        assert t3.utc_daq_time - t2.utc_daq_time == 1e10
        assert t2.utc_daq_time - t1.utc_daq_time == 1e10
        assert t3.utc_daq_time - t1.utc_daq_time == 2e10
        assert t1.utc_daq_time == ( 181 + int(year%4==0) )*int(8.64e14) + nanosec*10 - int(1e10)
        assert t2.utc_daq_time == ( 181 + int(year%4==0) )*int(8.64e14) + nanosec*10 
        assert t3.utc_daq_time == ( 181 + int(year%4==0) )*int(8.64e14) + nanosec*10 + int(1e10)
 
        assert t1.utc_day_of_month == 30
        assert t2.utc_day_of_month == 30
        assert t3.utc_day_of_month ==  1

        assert t1.utc_month == dataclasses.I3Time.Jun
        assert t2.utc_month == dataclasses.I3Time.Jun
        assert t3.utc_month == dataclasses.I3Time.Jul

        assert t1.utc_nano_sec == nanosec
        assert t2.utc_nano_sec == nanosec
        assert t3.utc_nano_sec == nanosec
        
        #July 1 is the 181th day of the year on standard years and 182th on leap years
        assert t1.utc_sec == (181+ int(year%4==0))*86400 - 1
        assert t2.utc_sec == (181+ int(year%4==0))*86400
        assert t3.utc_sec == (181+ int(year%4==0))*86400 + 1

        assert t1.utc_year==year
        assert t2.utc_year==year
        assert t3.utc_year==year 
           
        #test that it prints out the leap second correctly
        assert t1.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-06-30 23:59:59"%t1.utc_year
        assert t2.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-06-30 23:59:60"%t1.utc_year
        assert t3.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-07-01 00:00:00"%t1.utc_year

        #test arithmatic 
        assert(t1+  I3Units.second==t2)
        assert(t2+  I3Units.second==t3)
        assert(t1+2*I3Units.second==t3)

        assert(t3-  I3Units.second==t2)
        assert(t2-  I3Units.second==t1)
        assert(t3-2*I3Units.second==t1)
        
        assert(t2-t1==  I3Units.second)
        assert(t3-t2==  I3Units.second)
        assert(t3-t1==2*I3Units.second)

        assert(t1-t2==  -I3Units.second)
        assert(t2-t3==  -I3Units.second)
        assert(t1-t3==-2*I3Units.second)

    else:
        nanosec = random.randint(0,1e9)
        t1.set_utc_cal_date(year,6,30,23,59,59,nanosec)
        t2.set_utc_cal_date(year,6,30,23,59,60,nanosec)
        t3.set_utc_cal_date(year,7, 1, 0, 0, 0,nanosec)       

        assert t2 == dataclasses.I3Time()
        assert not t1.is_leap_second
        assert not t3.is_leap_second
             
        dt1=t1.date_time
        dt3=t3.date_time
        
        assert dt1                          == datetime.datetime(year,6,30,23,59,59,int(nanosec/1000.))
        assert dt1+datetime.timedelta(0, 1) == datetime.datetime(year,7, 1, 0, 0, 0,int(nanosec/1000.))
        assert dt3                          == datetime.datetime(year,7, 1, 0, 0, 0,int(nanosec/1000.))
        assert dt3+datetime.timedelta(0,-1) == datetime.datetime(year,6,30,23,59,59,int(nanosec/1000.))

        #although the invalid python leap second object return true when compared to eachother
        assert (t1+1*I3Units.second).date_time == dt3
        assert (t3-1*I3Units.second).date_time == dt1


        #mjd = t1.mod_julian_day
        assert t1.mod_julian_day +1 == t3.mod_julian_day
        assert t1.mod_julian_day not in leap_sec_mjd
        assert t3.mod_julian_day not in leap_sec_mjd

        assert int(t1.mod_julian_day_double) == t1.mod_julian_day
        assert int(t3.mod_julian_day_double) == t3.mod_julian_day

        assert approx_Equal(t1. mod_julian_day_double + 1/86400. , t3.mod_julian_day_double,1e-10)

        assert t1.mod_julian_nano_sec == nanosec
        assert t3.mod_julian_nano_sec == nanosec

        assert t1.mod_julian_sec == 86399 
        assert t3.mod_julian_sec ==     0

        #as per ntp unix time of leap seconds is the next seconds's unix time
        assert t1.unix_time + 1 == t3.unix_time
        assert t1.unix_time == (t3.mod_julian_day-40587)*86400 -1 
        assert t3.unix_time == (t3.mod_julian_day-40587)*86400 


        assert t3.utc_daq_time - t1.utc_daq_time == 1e10
        assert t1.utc_daq_time == ( 181 + int(year%4==0) )*int(8.64e14) + nanosec*10 - int(1e10)
        assert t3.utc_daq_time == ( 181 + int(year%4==0) )*int(8.64e14) + nanosec*10 
 
        assert t1.utc_day_of_month == 30
        assert t3.utc_day_of_month ==  1

        assert t1.utc_month == dataclasses.I3Time.Jun
        assert t3.utc_month == dataclasses.I3Time.Jul

        assert t1.utc_nano_sec == nanosec
        assert t3.utc_nano_sec == nanosec
        
        #July 1 is the 181th day of the year on standard years and 182th on leap years
        assert t1.utc_sec == (181+ int(year%4==0))*86400 - 1
        assert t3.utc_sec == (181+ int(year%4==0))*86400 

        assert t1.utc_year==year
        assert t3.utc_year==year 
           
        #test that it prints out the leap second correctly
        assert t1.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-06-30 23:59:59"%t1.utc_year
        assert t3.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-07-01 00:00:00"%t1.utc_year

        #test arithmatic 
        assert(t1+1*I3Units.second==t3)
        assert(t3-1*I3Units.second==t1)
        
        assert(t3-t1== 1*I3Units.second)
        assert(t1-t3==-1*I3Units.second)


    #do everyting again with year transition instead of June July transision
                        
    t1.set_utc_cal_date(year  ,12,31,12, 0, 0,0)
    t2.set_utc_cal_date(year+1, 1, 1,12, 0, 0,0)

    leap_sec = t2.mod_julian_day in leap_sec_mjd

    assert (t1-t2)/I3Units.second+86400 == -leap_sec
    assert (t2-t1)/I3Units.second-86400 ==  leap_sec

    if leap_sec:        
        assert(t1+86401*I3Units.second==t2)
        assert(t2-86401*I3Units.second==t1)
    else:
        assert(t1+86400*I3Units.second==t2)
        assert(t2-86400*I3Units.second==t1)

    #re-initilize t1 and t2
    t1=dataclasses.I3Time()
    t2=dataclasses.I3Time()
    t3=dataclasses.I3Time()

    if leap_sec:

        #set to leap second itsself
        nanosec = random.randint(0,1e9)
        t1.set_utc_cal_date(year  ,12,31,23,59,59,nanosec)
        t2.set_utc_cal_date(year  ,12,31,23,59,60,nanosec)
        t3.set_utc_cal_date(year+1, 1, 1, 0, 0, 0,nanosec)          

        assert not t1.is_leap_second
        assert t2.is_leap_second
        assert not t3.is_leap_second

        # Python's date time doesn't handle leap seconds,
        # so it sets the time to 23:59:59.999999

        dt1=t1.date_time
        dt2=t2.date_time
        dt3=t3.date_time
        
        assert dt1                          == datetime.datetime(year  ,12,31,23,59,59,int(nanosec/1000.))
        assert dt1+datetime.timedelta(0, 1) == datetime.datetime(year+1, 1, 1, 0, 0, 0,int(nanosec/1000.))
        assert dt2                          == datetime.datetime(year  ,12,31,23,59,59,999999)
        assert dt3                          == datetime.datetime(year+1, 1, 1, 0, 0, 0,int(nanosec/1000.))
        assert dt3+datetime.timedelta(0,-1) == datetime.datetime(year  ,12,31,23,59,59,int(nanosec/1000.))

        #although the invalid python leap second object return true when compared to eachother
        assert (t1+1*I3Units.second).date_time == dt2
        assert (t1+2*I3Units.second).date_time == dt3
        assert (t2+1*I3Units.second).date_time == dt3
        assert (t2-1*I3Units.second).date_time == dt1
        assert (t3-1*I3Units.second).date_time == dt2
        assert (t3-2*I3Units.second).date_time == dt1

        assert t1.mod_julian_day    == t2.mod_julian_day 
        assert t1.mod_julian_day +1 == t3.mod_julian_day
        assert t1.mod_julian_day not in leap_sec_mjd
        assert t3.mod_julian_day in leap_sec_mjd

        assert int(t1.mod_julian_day_double) == t1.mod_julian_day
        assert int(t2.mod_julian_day_double) == t2.mod_julian_day
        assert int(t3.mod_julian_day_double) == t3.mod_julian_day

        assert approx_Equal(t1.mod_julian_day_double           , t2.mod_julian_day_double, 1e-10)
        assert approx_Equal(t1.mod_julian_day_double + 1/86400., t3.mod_julian_day_double, 1e-10)

        assert t1.mod_julian_nano_sec == nanosec
        assert t2.mod_julian_nano_sec == nanosec
        assert t2.mod_julian_nano_sec == nanosec

        assert t1.mod_julian_sec == 86399 
        assert t2.mod_julian_sec == 86400
        assert t3.mod_julian_sec ==     0

        assert t2.unix_time + 1 == t3.unix_time
        assert t1.unix_time + 1 == t3.unix_time
        assert t1.unix_time == (t3.mod_julian_day-40587)*86400 -1 
        assert t2.unix_time == (t3.mod_julian_day-40587)*86400 -1 
        assert t3.unix_time == (t3.mod_julian_day-40587)*86400 

        sec_in_year = ( 365 + int(year%4==0) ) * 86400  + int( t1.mod_julian_day-183 in leap_sec_mjd)        
        assert t2.utc_daq_time - t1.utc_daq_time == 1e10
        assert t3.utc_daq_time ==  nanosec*10 
        assert (t1 + 2e9).utc_daq_time ==  nanosec*10 
        assert (t2 + 1e9).utc_daq_time ==  nanosec*10 
        assert t1.utc_daq_time == ( sec_in_year -1 )*int(1e10) + nanosec*10 
        assert t2.utc_daq_time == ( sec_in_year    )*int(1e10) + nanosec*10 

        assert t1.utc_day_of_month == 31
        assert t2.utc_day_of_month == 31
        assert t3.utc_day_of_month ==  1


        assert t1.utc_month == dataclasses.I3Time.Dec
        assert t2.utc_month == dataclasses.I3Time.Dec
        assert t3.utc_month == dataclasses.I3Time.Jan

        assert t1.utc_nano_sec == nanosec
        assert t2.utc_nano_sec == nanosec
        assert t3.utc_nano_sec == nanosec
        

        assert t1.utc_sec == sec_in_year - 1
        assert t2.utc_sec == sec_in_year
        assert t3.utc_sec == 0

        assert t1.utc_year==year
        assert t2.utc_year==year
        assert t3.utc_year==year + 1
           
        #test that it prints out the leap second correctly
        assert t1.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-12-31 23:59:59"%t1.utc_year
        assert t2.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-12-31 23:59:60"%t1.utc_year
        assert t3.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-01-01 00:00:00"%t3.utc_year

        #test arithmatic 
        assert(t1+  I3Units.second==t2)
        assert(t2+  I3Units.second==t3)
        assert(t1+2*I3Units.second==t3)

        assert(t3-  I3Units.second==t2)
        assert(t2-  I3Units.second==t1)
        assert(t3-2*I3Units.second==t1)
        
        assert(t2-t1==  I3Units.second)
        assert(t3-t2==  I3Units.second)
        assert(t3-t1==2*I3Units.second)

        assert(t1-t2==  -I3Units.second)
        assert(t2-t3==  -I3Units.second)
        assert(t1-t3==-2*I3Units.second)
        

    else:
        nanosec = random.randint(0,1e9)
        t1.set_utc_cal_date(year  ,12,31,23,59,59,nanosec)
        t2.set_utc_cal_date(year  ,12,31,23,59,60,nanosec)
        t3.set_utc_cal_date(year+1, 1, 1, 0, 0, 0,nanosec)          

        assert t2==dataclasses.I3Time()
        assert not t1.is_leap_second
        assert not t3.is_leap_second

        dt1=t1.date_time
        dt3=t3.date_time
        
        assert dt1                          == datetime.datetime(year  ,12,31,23,59,59,int(nanosec/1000.))
        assert dt1+datetime.timedelta(0, 1) == datetime.datetime(year+1, 1, 1, 0, 0, 0,int(nanosec/1000.))
        assert dt3                          == datetime.datetime(year+1, 1, 1, 0, 0, 0,int(nanosec/1000.))
        assert dt3+datetime.timedelta(0,-1) == datetime.datetime(year  ,12,31,23,59,59,int(nanosec/1000.))

        #although the invalid python leap second object return true when compared to eachother
        assert (t1+1*I3Units.second).date_time == dt3
        assert (t3-1*I3Units.second).date_time == dt1

        assert t1.mod_julian_day +1 == t3.mod_julian_day
        assert t1.mod_julian_day not in leap_sec_mjd
        assert t3.mod_julian_day not in leap_sec_mjd

        assert int(t1.mod_julian_day_double) == t1.mod_julian_day
        assert int(t3.mod_julian_day_double) == t3.mod_julian_day

        assert approx_Equal(t1. mod_julian_day_double + 1/86400., t3.mod_julian_day_double,1e-10)

        assert t1.mod_julian_nano_sec == nanosec
        assert t3.mod_julian_nano_sec == nanosec

        assert t1.mod_julian_sec == 86399 
        assert t3.mod_julian_sec ==     0

        #as per ntp unix time of leap seconds is the next seconds's unix time
        assert t1.unix_time + 1 == t3.unix_time
        assert t1.unix_time == (t3.mod_julian_day-40587)*86400 -1 
        assert t3.unix_time == (t3.mod_julian_day-40587)*86400 

        sec_in_year = ( 365 + int(year%4==0) ) * 86400  + int( t1.mod_julian_day-183 in leap_sec_mjd)        
        assert t3.utc_daq_time ==  nanosec*10 
        assert (t1 + 1e9).utc_daq_time ==  nanosec*10 
        assert t1.utc_daq_time == ( sec_in_year -1 )*int(1e10) + nanosec*10 

        assert t1.utc_day_of_month == 31
        assert t3.utc_day_of_month ==  1


        assert t1.utc_month == dataclasses.I3Time.Dec
        assert t3.utc_month == dataclasses.I3Time.Jan

        assert t1.utc_nano_sec == nanosec
        assert t3.utc_nano_sec == nanosec
        

        assert t1.utc_sec == sec_in_year - 1
        assert t3.utc_sec == 0

        assert t1.utc_year==year
        assert t3.utc_year==year + 1
           
        #test that it prints out the leap second correctly
        assert t1.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-12-31 23:59:59"%t1.utc_year
        assert t3.get_utc_string("%Y-%m-%d %H:%M:%S")=="%4d-01-01 00:00:00"%t3.utc_year

        #test arithmatic 
        assert(t1+1*I3Units.second==t3)
        assert(t3-1*I3Units.second==t1)
        
        assert(t3-t1== 1*I3Units.second)
        assert(t1-t3==-1*I3Units.second)



#set the range of days to pick two years since the first and last leap second
d_start = leap_sec_mjd[ 0]-365*2
d_stop  = leap_sec_mjd[-1]+365*2

a=[]
b=[]

#do a lot of trials 
for i in range(10000):
    #pick two random days
    d1 = random.randint(d_start,d_stop)
    d2 = random.randint(d_start,d_stop)

    ns = random.randint(0,1e9)

    #make I3Times noon of that day
    t1.set_mod_julian_time(d1,43200,ns)
    t2.set_mod_julian_time(d2,43200,ns)
    
    #calculate how many leap seconds between those days
    if d1> d2:
        leap_seconds =  sum([ x<=d1 and x>d2 for x in leap_sec_mjd])
    else:
        leap_seconds = -sum([ x>d1 and x<=d2 for x in leap_sec_mjd])

    #check that the delta is correct
    assert  int((t1-t2)/I3Units.second) == (d1-d2)*86400 + leap_seconds

    #check I3time + double is correct
    time_delta = ((d2-d1)*86400- leap_seconds)*I3Units.second
    assert(t1 + time_delta == t2)
    assert(t2 - time_delta == t1)


print("Leap second test successful, (you can ignore the errors about 'Invalid second: 60!')")
