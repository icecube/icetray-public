// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

//Converter for I3AntennaDataMap, based on I3WaveformSeriesMapConverter

#include "tableio/converter/I3AntennaDataMapConverter.h"

#include <dataclasses/I3AntennaDataMap.h>
#include <dataclasses/I3AntennaChannel.h>
#include <dataclasses/I3AntennaWaveform.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <icetray/I3Units.h>
#include <icetray/I3Frame.h>

#include <boost/foreach.hpp>


namespace {

  size_t getWaveformLength(const I3AntennaDataMap &antmap)
  {
    for (I3AntennaDataMap::const_iterator iter = antmap.begin();
        iter != antmap.end(); ++iter)
    {
      BOOST_FOREACH(const auto& pair, iter->second)
      {
        const I3AntennaChannel& channel = pair.second;
        if (channel.GetConstFFTData().GetConstTimeSeries().GetSize()) return channel.GetConstFFTData().GetConstTimeSeries().GetSize();
      }
    }

    return 0;
  }

}

/******************************************************************************/

I3AntennaDataMapConverter::I3AntennaDataMapConverter(bool bookGeometry)
  : I3ConverterImplementation<I3AntennaDataMap>(),
    bookGeometry_(bookGeometry)
{}

/******************************************************************************/

I3TableRowDescriptionPtr I3AntennaDataMapConverter::CreateDescription(const I3AntennaDataMap& antmap)
{
  I3TableRowDescriptionPtr desc = I3TableRowDescriptionPtr(new I3TableRowDescription() );

  size_t wfSize(getWaveformLength(antmap));
  if (wfSize == 0)
    log_error("Got some zero length waveforms.");

  desc->isMultiRow_ = true;
  desc->AddField<int32_t>("station", "", "Station ID");
  desc->AddField<uint32_t>("antenna", "", "Antenna ID");

  if (bookGeometry_) {
    desc->AddField<double>("x", "m", "X coordinate of the antenna");
    desc->AddField<double>("y", "m", "Y coordinate of the antenna");
    desc->AddField<double>("z", "m", "Z coordinate of the antenna");
  }

  desc->AddField<double>("offset", "ns", "start time of waveform");
  desc->AddField<double>("binning", "ns", "width of waveform bins");
  desc->AddField<uint16_t>("nbins", "", "number of waveform bins");
  desc->AddField<double>("waveform_ch0", "", "waveforms of channel 0", wfSize);
  desc->AddField<double>("waveform_ch1", "", "waveforms of channel 1", wfSize);

  return desc;
}

size_t I3AntennaDataMapConverter::FillRows(const I3AntennaDataMap& antmap,
                                           I3TableRowPtr rows)
{
  static int nGeometryWarnings = 0;

  I3GeometryConstPtr geometry;
  if (bookGeometry_) {
    if (!currentFrame_)  // obsolete check?
      log_fatal("Trying to book geometry, but the current frame is not set.");
    geometry = currentFrame_->Get<I3GeometryConstPtr>();
    if (!geometry) {
      log_error("%s: No geometry in frame", __PRETTY_FUNCTION__);
      return 0;
    }
  }


  I3Map<AntennaKey, I3Map<int, I3AntennaChannel> >::const_iterator iter;

  const size_t startRow = rows->GetCurrentRow();
  const I3TableRowDescription &desc = *rows->GetDescription();
  const size_t wfsize = desc.GetFieldArrayLengths()[desc.GetFieldColumn("waveform_ch0")];

  size_t currentRow;
  for (iter = antmap.begin(), currentRow = rows->GetCurrentRow();
       iter != antmap.end(); ++iter)
    {
      rows->SetCurrentRow(currentRow++);

      AntennaKey key = iter->first;

      I3AntennaGeo antennageo;
      if (bookGeometry_) {
        I3AntennaGeoMap::const_iterator geoiter = geometry->antennageo.find(key);
        if (geoiter == geometry->antennageo.end()) {
          log_warn("%s: AntennaKey (%d,%d) not in geometry!", __PRETTY_FUNCTION__,
               key.GetStationID(), key.GetAntennaID());
          ++nGeometryWarnings;
          if (nGeometryWarnings >= 100)
            log_info("Warned 100 times. Will suppress any further warnings.");
        } else {
          antennageo = geoiter->second;
        }
      }

      // write out the timeseries
      const I3AntennaChannelMap& chanmap = iter->second;
      // make sure there are exactly two channels per antenna
      if (chanmap.size() != 2) {
        log_fatal("%s: The number of channels for AntennaKey (%d,%d) is %zu, not 2!", __PRETTY_FUNCTION__,
                  key.GetStationID(), key.GetAntennaID(), chanmap.size());
      }

      const AntennaTimeSeries& ts0 = chanmap.at(0).GetConstFFTData().GetConstTimeSeries();
      const AntennaTimeSeries& ts1 = chanmap.at(1).GetConstFFTData().GetConstTimeSeries();
      rows->Set<int32_t>("station", key.GetStationID());
      rows->Set<uint32_t>("antenna", key.GetAntennaID());
      rows->Set<double>("offset", ts0.GetOffset()/I3Units::ns);
      rows->Set<double>("binning", ts0.GetBinning()/I3Units::ns);
      rows->Set<uint16_t>("nbins", ts0.GetSize());

      if (bookGeometry_) {
        rows->Set<double>("x", antennageo.GetPosition().GetX());
        rows->Set<double>("y", antennageo.GetPosition().GetY());
        rows->Set<double>("z", antennageo.GetPosition().GetZ());
      }

      // channel 0
      AntennaTimeSeries::ConstIterator wfiter;
      double* buffer0 = rows->GetPointer<double>("waveform_ch0");
      unsigned i = 0;

      for (wfiter = ts0.Begin();
          i < wfsize && wfiter != ts0.End();
          ++i, ++wfiter)
        {
          buffer0[i] = *wfiter;
        }

      // channel 1
      double* buffer1 = rows->GetPointer<double>("waveform_ch1");
      i = 0;

      for (wfiter = ts1.Begin();
          i < wfsize && wfiter != ts1.End();
          ++i, ++wfiter++)
        {
          buffer1[i] = *wfiter;
        }
  }
  return currentRow - startRow;

}

/******************************************************************************/

size_t I3AntennaDataMapConverter::GetNumberOfRows(const I3AntennaDataMap& antmap) {
  size_t size = 0;
  BOOST_FOREACH(const I3AntennaDataMap::value_type &pair, antmap)
  if (pair.second.size() > 0)
    size++;
  return size;
}

/******************************************************************************/
