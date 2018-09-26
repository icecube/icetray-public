#include <icetray/serialization.h>
#include <dataclasses/physics/I3Waveform.h>

#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>


using namespace std;
using namespace boost::lambda;


I3Waveform::StatusCompound::~StatusCompound() {}

template <class Archive>
void I3Waveform::StatusCompound::save(Archive& ar, unsigned version) const
{
  ar & make_nvp("interval", interval_);
  ar & make_nvp("status", status_);
  ar & make_nvp("channel", channel_);
}

template <class Archive>
void I3Waveform::StatusCompound::load(Archive& ar, unsigned version)
{
  if (version>i3waveform_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Waveform class",version,i3waveform_version_);

  ar & make_nvp("interval", interval_);

  if (version < 3) {
    unsigned status;
    ar & make_nvp("status", status);
    switch (status) {
      case 0: status_ = VIRGINAL; break;  /* was: VIRGINAL */
      case 10: status_ = COMBINED; break; /* was: SHADY */
      default: status_ = SATURATED;       /* was: ADULTERATED (20) */
    }
    channel_ = -1;
  } else {
    ar & make_nvp("status", status_);
    ar & make_nvp("channel", channel_);
  }
}

I3_SPLIT_SERIALIZABLE(I3Waveform::StatusCompound);

unsigned I3Waveform::GetStatus(const vector<StatusCompound>& waveformInfo)
{
  unsigned retVal = VIRGINAL;

  std::vector<StatusCompound>::const_iterator it = waveformInfo.begin();
  for ( ; it != waveformInfo.end(); it++)
    retVal |= it->GetStatus();

  return retVal;
}

unsigned
I3Waveform::GetStatus() const
{
	return GetStatus(waveformInfo_);
}

int
I3Waveform::GetChannel() const
{
	int channel = 0;
	
	BOOST_FOREACH(const StatusCompound &stat, waveformInfo_)
		if (stat.GetChannel() > channel)
			channel = stat.GetChannel();
	
	return channel;
}


I3Waveform::~I3Waveform() {}


template <class Archive>
void I3Waveform::save(Archive& ar, unsigned version) const
{
  if (version>i3waveform_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Waveform class.",version,i3waveform_version_);

  ar & make_nvp("startTime", startTime_);
  ar & make_nvp("binWidth", binWidth_);
  ar & make_nvp("waveform", waveform_);
  ar & make_nvp("waveformInformation", waveformInfo_);
  ar & make_nvp("source", source_.bits);
}

template <class Archive>
void I3Waveform::load(Archive& ar, unsigned version)
{
  if (version>i3waveform_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Waveform class.",version,i3waveform_version_);

  ar & make_nvp("startTime", startTime_);
  ar & make_nvp("binWidth", binWidth_);
  ar & make_nvp("waveform", waveform_);
  // for version > 0 there is a waveform info to be serialized
  if(version > 0)
    ar & make_nvp("waveformInformation", waveformInfo_);
  // for version <= 0 there is no waveform info available
  // clear waveform info instead (assuming the full waveform is "VIRGINAL")
  else
    waveformInfo_.clear();

	/* Convert old-style enum to current struct. */
	if (version < 3) {
		int source;
		ar & make_nvp("source", source);
		source_.bits = 0;
		source_.fields.source = source/10;
	} else {
		uint8_t bits;
		ar & make_nvp("source", bits);
		source_.bits = bits;
	}
}

bool
operator==(const I3Waveform& lhs, const I3Waveform& rhs)
{
  return lhs.GetStartTime() == rhs.GetStartTime() 
    && lhs.GetBinWidth() == rhs.GetBinWidth() 
    && lhs.GetWaveform() == rhs.GetWaveform()
    && lhs.GetWaveformInformation() == rhs.GetWaveformInformation();
}

std::ostream& I3Waveform::Print(std::ostream& oss) const
{
  std::string srcstr;
  if (GetSource() == I3Waveform::ATWD) srcstr.append("ATWD");
  if (GetSource() == I3Waveform::FADC) srcstr.append("FADC");
  if (GetSource() == I3Waveform::SLC) srcstr.append("SLC");
  oss << "[I3Waveform:\n"
      << "  StartTime : " << GetStartTime() << '\n'
      << "      isHLC : " << IsHLC() << '\n'
      << "     Source : " << srcstr << "\n]";
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3Waveform& wf)
{
  return(wf.Print(oss));
}

std::ostream& I3Waveform::StatusCompound::Print(std::ostream& oss) const
{
  std::string srcstr;
  if (GetStatus() == I3Waveform::VIRGINAL) srcstr.append("VIRGINAL");
  if (GetStatus() == I3Waveform::COMBINED) srcstr.append("COMBINED");
  if (GetStatus() == I3Waveform::SATURATED) srcstr.append("SATURATED");
  if (GetStatus() == I3Waveform::UNDERSHOT) srcstr.append("UNDERSHOT");

  oss << "[I3Waveform::StatusCompound: \n"
      << "                        Range : " << GetInterval().first << "--" << GetInterval().second << '\n'
      << "                      Channel : " << GetChannel() << '\n'
      << "                       Status : " << srcstr << "\n]";
  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3Waveform::StatusCompound& sc)
{
  return(sc.Print(oss));
}

I3_SPLIT_SERIALIZABLE(I3Waveform);

I3_SERIALIZABLE(I3WaveformSeriesMap);
