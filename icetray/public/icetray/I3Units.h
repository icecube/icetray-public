#ifndef ICETRAY_I3UNITS_H_INCLUDED
#define ICETRAY_I3UNITS_H_INCLUDED

/**
 @brief A ripped Geant4 header declaring unit conventions

 I just wrapped it all in a namespace I3Units, Also changed the
 definition of Energy, so that GeV is the base units, rather than
 Joules

 Modified 2-17-04 by John Pretz from a Geant4 header
 @version $Id$
 @file I3Units.h  
 @date $Date$
 
 Also removed everything but Time, Energy, Length, Angle, Areas and Volumes.
 Pressure, Power, stuff like that is not done, since the redefinition of
 GeV = 1.0 messes with all that.
 
 Here's the original Geant4 header:
 
<b>
 DISCLAIMER

 The following disclaimer summarizes all the specific disclaimers 
 of contributors to this software. The specific disclaimers,which 
 govern, are listed with their locations in:
 http://cern.ch/geant4/license
 
 Neither the authors of this software system, nor their employing 
 institutes,nor the agencies providing financial support for this 
 work  make  any representation or  warranty, express or implied, 
 regarding  this  software system or assume any liability for its 
 use.

 This  code  implementation is the  intellectual property  of the 
 GEANT4 collaboration.
 By copying,  distributing  or modifying the Program (or any work 
 based  on  the Program)  you indicate  your  acceptance of  this 
 statement, and all its terms.
</b>
*/
/**
 * @namespace I3Units
 * @brief Unit definitions for the dataclasses
 *
 * The I3Units namespace contains a bunch of "static const double's" which
 * define the units that are used in the dataclasses.  The header originally
 * comes from Geant4, and wrapping it in and I3Units namespace is an IceCube
 * modification.
 * 
 * The basic units are those of the International System:<br>
 *                                                       <br>
 *              meter                                    <br>
 *              second                                   <br>
 *              kilogram                                 <br>
 *              ampere                                   <br>
 *              kelvin                                   <br>
 *              the amount of substance (mole)           <br>
 *              luminous intensity      (candela)        <br>
 *              radian                                   <br>
 *              steradian                                <br>
 *<br>
 *
 * The SI numerical value of the positron charge is defined here,
 * as it is needed for conversion factor : positron charge = e_SI (coulomb)
 *
 *
 *  This clearly needs more docs.
 *  In general, you want to "add units" when you store them in
 *      container, and "remove" them when you fetch them.  This 
 *      ensures that all data is stored uniformly.
 *    For example:
 *      double myvoltage = 45.00*I3Units::V;
 *      myContainerPtr->SetVoltage(myvoltage);
 *
 *      .....
 *      readvoltage = myContainerPtr->GetVoltage()/I3Units::V;
 *
 * @version $Version:$
 * @date $Date$
 * @author Geant4 Team (original) M.Maire, S.Giani
 * @author pretz (IceCube modifications)
 * @todo Should we have a 'speed' here too?
 * @todo add 'ergs'
 */

#include <cmath>

#if defined(__CINT__) && defined(__MAKECINT__)
#include <TMath.h>

#define M_PI TMath::Pi()
#define M_E  TMath::E()
#endif

namespace I3Units
{
  /**
   * meter
   */
  static const double meter  = 1.;

  /**
   * square meter
   */
  static const double meter2 = meter*meter;

  /**
   * cubic meter
   */
  static const double meter3 = meter*meter*meter;

  /**
   * millimeter
   */ 
  static const double millimeter  = 0.001*meter;

  /**
   * square millimeter
   */
  static const double millimeter2 = millimeter*millimeter;

  /**
   * cubic millimeter
   */
  static const double millimeter3 = millimeter*millimeter*millimeter;

  /**
   * centimeter
   */
  static const double centimeter  = 10.*millimeter;

  /**
   * square centimeter
   */
  static const double centimeter2 = centimeter*centimeter;

  /**
   * cubic centimeter
   */
  static const double centimeter3 = centimeter*centimeter*centimeter;

  /**
   * kilometer
   */
  static const double kilometer = 1000.*meter;

  /**
   * square kilometer
   */
  static const double kilometer2 = kilometer*kilometer;

  /**
   * cubic kilometer
   */
  static const double kilometer3 = kilometer*kilometer*kilometer;

  /**
   * parsec
   */
  static const double parsec = 3.0856775807e+16*meter;

  /**
   * micrometer
   */
  static const double micrometer = 1.e-6 *meter;

  /**
   * nanometer
   */
  static const double  nanometer = 1.e-9 *meter;

  /**
   * angstrom
   */
  static const double  angstrom  = 1.e-10*meter;

  /**
   * fermi
   */
  static const double fermi     = 1.e-15*meter;

  /**
   * barn
   */
  static const double barn = 1.e-28*meter2;

  /**
   * millibarn
   */
  static const double millibarn = 1.e-3 *barn;

  /**
   * microbarn
   */
  static const double microbarn = 1.e-6 *barn;

  /**
   * nanobarn
   */
  static const double nanobarn = 1.e-9 *barn;

  /**
   * picobarn
   */
  static const double picobarn = 1.e-12*barn;

  /**
   * feet
   */
  static const double feet = 0.3048*meter;

  // symbols
  /**
   * millimeter
   */
  static const double mm  = millimeter;

  /**
   * square millimeter
   */
  static const double mm2 = millimeter2;

  /**
   * cubic millimeter
   */
  static const double mm3 = millimeter3;

  /**
   * centimeter
   */
  static const double cm  = centimeter;

  /**
   * square centimeter
   */
  static const double cm2 = centimeter2;

  /**
   * cubic centimeter
   */
  static const double cm3 = centimeter3;

  /**
   * meter
   */
  static const double m  = meter;

  /**
   * square meter
   */
  static const double m2 = meter2;

  /**
   * cubic meter
   */
  static const double m3 = meter3;

  /**
   * kilometer
   */
  static const double km  = kilometer;

  /**
   * square kilometer
   */
  static const double km2 = kilometer2;

  /**
   * cubic kilometer
   */
  static const double km3 = kilometer3;

  /**
   * parsec
   */
  static const double pc = parsec;

  /**
   * feet
   */
  static const double ft = feet;

  //
  // Angle
  //
  /**
   * radian
   */
  static const double radian      = 1.;

  /**
   * milliradian
   */
  static const double milliradian = 1.e-3*radian;

  /**
   * degree
   */
  static const double degree = (M_PI/180.0)*radian;

  /**
   * steradian
   */
  static const double   steradian = 1.;

  // symbols
  /**
   * radian
   */
  static const double rad  = radian;

  /**
   * milliradian
   */
  static const double mrad = milliradian;

  /**
   * steradian
   */
  static const double sr   = steradian;

  /**
   * degree
   */
  static const double deg  = degree;

  //
  // Time [T]
  //
  /**
   * second
   */
  static const double second      = 1.e9;

  /**
   * millisecond
   */ 
  static const double millisecond = 1.e-3 *second;

  /**
   * microsecond
   */
  static const double microsecond = 1.e-6 *second;

  /**
   * nanosecond
   */
  static const double nanosecond  = 1.e-9 *second;

  /**
   * picosecond
   */
  static const double  picosecond = 1.e-12*second;


  /**
   * minute
   */
  static const double minute = 60*second;

  /**
   * hour
   */
  static const double hour = 60*minute;

  /**
   * day
   */
  static const double day = 24*hour;

  /**
   * Gregorian Year
   */
  static const double gregorianyear = 365.2425*day;

  /**
   * Julian Year
   */
  static const double julianyear = 365.25*day;

  /**
   * hertz
   */
  static const double hertz = 1./second;

  /**
   * kilohertz
   */
  static const double kilohertz = 1.e+3*hertz;

  /**
   * megahertz
   */
  static const double megahertz = 1.e+6*hertz;

  // symbols
  /**
   * nanosecond
   */
  static const double ns = nanosecond;

  /**
   * second
   */
  static const double  s = second;

  /**
   * millisecond
   */
  static const double ms = millisecond;

  //
  // Energy [E]
  //
  /**
   * eV
   */
  static const double     electronvolt = 1.0e-9;

  /**
   * keV
   */
  static const double kiloelectronvolt = 1.e+3*electronvolt;

  /**
   * MeV
   */
  static const double megaelectronvolt = 1.e+6*electronvolt; 

  /**
   * GeV
   */
  static const double gigaelectronvolt = 1.e+9*electronvolt;

  /**
   * TeV
   */
  static const double teraelectronvolt = 1.e+12*electronvolt;

  /**
   * PeV
   */
  static const double petaelectronvolt = 1.e+15*electronvolt;

  // symbols
  /**
   * MeV
   */
  static const double MeV = megaelectronvolt;

  /**
   * eV
   */
  static const double  eV = electronvolt;

  /**
   * keV
   */
  static const double keV = kiloelectronvolt;

  /**
   * GeV
   */
  static const double GeV = gigaelectronvolt;

  /**
   * TeV
   */
  static const double TeV = teraelectronvolt;

  /**
   * PeV
   */
  static const double PeV = petaelectronvolt;

  //
  // Charge
  //
  /**
   * Charge
   */
   static const double eplus = 1.;               // positron charge
   static const double eSI   = 1.602176462e-19;  // positron charge in coulomb

  /**
   * Coulomb
   */ 
   static const double coulomb = eplus/eSI;      // coulomb = 6.24150e18*eplus 
   static const double C = coulomb;

  /**
   * Picocoulomb
   */
   static const double picocoulomb = 1.e-12*coulomb;
   static const double pC = picocoulomb;


  //
  // Energy in joule
  //
  /**
   * joule
   */
  static const double joule = electronvolt/eSI;  // joule = 6.24150 e+12 * MeV;

  //
  // Current
  //
  /*
   * Ampere
   */
   static const double ampere = coulomb/second;  // ampere = 6.24150 e+9 * eplus/ns
   static const double A = ampere;

  /*
   * Milliampere
   */
   static const double milliampere = 1.e-3*ampere;
   static const double mA = milliampere;

  /*
   * Nanoampere
   */
   static const double nanoampere = 1.e-9*ampere;
   static const double nA = nanoampere;

  //
  // Voltage
  //

  /**
   * Millivolt
   */
  static const double millivolt = (electronvolt*1.e-3)/eplus;
  static const double mV = millivolt;

  /**
   * Volt
   */
  static const double volt = 1.e3 * millivolt;
  static const double V = volt;

  /**
   * Microvolt
   */
  static const double microvolt = 1.e-6 * volt;

  /**
   * Kilovolt
   */
  static const double kilovolt = 1.e3 * volt;
  static const double kV = kilovolt;

  /**
   * Voltage in cgs is the statvolt.
   * [statV] = c_light (in m/s) / 10^6 [Volt]
   */
  static const double statvolt = 299792458. / 1.e6 * volt;

  //
  // Power
  // 

  /**
   * Watt
   */  
  static const double watt = joule / second;

  /**
   * Milliwatt
   */ 
  static const double milliwatt = 1.e-3 * watt;
  static const double mW = milliwatt;

  //
  // Resistance
  //
  /**
   * Ohm
   */
   static const double ohm = volt/ampere; 

    //
    // Temperature
    //
    /**
     * Kelvin
     */
    static const double kelvin = 1.0;

    //
    // Mass [E][T^2][L^-2]
    //
    /**
     * Kilogram
     */
    static const double kilogram = joule*second*second/(meter*meter);
    static const double kg = kilogram;

    /**
     * Gram
     */
    static const double gram = 1.e-3*kilogram;
    static const double g = gram;

    /**
     * Milligram
     */
    static const double milligram = 1.e-3*gram;
    static const double mg = milligram;

    /**
     * Force
     */
    static const double newton = joule / meter;

    /**
     * Pressure
     */
    static const double pascal = newton / (meter * meter);
    static const double bar = 1.e5 * pascal;
    static const double millipascal = 1.e-3 * pascal;
    static const double micropascal = 1.e-6 * pascal;
    static const double nanopascal = 1.e-9 * pascal;

  /**
   * Magnetic Field Strength
   */

  static const double tesla = volt * second / m2;
  static const double Tesla = tesla;

  static const double millitesla = 1.e-3 * tesla;
  static const double mT = 1.e-3 * tesla;
  static const double microtesla = 1.e-6 * tesla;
  static const double uT = 1.e-6 * tesla;
  static const double nanotesla = 1.e-9 * tesla;
  static const double nT = 1.e-9 * tesla;


  //
  // Miscellaneous
  //
  /**
   * percent
   */
  static const double perCent     = 0.01 ;

  /**
   * perThousand
   */
  static const double perThousand = 0.001;

  /**
   * perMillion
   */
  static const double perMillion  = 0.000001;

};

#endif

