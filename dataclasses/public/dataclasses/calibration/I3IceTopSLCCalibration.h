// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

/**
 *
 * @version $Id$
 * @file I3IceTopSLCCalibration.h
 * @date $Date$
 */

#ifndef I3ICETOPSLCCALIBRATION_H_INCLUDED
#define I3ICETOPSLCCALIBRATION_H_INCLUDED

#include <icetray/OMKey.h>
#include <icetray/I3FrameObject.h>
#include <dataclasses/Utility.h>
#include <dataclasses/external/CompareFloatingPoint.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/I3Time.h>
/**
 * @brief This struct stores the slopes and intercepts needed for charge calibration
 * of SLC hits in IceTop DOMs.
 * @author Kath Rawlins
 */

// First, we'll define the individual items, that will go into a Map,
// which will then go into a frame-object container:
class I3IceTopSLCCalibration
{
public:
    /**
     * Slopes and Intercepts for each Chip/ATWD
     */
    double intercept_C0A0;
    double intercept_C0A1;
    double intercept_C0A2;
    double intercept_C1A0;
    double intercept_C1A1;
    double intercept_C1A2;
    double slope_C0A0;
    double slope_C0A1;
    double slope_C0A2;
    double slope_C1A0;
    double slope_C1A1;
    double slope_C1A2;

    /**
     * Slopes and Intercepts to be used when the Chip/ATWD is unknown
     */
    double intercept_CunkA0;
    double intercept_CunkA1;
    double intercept_CunkA2;
    double slope_CunkA0;
    double slope_CunkA1;
    double slope_CunkA2;
    double A0_A1_crossover;
    double A1_A2_crossover;

    I3IceTopSLCCalibration()
    {
        // Initialize them all to NaN
        intercept_C0A0 = NAN;
        intercept_C0A1 = NAN;
        intercept_C0A2 = NAN;
        intercept_C1A0 = NAN;
        intercept_C1A1 = NAN;
        intercept_C1A2 = NAN;
        slope_C0A0 = NAN;
        slope_C0A1 = NAN;
        slope_C0A2 = NAN;
        slope_C1A0 = NAN;
        slope_C1A1 = NAN;
        slope_C1A2 = NAN;
        intercept_CunkA0 = NAN;
        intercept_CunkA1 = NAN;
        intercept_CunkA2 = NAN;
        slope_CunkA0 = NAN;
        slope_CunkA1 = NAN;
        slope_CunkA2 = NAN;
        A0_A1_crossover = NAN;
        A1_A2_crossover = NAN;
    }
    ~I3IceTopSLCCalibration();

    double GetIntercept(int chip, unsigned int atwd) const {
        int c = chip*10+atwd;
        if (c < 0) c += 30;  // Deal with chip = -1, which is "unknown chip number"
        switch(c) {
            case 0:
                return intercept_C0A0; break;
            case 1:
                return intercept_C0A1; break;
            case 2:
                return intercept_C0A2; break;
            case 10:
                return intercept_C1A0; break;
            case 11:
                return intercept_C1A1; break;
            case 12:
                return intercept_C1A2; break;
            case 20:
                return intercept_CunkA0; break;
            case 21:
                return intercept_CunkA1; break;
            case 22:
                return intercept_CunkA2; break;
            default:
                log_fatal("Unknown chip/atwd combination %d/%d", chip, atwd);
        }
    }

    double GetSlope(int chip, unsigned int atwd) const {
        int c = chip*10+atwd;
        if (c < 0) c += 30;  // Deal with chip = -1, which is "unknown chip number"
        switch(c) {
            case 0:
                return slope_C0A0; break;
            case 1:
                return slope_C0A1; break;
            case 2:
                return slope_C0A2; break;
            case 10:
                return slope_C1A0; break;
            case 11:
                return slope_C1A1; break;
            case 12:
                return slope_C1A2; break;
            case 20:
                return slope_CunkA0; break;
            case 21:
                return slope_CunkA1; break;
            case 22:
                return slope_CunkA2; break;
            default:
                log_fatal("Unknown chip/atwd combination %d/%d", chip, atwd);
        }
    }

    void SetIntercept(int chip, unsigned int atwd, double val) {
        int c = chip*10+atwd;
        if (c < 0) c += 30;  // Deal with chip = -1, which is "unknown chip number"
        switch(c) {
            case 0:
                intercept_C0A0 = val; break;
            case 1:
                intercept_C0A1 = val; break;
            case 2:
                intercept_C0A2 = val; break;
            case 10:
                intercept_C1A0 = val; break;
            case 11:
                intercept_C1A1 = val; break;
            case 12:
                intercept_C1A2 = val; break;
            case 20:
                intercept_CunkA0 = val; break;
            case 21:
                intercept_CunkA1 = val; break;
            case 22:
                intercept_CunkA2 = val; break;
            default:
                log_fatal("Unknown chip/atwd combination %d/%d", chip, atwd);
        }
    }

    void SetSlope(int chip, unsigned int atwd, double val) {
        int c = chip*10+atwd;
        if (c < 0) c += 30;  // Deal with chip = -1, which is "unknown chip number"
        switch(c) {
            case 0:
                slope_C0A0 = val; break;
            case 1:
                slope_C0A1 = val; break;
            case 2:
                slope_C0A2 = val; break;
            case 10:
                slope_C1A0 = val; break;
            case 11:
                slope_C1A1 = val; break;
            case 12:
                slope_C1A2 = val; break;
            case 20:
                slope_CunkA0 = val; break;
            case 21:
                slope_CunkA1 = val; break;
            case 22:
                slope_CunkA2 = val; break;
            default:
                log_fatal("Unknown chip/atwd combination %d/%d", chip, atwd);
        }
    }

    bool operator==(const I3IceTopSLCCalibration& rhs) const
    {
        using CompareFloatingPoint::Compare;
        return (Compare(intercept_C0A0, rhs.intercept_C0A0) &&
                Compare(intercept_C0A1, rhs.intercept_C0A1) &&
                Compare(intercept_C0A2, rhs.intercept_C0A2) &&
                Compare(intercept_C1A0, rhs.intercept_C1A0) &&
                Compare(intercept_C1A1, rhs.intercept_C1A1) &&
                Compare(intercept_C1A2, rhs.intercept_C1A2) &&
                Compare(slope_C0A0, rhs.slope_C0A0) &&
                Compare(slope_C0A1, rhs.slope_C0A1) &&
                Compare(slope_C0A2, rhs.slope_C0A2) &&
                Compare(slope_C1A0, rhs.slope_C1A0) &&
                Compare(slope_C1A1, rhs.slope_C1A1) &&
                Compare(slope_C1A2, rhs.slope_C1A2) &&
                Compare(intercept_CunkA0, rhs.intercept_CunkA0) &&
                Compare(intercept_CunkA1, rhs.intercept_CunkA1) &&
                Compare(intercept_CunkA2, rhs.intercept_CunkA2) &&
                Compare(slope_CunkA0, rhs.slope_CunkA0) &&
                Compare(slope_CunkA1, rhs.slope_CunkA1) &&
                Compare(slope_CunkA2, rhs.slope_CunkA2) &&
                Compare(A0_A1_crossover, rhs.A0_A1_crossover) &&
                Compare(A1_A2_crossover, rhs.A1_A2_crossover));

    }
    bool operator!=(const I3IceTopSLCCalibration& rhs) const
    {
        return !operator==(rhs);
    }

private:
    friend class icecube::serialization::access;
    template <class Archive> void serialize(Archive & ar, unsigned version);

};

// ...and its pointers.
I3_POINTER_TYPEDEFS(I3IceTopSLCCalibration);

// Now define the Map of these things, and its pointers.
typedef I3Map<OMKey, I3IceTopSLCCalibration> I3IceTopSLCCalibrationMap;
I3_POINTER_TYPEDEFS(I3IceTopSLCCalibrationMap);

// Now define the container (I3FrameObject) which will actually go in the frame.
// It contains the MapOfTheThings, plus other (small) associated objects, like StartTime/EndTime, etc.
// (This bit is basically copied and adapted from I3Calibration.h)
static const unsigned i3itslccalibration_version_ = 0;

class I3IceTopSLCCalibrationCollection : public I3FrameObject
{
public:
    I3Time startTime; ///< Start time of period used to compute constants (may be different from the rest of I3Calibration)
    I3Time endTime;   ///< End time of period used to compute constants (may be different from the rest of I3Calibration)

    I3IceTopSLCCalibrationCollection();

    ~I3IceTopSLCCalibrationCollection();

    I3IceTopSLCCalibrationMap ITslcCal;  ///< The most important object in here: the Map of all the calibration constants

    bool operator==(const I3IceTopSLCCalibrationCollection& rhs)
    {
      return (startTime == rhs.startTime &&
              endTime == rhs.endTime &&
              ITslcCal == rhs.ITslcCal);
    }
    bool operator!=(const I3IceTopSLCCalibrationCollection& rhs)
    {
      return !operator==(rhs);
    }

private:
    friend class icecube::serialization::access;
    template <class Archive> void load(Archive & ar, unsigned version);
    template <class Archive> void save(Archive & ar, unsigned version) const;
    I3_SERIALIZATION_SPLIT_MEMBER();
};

I3_CLASS_VERSION(I3IceTopSLCCalibrationCollection, i3itslccalibration_version_);
I3_POINTER_TYPEDEFS(I3IceTopSLCCalibrationCollection);

std::ostream& operator<<(std::ostream& oss, const I3IceTopSLCCalibration& c);

#endif //I3ICETOPSLCCALIBRATION_H_INCLUDED

