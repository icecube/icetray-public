Using I3Units
==============

In general, you want to "add units" when you store them in
container, and "remove" them when you fetch them.  This
ensures that all data is stored uniformly.
For example::

 double myvoltage = 45.00*I3Units::V;
 myContainerPtr->SetVoltage(myvoltage);

And later::

 readvoltage = myContainerPtr->GetVoltage()/I3Units::V;

Alternatively, if you prefer mV::

 readvoltage = myContainerPtr->GetVoltage()/I3Units::mV;

Note, however, that there is no real need to divide out the unit when
you fetch the data. Doing so means you have made a choice of units
(for readvoltage in this case) which other people reading your code
need to be aware of. If instead you just do::

 readvoltage = myContainerPtr->GetVoltage();

you get a voltage with "dimension". You can then use it like, e.g.::

 if(readvoltage > 10*I3Units::mV) { ... };

while for the second way of access above you would have to write::

 if(readvoltage > 10) { ... };

and the reader of the code would have to remember that you chose mV for 
readvoltage higher up in the code.

The IceCube unit system
------------------------

In the IceTray software a set of "units" have been defined in the file
I3Units.h. These units live in namespace I3Units, for example
I3Units::GeV.  We have, for example::

 static const double eplus = 1.;		   // positron charge

 static const double     electronvolt = 1.0e-9;

 static const double gigaelectronvolt = 1.e+9*electronvolt;

 static const double  eV = electronvolt;

 static const double millivolt = (electronvolt*1.e-3)/eplus;

A few basic units (like eplus and electronvolt) are defined quite 
arbitrarily, and most units are calculated from these (as above).  It 
is important to realize that the I3Units are just numbers, although 
the idea is to USE them as real units.

A unit is a quantity with dimension (the original kilogram was for 
example the mass of a thing in Paris). You divide other quantities 
of the same dimension (mass, in the case of the kilogram) by the unit 
in order to obtain a ''dimensionless'' number specifying their 
value in that unit. In other words::

 quantity=number*unit

As long as we deal with quantities with dimension we do not need to
choose the unit. We can write, for example::

 distance = 10*I3Unit::m;

 time = 40*I3Unit::ns;

The quantities so constructed are '''not''' tied to any particular
units. We can divide distance by I3Units::m to get the value in
meters, or with I3Units::yd to get it in yards. So we can pass these
quantities around between modules without caring about units, and in
some other module we can set::

 speed = distance/time;

which will give us the speed, again without specific unit. It is only
when we want to communicate with some external code which expects
quantities expressed in (i.e. divided by) some specific unit that we
must divide by the appropriate unit, like::

 speed_in_metres_per_second = speed/(I3Units::m/I3Units::s);

Of course, if we want to print the speed in metres per second we must
also do the above.

Need for a convention!
^^^^^^^^^^^^^^^^^^^^^^^^

There is one case which can cause trouble: It is possible for the user 
to provide parameters for modules in a steering script either as 
just a number or as number*I3Unit.  This is dimensionally incorrect, and 
opens up for confusion if the particular I3Unit  does not equal one. It 
is therefore **essential** that a convention is adopted. All modules 
should assume that parameters with dimension include the unit, and all 
users should be aware of this.

To exploit the benefits of the I3Units we should also avoid converting
to specific units inside the code, and in particular when passing
parameters to functions and services. If we don't do this the I3Unit
approach only serves to complicate things!

Using in Python Scripts
------------------------

Using I3Units in python scripts to specify parameters is simple.  As
long as you have this line::

  from icecube.icetray import I3Units

then you can use I3Units like this::

  AzimuthRange = [-azimuth * I3Units.degree, azimuth * I3Units.degree]



