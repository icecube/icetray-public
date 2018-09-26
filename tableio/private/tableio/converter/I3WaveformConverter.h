/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#ifndef TABLEIO_I3WAVEFORMCONVERTER_H_INCLUDED
#define TABLEIO_I3WAVEFORMCONVERTER_H_INCLUDED

#include "tableio/I3Converter.h"
#include "dataclasses/physics/I3Waveform.h"

class I3WaveformConverter : public I3ConverterImplementation<I3WaveformSeriesMap > {
public:
    I3WaveformConverter();
    I3WaveformConverter(std::string atwdName, std::string fadcwaveform, bool calibrate);
private:
    I3TableRowDescriptionPtr CreateDescription(const I3WaveformSeriesMap& waveforms);
    size_t FillRows(const I3WaveformSeriesMap& waveforms, I3TableRowPtr rows);
    size_t GetNumberOfRows(const I3WaveformSeriesMap& waveforms);

    std::string atwdName_;
    std::string fadcName_;
    bool calibrate_;
};

#endif // TABLEIO_I3WAVEFORMCONVERTER_H_INCLUDED
