////////////////////////////////////////////////////////////
//Author: Alan Coleman
//A set of functions used to calculate the Analytic signal
//and the Hilbert envelope from a timeseries (or FFTDataContainer)
////////////////////////////////////////////////////////////

#ifndef HILBERTTOOLS_INCLUDE_H
#define HILBERTTOOLS_INCLUDE_H

#include <dataclasses/I3AntennaWaveform.h>
#include <dataclasses/fft/FFTDataContainer.h>

namespace fft {
//Returns the hilbert envelope for the various input types
AntennaTimeSeries GetHilbertEnvelope(const FFTData &data);
EFieldTimeSeries GetHilbertEnvelope(const FFTData3D& data);
AntennaTimeSeries GetHilbertEnvelope(const AntennaTimeSeries &timeSeries);
EFieldTimeSeries GetHilbertEnvelope(const EFieldTimeSeries &timeSeries);

//The analytic signal is the Hilbert Transform of the timeseries and
//is used to get the Hilbert Envelope
AntennaSpectrum GetAnalyticSignal(const FFTData &data);
AntennaSpectrum GetAnalyticSignal(const AntennaTimeSeries &timeSeries);

//Base function that performs the transformation
//This would be a great place to implement GPUs...
void ApplyHilbertTransform(std::complex<double> *data, const int n);

//Returns the time at which the Hilbert envelope reaches its peak
//Written for all the potential data types
double GetHilbertPeakTime(const FFTData &data);
double GetHilbertPeakTime(const AntennaTimeSeries &timeSeries);
double GetHilbertPeakTime(const FFTData &data, double &maxVal);
double GetHilbertPeakTime(const AntennaTimeSeries &timeSeries, double &maxVal);
double GetHilbertPeakTime(const FFTData3D &data, double &maxVal);
double GetHilbertPeakTime(const EFieldTimeSeries &timeSeries, double &maxVal);

}
#endif
