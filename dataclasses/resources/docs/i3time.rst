===========
I3Time
===========
There are two types of time in an event and I distinguish them by their C++ type: I3Time and double.

This is a class used to denote the absolute time to the tenth of a ns. It's instructive to look at the constructor. To create an object of this type you need to specify the year and the number of tenths of nanoseconds since the beginning of the year, otherwise known as DAQTime.

This is used in only two places in simulation: DrivingTime and EventHeader.

* DrivingTime is set at the **beginning** of the event and is passed to the geometry, calibration, and detector status services. This sets the detector configuration.
* EventHeader is set at the **end** of the event by the trigger simulation. The event header consists mainly of a StartTime and an EndTime (both of type I3Time).
  * EventHeaderStartTime=DrivingTime + timeofEarliestDOMLaunch
  * EventHeaderEndTime = DrivingTime + timeofLatestDOMLaunch

double
======

Every simulation module other than trigger sim **only** deals with times of type double and carry I3Units of time. The origin (t_0 = 0 ns) is determined by the earliest primary is set by the generator. Each subsequent time is taken with respect to the earliest primary.

Typical Simulation Chain
========================

* Event Service- DrivingTime (DT) is set
* DetectorStatusService - satatus served up depending on DT
* CalibrationService - calibration served up dependign on DT
* GeometryService - geometry served up depending on DT
* Muxer - Controls the combining of the above streams
* Generator - Injects at least one primary and sets time (t_0) to 0 ns.
* Propagator - Generates secondaries and sets times relative to t_0.
* HitConstructor - Generates I3MCHits and sets relative to t_0.
* PMTSim - Generates PMT waveforms whose start and stop times are relative to t_0.
* DOMSim - Generates DOM launches whose launch times are relative to t_0.
* TriggerSim - Based on the DOM launch series map triggers events and determines the EventHeader. Let's say the first DOMLaunch occurs at t_1 (>t_0) and the last DOMLaunch occurs at t_2 ( >t_1>t_0). The EventHeader will have a time range of (DT + t_1, DT + t_2)

The Problem
===========

The statement of the goal is very simple: make simulation look like data. With the setup outlined above the DOM launch times are relative to t_0 (which is coincident with DT). In data the DOM launch times are relative to EventHeaderStartTime.

* Option 1 : Set EventHeaderStarTime to DT.
  * Pros
    * The DOM launch times are now relative to EventHeaderStartTime and the **interpretation** is consistent with data.
    * There's a single point of modification, which is **very** safe and clean.
  * Cons
    * The DOM launches will now appear to launch long after the trigger time(~t_1 in the above example).
* Option 2 : Correct the DOM Launch times
  * Pros
    * The DOM launch times are consitent in interpretation and measurement as compared to data.
    * There are only two maps to worry about. InIce and IceTop.
  * Cons
    * The DOM launch times are shifted with respect to the low level MC objects that created them
* Option 3 : Correct **every** time in the event
  * Pros
    * Everyone is happy
  * Cons
    * This could be **very** difficult to do in practice.

The Solution
============

Check out I3TimeShifter in trigger-sim. There is an attempt at option #3. It calculates the time shift from a given trigger hierarchy ( choosing the earliest time) and shifts all **known** times with respect to the one time. 



Examples
========
Example use of I3Time
::
   >>> /**
   >>>  *@brief creates the object with the given times as the DAQ time
   >>>  */
   >>> I3Time(int32_t year, int64_t daqTime);
   




