#   copyright  (c) 2004, 2005
#   the icecube collaboration
#   $Id: example.py 8253 2005-05-27 18:03:30Z troy $
#
#   @version $Revision: 1.1 $
#   @date $Date: 2005-05-27 14:03:30 -0400 (Fri, 27 May 2005) $
#   @author John Pretz
#
#   Very simple.  Just defining global names which correspond to the
#   units as ithon exported them.  The idea is that icetray scripts
#   can just use import I3Units print I3Units.meter.
#   
#   The syntax is very similar to the familiar root scripts,e.g.
#   @code
#   import I3Units
#   4 * I3Units.meter
#   @endcode

import icecube.icetray as icetray

meter = icetray.meter(1)
meter2 = icetray.meter2(1)
meter3 = icetray.meter3(1)
millimeter = icetray.millimeter(1)
millimeter2 = icetray.millimeter2(1)
millimeter3 = icetray.millimeter3(1)
centimeter = icetray.centimeter(1)
centimeter2 = icetray.centimeter2(1)
centimeter3 = icetray.centimeter3(1)
kilometer = icetray.kilometer(1)
kilometer2 = icetray.kilometer2(1)
kilometer3 = icetray.kilometer3(1)
parsec = icetray.parsec(1)
micrometer = icetray.micrometer(1)
angstrom = icetray.angstrom(1)
fermi = icetray.fermi(1)
barn = icetray.barn(1)
millibarn = icetray.millibarn(1)
microbarn = icetray.microbarn(1)
nanobarn = icetray.nanobarn(1)
picobarn = icetray.picobarn(1)
feet = icetray.feet(1)
mm = icetray.mm(1)
mm2 = icetray.mm2(1)
mm3 = icetray.mm3(1)
cm = icetray.cm(1)
cm2 = icetray.cm2(1)
cm3 = icetray.cm3(1)
m = icetray.m(1)
m2 = icetray.m2(1)
m3 = icetray.m3(1)
km = icetray.km(1)
km2 = icetray.km2(1)
km3 = icetray.km3(1)
pc = icetray.pc(1)
ft = icetray.ft(1)
radian = icetray.radian(1)
milliradian = icetray.milliradian(1)
degree = icetray.degree(1)
steradian = icetray.steradian(1)
rad = icetray.rad(1)
mrad = icetray.mrad(1)
sr = icetray.sr(1)
deg = icetray.deg(1)
second = icetray.second(1)
nanosecond = icetray.nanosecond(1)
millisecond = icetray.millisecond(1)
microsecond = icetray.microsecond(1)
hertz = icetray.hertz(1)
kilohertz = icetray.kilohertz(1)
megahertz = icetray.megahertz(1)
ns = icetray.ns(1)
s = icetray.s(1)
ms = icetray.ms(1)
electronvolt = icetray.electronvolt(1)
kiloelectronvolt = icetray.kiloelectronvolt(1)
megaelectronvolt = icetray.megaelectronvolt(1)
gigaelectronvolt = icetray.gigaelectronvolt(1)
teraelectronvolt = icetray.teraelectronvolt(1)
petaelectronvolt = icetray.petaelectronvolt(1)
MeV = icetray.MeV(1)
eV = icetray.eV(1)
keV = icetray.keV(1)
GeV = icetray.GeV(1)
TeV = icetray.TeV(1)
PeV = icetray.PeV(1)
eplus = icetray.eplus(1)
eSI = icetray.eSI(1)
coulomb = icetray.coulomb(1)
C = icetray.C(1)
picocoulomb = icetray.picocoulomb(1)
pC = icetray.pC(1)
ampere = icetray.ampere(1)
A = icetray.A(1)
milliampere = icetray.milliampere(1)
mA = icetray.mA(1)
nanoampere = icetray.nanoampere(1)
nA = icetray.nA(1)
millivolt = icetray.millivolt(1)
mV = icetray.mV(1)
volt = icetray.volt(1)
V = icetray.V(1)
kilovolt = icetray.kilovolt(1)
kV = icetray.kV(1)
microvolt = icetray.microvolt(1)
ohm = icetray.ohm(1)
perCent = icetray.perCent(1)
perThousand = icetray.perThousand(1)
perMillion = icetray.perMillion(1)


