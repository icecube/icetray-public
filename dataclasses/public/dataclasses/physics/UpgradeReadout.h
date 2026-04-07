/**
 * Copyright  (C) 2026 the icecube collaboration
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef I3UPGRADEREADOUT_H_INCLUDED
#define I3UPGRADEREADOUT_H_INCLUDED

#include <ostream>
#include <vector>
#include <variant>

#include <dataclasses/geometry/I3OMGeo.h>
#include <dataclasses/physics/I3DEggLaunch.h>
#include <dataclasses/physics/I3mDOMLaunch.h>
#include <dataclasses/physics/I3XDOMHit.h>
#include <dataclasses/physics/detail/UpgradeLCFlags.h>
#include <icetray/OMKey.h>

// Forward-define the Series and SeriesMap classes
struct UpgradeReadout;
using UpgradeReadoutSeries = std::vector<UpgradeReadout>;
using UpgradeReadoutSeriesMap = std::map<OMKey, UpgradeReadoutSeries>;

//---------------------------
// The various Upgrade raw hit classes have similar interfaces
// and often need to be used together, but they don't share any
// inheritance. To work around this, we need to set up a generic
// interface that can be used for any of the classes. If we eventually
// build similar raw hit classes for FOMs or WOMs, they can be added
// here by ensuring they have similar interfaces and adding them
// to the UpgradeLaunch variant definition below.
//---------------------------
struct UpgradeReadout{
  /**
   * A raw hit class to generically wrap the type-specific classes
   * for DEggs and mDOMs. Here, we take an OMType (necessary for
   * some applications with I3XDOMHits, which are shared across
   * multiple module types) and the launch we want to wrap. The
   * type of launches we accept are given by the std::variant below.
   */
  using Readout = std::variant<I3DEggLaunch, I3mDOMLaunch, I3XDOMHit>;
  
  UpgradeReadout(Readout launch_, I3OMGeo::OMType type_){
    launch = launch_;
    type = type_;
  }

  bool operator==(UpgradeReadout rhs){
    return (type == rhs.type) && (launch == rhs.launch);
  }

  /**
   * Get the time from the raw class. Here, we use a generic visitation
   * to apply the GetTime operator of each of the lower level classes.
   * If we want to do something more complex (for example, get the times
   * of the discriminator crossings instead of the readout time), we
   * will need to use the raw classes or update this code.
   */
  double GetTime() const{
    return std::visit([](auto const& x){return x.GetTime();}, launch);
  }

  /**
   * Set the specified UpgradeLCFlags to 0/false.
   */
  void ResetLCFlags(UpgradeLCFlags flags){
    SetLCFlags(GetLCFlags() & ~flags);
  }

  /**
   * Get the UpgradeLCFlags associated with the wrapped readout.
   */
  UpgradeLCFlags GetLCFlags() const{
    return std::visit([](auto const& x){return x.GetLCFlags();}, launch);
  }

  /**
   * Set the wrapped readout's LC flags to match the given values.
   */
  void SetLCFlags(UpgradeLCFlags flags){
    std::visit([&](auto& x){x.SetLCFlags(flags);}, launch);
  }

  /**
   * Set the wrapped readout's LC flags corresponding to `flags` to 1/true.
   */
  void AddLCFlags(UpgradeLCFlags flags){
    std::visit([&](auto& x){x.AddLCFlags(flags);}, launch);
  }

  /**
   * Check whether the wrapped readout is of type T. The call will look
   * like my_wrapped_readout.Contains<T>() and return a true
   * if the contained launch is of type T.
   */
  template <typename T>
  bool Contains() const{
    return std::holds_alternative<T>(launch);
  }

  /**
   * Try to extract a readout of type T from the contained UpgradeLaunch.
   * This will return a nullptr if we attempt to get the wrong type, allowing
   * us to write code of the form
   *
   * @code
   * if(const auto* l      = my_wrapped_readout.Get<I3DEggLaunch>()){...}
   * else if(const auto* l = my_wrapped_readout.Get<I3mDOMLaunch>()){...}
   * else if(const auto* l = my_wrapped_readout.Get<I3XDOMHit>()){...}
   * else{log_fatal(...);}
   * @endcode
   */
  template <typename T>
  const T* Get() const{
    return std::get_if<T>(&launch);
  }

  /**
   * Define a helper function to take eg an I3DEggLaunchSeriesMap, I3mDOMLaunchSeriesMap,
   * or I3XDOMHitSeriesMap and return an UpgradeReadoutSeriesMap. This requires passing
   * in an I3OMGeo so we can correctly set the readout module type for I3XDOMHits. The
   * intended usage here is to be able to do
   *
   * @code
   * UpgradeReadoutSeriesMap merged_map;
   * merged_map.merge(UpgradeReadout::From<I3mDOMLaunchSeriesMap>(omgeoMap, mdomLaunches);
   * merged_map.merge(UpgradeReadout::From<I3DEggLaunchSeriesMap>(omgepMap, degglaunches);
   * merged_map.merge(UpgradeReadout::From<I3XDOMHitSeriesMap(omgeoMap, xdomhits);
   * @endcode
   */
  template<typename T> static
  UpgradeReadoutSeriesMap From(const T& inputMap, I3OMGeoMap omgeoMap){
    UpgradeReadoutSeriesMap outputMap;
    for(const auto& [omkey, hitSeries] : inputMap){
      UpgradeReadoutSeries readouts;
      auto omgeo = omgeoMap.find(omkey);
      if(omgeo == omgeoMap.end()){
        log_fatal_stream("OMKey " << omkey << " not found in I3OMGeoMap!");
      }
      for(const auto& hit : hitSeries){
        readouts.push_back(UpgradeReadout(hit, omgeo->second.omtype));
      }
      std::sort(readouts.begin(), readouts.end(),
                [](const UpgradeReadout& A, const UpgradeReadout& B){return A.GetTime() < B.GetTime();});
      outputMap[omkey] = readouts;
    }
    return outputMap;
  }
  
  /**
   * Define a templated helper function to pull various I3XSeriesMap objects from an
   * UpgradeReadoutSeriesMap. The intended usage is
   *
   * @code
   * I3DEggLaunchSeriesMap deggMap = UpgradeReadout::To<I3DEggLaunchSeriesMap>(readoutMap);
   * I3mDOMLaunchSeriesMap mdomMap = UpgradeReadout::To<I3mDOMLaunchSeriesMap>(readoutMap);
   * I3XDOMHitSeriesMap    xdomMap = UpgradeReadout::To<I3XDOMHitSeriesMap>(readoutMap);
   * @endcode
   */
  template<typename T> static
  T To(const UpgradeReadoutSeriesMap& readoutMap){
    T outputMap;
    for(const auto& [omkey, readoutSeries] : readoutMap){
      outputMap[omkey];
      for(const auto& readout : readoutSeries){
        auto hit = readout.template Get<typename T::mapped_type::value_type>();
        if(hit){
          outputMap[omkey].push_back(*hit);
        }
      }
      // If we didn't find any hits of the right type, remove this entry in the map.
      if(!outputMap[omkey].size()){
        outputMap.erase(omkey);
      }
    }
    return outputMap;
  }

  /**
   * Define a templated helper function to merge several separate UpgradeReadoutSeriesMap objects,
   * including merging hits on the same module from multiple sources. Hits are sorted in time in
   * each hit series after merging.
   *
   * @code
   * auto degg_readouts = UpgradeReadout::From<I3DEggLaunchSeriesMap>(omgeoMap, deggMap);
   * auto mdom_readouts = UpgradeReadout::From<I3mDOMLaunchSeriesMap>(omgeoMap, mdomMap);
   * auto xdom_readouts = UpgradeReadout::From<I3XDOMHitSeriesMap>(omgeoMap, xdomMap);
   * auto readoutMap = UpgradeReadout::Merge(degg_readouts, mdom_readouts, xdom_readouts);
   * @endcode
   */
  template <typename... Pack> static
  UpgradeReadoutSeriesMap Merge(const UpgradeReadoutSeriesMap& first,
                                const Pack&... rest)
  {
    UpgradeReadoutSeriesMap result(first);
    (MergeInto(result, rest), ...);
    return result;
  }
  
  /**
   * Define a templated helper function to merge two separate UpgradeReadoutSeriesMap objects,
   * including merging hits on the same module from multiple sources. This is private because
   * it is intended to be used only through the variadic Merge(...) code above.
   *
   * @code
   * UpgradeReadoutSeriesMap readoutMap;
   * UpgradeReadout::Merge(readoutMap, UpgradeReadout::To<I3DEggLaunchSeriesMap>(readoutMap));
   * UpgradeReadout::Merge(readoutMap, UpgradeReadout::To<I3mDOMLaunchSeriesMap>(readoutMap));
   * UpgradeReadout::Merge(readoutMap, UpgradeReadout::To<I3XDOMHitSeriesMap>(readoutMap));
   * @endcode
   */
  static
  void MergeInto(UpgradeReadoutSeriesMap& toMap,
                 const UpgradeReadoutSeriesMap& fromMap){
    for(const auto& [omkey, readouts] : fromMap){
      if(toMap.find(omkey) == toMap.end()){
        toMap[omkey] = fromMap.at(omkey);
        continue;
      }
      
      // Overlapping key. Merge the vectors
      toMap[omkey].insert(toMap[omkey].end(),
                          fromMap.at(omkey).begin(),
                          fromMap.at(omkey).end());
      // Sort the results now.
      std::sort(toMap[omkey].begin(), toMap[omkey].end(),
                [](auto A, auto B){return A.GetTime() < B.GetTime();});
    }
    return;
  }

  Readout launch;
  I3OMGeo::OMType type;
};

#endif  // I3UPGRADEREADOUT_H_INCLUDED
