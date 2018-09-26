/**
    copyright  (C) 2004
    the icecube collaboration
    $Id$

    @version $Revision$
    @date $Date$
    @author tschmidt
*/

#include <I3Test.h>

#include <dataclasses/physics/I3Waveform.h>


using namespace std;


TEST_GROUP(I3WaveformTest);


TEST(GetStatusFromWaveformInformation)
{
  I3Waveform wf;
  vector<I3Waveform::StatusCompound> &waveformInfo = wf.GetWaveformInformation();

  I3Waveform::StatusCompound virginal;
  I3Waveform::StatusCompound shady;
  I3Waveform::StatusCompound adulterated;
  I3Waveform::StatusCompound undershot;
  
  virginal.SetStatus(I3Waveform::VIRGINAL);
  virginal.SetChannel(0);
  shady.SetStatus(I3Waveform::COMBINED);
  shady.SetChannel(1);
  adulterated.SetStatus(I3Waveform::SATURATED);
  adulterated.SetChannel(2);
  undershot.SetStatus(I3Waveform::UNDERSHOT);
  
  ENSURE_EQUAL(I3Waveform::GetStatus(waveformInfo), (unsigned)I3Waveform::VIRGINAL);
  ENSURE_EQUAL(wf.GetChannel(), 0);
  
  waveformInfo.push_back(virginal);
  ENSURE_EQUAL(I3Waveform::GetStatus(waveformInfo), (unsigned)I3Waveform::VIRGINAL);
  ENSURE_EQUAL(wf.GetChannel(), 0);
  
  waveformInfo.push_back(shady);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::COMBINED);
  ENSURE(!(I3Waveform::GetStatus(waveformInfo) & I3Waveform::SATURATED));
  ENSURE(!(I3Waveform::GetStatus(waveformInfo) & I3Waveform::UNDERSHOT));
  ENSURE_EQUAL(wf.GetChannel(), 1);
  
  waveformInfo.push_back(virginal);
  ENSURE_EQUAL(I3Waveform::GetStatus(waveformInfo), (unsigned)I3Waveform::COMBINED);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::COMBINED);
  ENSURE(!(I3Waveform::GetStatus(waveformInfo) & I3Waveform::SATURATED));
  ENSURE(!(I3Waveform::GetStatus(waveformInfo) & I3Waveform::UNDERSHOT));
  ENSURE_EQUAL(wf.GetChannel(), 1);
  
  waveformInfo.push_back(adulterated);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::COMBINED);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::SATURATED);
  ENSURE(!(I3Waveform::GetStatus(waveformInfo) & I3Waveform::UNDERSHOT));
  ENSURE_EQUAL(wf.GetChannel(), 2);
  
  waveformInfo.push_back(shady);
  waveformInfo.push_back(virginal);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::COMBINED);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::SATURATED);
  ENSURE(!(I3Waveform::GetStatus(waveformInfo) & I3Waveform::UNDERSHOT));
  ENSURE_EQUAL(wf.GetChannel(), 2);

  waveformInfo.push_back(undershot);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::COMBINED);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::SATURATED);
  ENSURE(I3Waveform::GetStatus(waveformInfo) & I3Waveform::UNDERSHOT);
  ENSURE_EQUAL(wf.GetChannel(), 2);

  waveformInfo.clear();
  waveformInfo.push_back(shady);
  waveformInfo.push_back(shady);  
  ENSURE_EQUAL(I3Waveform::GetStatus(waveformInfo), (unsigned)I3Waveform::COMBINED);
}
