// SPDX-FileCopyrightText: 2025 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

//Converter for I3AntennaDataMap, based on I3WaveformSeriesMapConverter

#ifndef TABLEIO_I3ANTENNADATAMAPCONVERTER_H_INCLUDED
#define TABLEIO_I3ANTENNADATAMAPCONVERTER_H_INCLUDED

#include "tableio/I3Converter.h"
#include "dataclasses/I3AntennaDataMap.h"

class I3AntennaDataMapConverter : public I3ConverterImplementation< I3AntennaDataMap > {
public:
    I3AntennaDataMapConverter(bool bookGeometry = false);

private:
    I3TableRowDescriptionPtr CreateDescription(const I3AntennaDataMap& antmap);
    size_t FillRows(const I3AntennaDataMap& antmap, I3TableRowPtr rows);
    size_t GetNumberOfRows(const I3AntennaDataMap& antmap);

    bool bookGeometry_;
};

#endif // TABLEIO_I3ANTENNADATAMAPCONVERTER_H_INCLUDED
