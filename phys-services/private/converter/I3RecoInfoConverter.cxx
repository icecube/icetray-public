/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 * copied from Sebastian Panknin's I3RecoInfoToTable in hdf-writer
 */

#include "I3RecoInfoConverter.h"

#include "phys-services/I3ScaleCalculator.h"
#include "phys-services/I3Cuts.h"

#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/physics/I3RecoPulse.h>

#include <boost/foreach.hpp>

/******************************************************************************/

I3RecoInfoConverter::I3RecoInfoConverter(std::string pulseMapName) :
    I3ConverterImplementation<I3Particle>(),
    pulseMapName_(pulseMapName),
    icecubeConf_(I3ScaleCalculator::IC_GUESS),
    icetopConf_(I3ScaleCalculator::IT_GUESS),
    timeWindows_(),
    muonTimeWindows_()
{
  defineTimeWindows();
}

I3RecoInfoConverter::I3RecoInfoConverter(std::string pulseMapName, 
                                         int icecubeConf, 
                                         int icetopConf) :
    I3ConverterImplementation<I3Particle>(),
    pulseMapName_(pulseMapName),
    icecubeConf_(icecubeConf),
    icetopConf_(icetopConf),
    timeWindows_(),
    muonTimeWindows_()
{
  defineTimeWindows();
}

/******************************************************************************/

I3RecoInfoConverter::I3RecoInfoConverter() :
    I3ConverterImplementation<I3Particle>() {
    log_fatal("%s: this constructor should not be callable", __PRETTY_FUNCTION__);
}

/******************************************************************************/
    
std::string I3RecoInfoConverter::generateDocString(std::string prefix, 
                                                   std::string identifier,
                                                   bool muon) {
    std::ostringstream result;
    if (muon) {
        const std::pair<double, double>& interval = 
           muonTimeWindows_[identifier];
        result << prefix << " (" << interval.first << ", " << interval.second << ")";
    }
    else {
        const std::pair<double, double>& interval = 
           timeWindows_[identifier];
        result << prefix << " (" << interval.first << ", " << interval.second << ")";
    }
    result << " from " << pulseMapName_;
    return result.str();
}

/******************************************************************************/

void I3RecoInfoConverter::defineTimeWindows() {
  timeWindows_["A"] = std::pair<double, double >( -15, +15); 
  timeWindows_["B"] = std::pair<double, double >( -15, +25); 
  timeWindows_["C"] = std::pair<double, double >( -15, +75); 
  timeWindows_["D"] = std::pair<double, double >( -15, +150);
  timeWindows_["E"] = std::pair<double, double >( -15, +250);
  muonTimeWindows_["early"] = std::pair<double, double >( -10000, -15);
  muonTimeWindows_["late"]  = std::pair<double, double >( +250, 10000);   
}

/******************************************************************************/

I3TableRowDescriptionPtr I3RecoInfoConverter::CreateDescription(const I3Particle& reco) {
    I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription() );
    
    desc->AddField<int32_t>("ndirA", "", generateDocString("number of direct hits in timeinterval A", "A", false) );
    desc->AddField<int32_t>("ndirB", "", generateDocString("number of direct hits in timeinterval B", "B", false) );
    desc->AddField<int32_t>("ndirC", "", generateDocString("number of direct hits in timeinterval C", "C", false) );
    desc->AddField<int32_t>("ndirD", "", generateDocString("number of direct hits in timeinterval D", "D", false) );
    desc->AddField<int32_t>("ndirE", "", generateDocString("number of direct hits in timeinterval E", "E", false) );
    
    desc->AddField<double>("ldirA", "", generateDocString("length (projected along the track) between hits in timeinterval A", "A", false));
    desc->AddField<double>("ldirB", "", generateDocString("length (projected along the track) between hits in timeinterval B", "B", false));
    desc->AddField<double>("ldirC", "", generateDocString("length (projected along the track) between hits in timeinterval C", "C", false));
    desc->AddField<double>("ldirD", "", generateDocString("length (projected along the track) between hits in timeinterval D", "D", false));
    desc->AddField<double>("ldirE", "", generateDocString("length (projected along the track) between hits in timeinterval E", "E", false));

    desc->AddField<int32_t>("nearly", "", generateDocString("number of early hits for muons in timeinterval", "early", true));
    desc->AddField<int32_t>("nlate", "", generateDocString("number of late hits for muons in timeinterval", "late", true));

    desc->AddField<double>("icecube_scale", "", "IceCube scale parameter");
    desc->AddField<double>("icetop_scale", "", "IceTop scale parameter");
    desc->AddField<int8_t>("contained", "", "Is the track contained?");
    
    return desc;
}

/******************************************************************************/

size_t I3RecoInfoConverter::FillRows(const I3Particle& reco, I3TableRowPtr rows) {
     
    // retrieve detector geometry
    // bail-out if not available
    I3GeometryConstPtr geometry = currentFrame_->Get<I3GeometryConstPtr>();
    if (!geometry) {
        log_fatal("%s: couldn't find detector geometry in current frame!",
                  __PRETTY_FUNCTION__);
    }
    
    I3RecoPulseSeriesMapConstPtr pulsemap = 
        currentFrame_->Get<I3RecoPulseSeriesMapConstPtr>(pulseMapName_);

    if (!pulsemap) {
        log_warn("%s: couldn't find pulsemap with name %s in current frame! NDir will be 0 and LDir will be NAN.",
                 __PRETTY_FUNCTION__, pulseMapName_.c_str());
    }

    // variables we want to calculate
    std::map<std::string, int> nDirs;
    std::map<std::string, double> lDirs;

    int nEarly=0; 
    int nLate=0;
  
    // need a typedef for the boost macro
    typedef std::pair<std::string, std::pair<double, double > > TimeWindowPair;

    // call different functions for cascades and muons
    if (pulsemap) {
      if (reco.IsCascade()){
        // cascades ------------------------------------------------------
        BOOST_FOREACH (TimeWindowPair win, timeWindows_) {
            int nDir;
            int nChan;  // not used (for output)
            int nHit;   // not used
            int n1hit;  // not used
            int nString;  // not used
            // do this calculation for all time windows
            // (this is not worse than calling NDir etc, 
            // because this only calls CutsCalc)
            I3Cuts::CascadeCutsCalc(reco, *geometry, *pulsemap, // input
                    win.second.first,     // timewindows
                    win.second.second,    
                    nChan, nHit, n1hit, nString, // unused buffers
                    nDir, nEarly, nLate); // intersting stuff

            nDirs[win.first] = nDir;
            lDirs[win.first] = 0;  // no ldir for cascades
        }
      } else {
        // track ---------------------------------------------------------
        int nChan;  // not used (for output)
        int nHit;   // not used
        int nString;  // not used
        double sDir; // not used
        double sAll; // not used
        double lDir; // also needed as a buffer

        BOOST_FOREACH (TimeWindowPair win, timeWindows_) {
            int nDir;
            // see comment above for efficiency
            I3Cuts::CutsCalc(reco, *geometry, *pulsemap,
                    win.second.first,
                    win.second.second,
                    nChan, nHit, nString,
                    nDir, lDir,
                    sDir, sAll); // not used yet

            nDirs[win.first] = nDir;
            lDirs[win.first] = lDir;
        }
        const std::pair<double, double>& earlyWindow = 
            muonTimeWindows_["early"];
        const std::pair<double, double>& lateWindow = 
            muonTimeWindows_["late"];
        // separatly calculate nEarly/nLate for muons
        I3Cuts::CutsCalc(reco, *geometry, *pulsemap,
                earlyWindow.first,
                earlyWindow.second,
                nChan, nHit, nString,
                nEarly,             // <- calculate this
                lDir, sDir, sAll);

        I3Cuts::CutsCalc(reco, *geometry, *pulsemap,
                lateWindow.first,
                lateWindow.second,
                nChan, nHit, nString,
                nLate,             // <- calculate this
                lDir, sDir, sAll);

      }
    } else {
      nDirs["A"] = nDirs["B"] = nDirs["C"] = nDirs["D"] = nDirs["E"] = 0;
      lDirs["A"] = lDirs["B"] = lDirs["C"] = lDirs["D"] = lDirs["E"] = NAN;
    }

    // init scaleCalculator
    I3ScaleCalculator scaleCalc(geometry,
            static_cast<I3ScaleCalculator::IceCubeConfig > 
            (icecubeConf_),
            static_cast<I3ScaleCalculator::IceTopConfig >
            (icetopConf_));

    rows->Set<double>("icecube_scale", scaleCalc.ScaleInIce (reco) );
    if (reco.IsTrack())
        rows->Set<double>("icetop_scale", scaleCalc.ScaleIceTop (reco) );
    else
        rows->Set<double>("icetop_scale", -1 );

    rows->Set<int8_t>("contained", scaleCalc.VertexIsInside (reco) ? -1 : 0);
    
    rows->Set<int32_t>("ndirA", nDirs["A"]);
    rows->Set<int32_t>("ndirB", nDirs["B"]);
    rows->Set<int32_t>("ndirC", nDirs["C"]);
    rows->Set<int32_t>("ndirD", nDirs["D"]);
    rows->Set<int32_t>("ndirE", nDirs["E"]);
    
    rows->Set<double>("ldirA", lDirs["A"]);
    rows->Set<double>("ldirB", lDirs["B"]);
    rows->Set<double>("ldirC", lDirs["C"]);
    rows->Set<double>("ldirD", lDirs["D"]);
    rows->Set<double>("ldirE", lDirs["E"]);

    rows->Set<int32_t>("nearly", nEarly);
    rows->Set<int32_t>("nlate" , nLate);
  
    return 1;

}

/******************************************************************************/
