#include <I3Test.h>

#include <sys/time.h>
#include <numeric>

#include "icetray/OMKey.h"
#include "icetray/I3Tray.h"
#include "icetray/I3Units.h"
#include "dataclasses/physics/I3EventHeader.h"
#include "dataclasses/physics/I3RecoPulse.h"

#include "dataclasses/payload/I3SuperDST.h"
#include "dataclasses/payload/I3SuperDSTUtils.h"

#include <archive/portable_binary_archive.hpp>
#include <boost/filesystem.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <boost/assign/std/vector.hpp>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>

#include <boost/python/import.hpp>

TEST_GROUP(I3SuperDST)

struct Empty {
	template <typename Archive>
	void serialize(Archive &ar, unsigned version) {}
};
I3_SERIALIZABLE(Empty);

template <class Archiveable>
void resurrect(const Archiveable &in, Archiveable &out, size_t stream_size=0)
{
	namespace io = boost::iostreams;
	typedef std::vector<char> buffer_t;
	typedef io::stream<io::back_insert_device<buffer_t > > sink_t;
	typedef io::stream<io::array_source> source_t;

	buffer_t buffer;
	sink_t sink(buffer);
	/* This is the archive type instantiated by I3_SERIALIZABLE */
	icecube::archive::portable_binary_oarchive oarchive(sink);

	/* Find the size of an empty archive */
	size_t base_size = 0;
	if (stream_size > 0) {
		Empty probe;
		oarchive << probe;
		sink.flush();
		base_size = buffer.size();
		buffer.clear();
	}

	oarchive << in;
	sink.flush();

	if (stream_size > 0)
		ENSURE_EQUAL(buffer.size()-base_size, stream_size);

	source_t source(&*buffer.begin(), &*buffer.end());
	icecube::archive::portable_binary_iarchive iarchive(source);
	iarchive >> out;
}

static double uniform(double lo=0., double hi=1.)
{
	return lo + (hi-lo)*double(random())/RAND_MAX;
}

TEST(RunCodec)
{
	using namespace I3SuperDSTUtils;
	using namespace boost::assign;

	RunCodec::vector_t values, codes, revalues;
	//icodec.values_ += 1,1,1,2,2,1,1,1;
	values.insert(values.end(), 6, 1);
	values.insert(values.end(), 255, 2);
	values.insert(values.end(), 1025, 3);

	RunCodec::Encode(values, codes);
	
	ENSURE_EQUAL(codes.size(), 1u+2u+3u);

	RunCodec::Decode(codes, revalues);

	ENSURE_EQUAL(values.size(), revalues.size());
	for (unsigned i=0; i < values.size(); i++)
		ENSURE_EQUAL(values[i], revalues[i]);
}

TEST(SizeCodec)
{
	using namespace I3SuperDSTUtils;
	using namespace boost::assign;

	SizeCodec icodec, codec;

	typedef std::vector<std::pair<uint64_t, size_t> > spec_t;
	spec_t specs;
	push_back(specs)(0,1)(246,1)(247,2);
	for (unsigned i = 1; i < std::min(sizeof(size_t)+1,size_t(7)); i++) {
		specs.push_back(std::make_pair((uint64_t(1) << i*8)-1, i+1));
		specs.push_back(std::make_pair((uint64_t(1) << i*8)-2, i+1));
	}
	if (sizeof(size_t) >= 8)
		push_back(specs)(UINT64_MAX, 9);

	BOOST_FOREACH(const spec_t::value_type &spec, specs) {
		icodec = SizeCodec(spec.first);
		//resurrect(icodec, codec, spec.second);
		resurrect(icodec, codec);
		ENSURE_EQUAL(icodec.size_, codec.size_)
	}
}

TEST(OMKeyCoding)
{
	OMKey key1(3,60);
	OMKey key2(3,59);
	
	ENSURE_EQUAL(key1, I3SuperDST::DecodeOMKey(I3SuperDST::EncodeOMKey(key1, 13)));
	ENSURE_EQUAL(key2, I3SuperDST::DecodeOMKey(I3SuperDST::EncodeOMKey(key2, 13)));
	
	ENSURE( I3SuperDST::DecodeOMKey(I3SuperDST::EncodeOMKey(key1, 13)) !=
	    I3SuperDST::DecodeOMKey(I3SuperDST::EncodeOMKey(key2, 13)));
	
	key1 = OMKey(32,60);
	
	ENSURE_EQUAL(key1, I3SuperDST::DecodeOMKey(I3SuperDST::EncodeOMKey(key1, 13)));
	
	key1 = OMKey(86,60); /* Largest OM number in the full detector */
	
	ENSURE_EQUAL(key1, I3SuperDST::DecodeOMKey(I3SuperDST::EncodeOMKey(key1, 13)));
	
	key1 = OMKey(127,64); /* Largest representable code */
	
	ENSURE_EQUAL(key1, I3SuperDST::DecodeOMKey(I3SuperDST::EncodeOMKey(key1, 13)));
}

TEST(LambdaMadness)
{
	std::vector<I3RecoPulse> pulses;
	unsigned i;
	
	for (i = 0; i < 10; i++) {
		I3RecoPulse p;
		p.SetTime(uniform(1.0, 6400.0));
		p.SetCharge(uniform(0.1, 9.0));
		
		pulses.push_back(p);
	}
	
	double qtot = std::accumulate(pulses.begin(), pulses.end(), 0.0,
	    boost::lambda::_1 += boost::lambda::bind(&I3RecoPulse::GetCharge, boost::lambda::_2));
	
	double qtest = 0.0;
	for (i = 0; i < pulses.size(); i++)
		qtest += pulses[i].GetCharge();
		
	ENSURE(qtot != 0.0);
	ENSURE_EQUAL(qtot, qtest);
}

TEST(EmptySerialization)
{
	I3SuperDST supi, supa;
	resurrect(supi, supa);
}

TEST(Serialization)
{
	I3RecoPulseSeriesMap pulsemap;
	I3EventHeader head;
	OMKey key1(55,47);
	OMKey key2(23,17);
	std::vector<double> times;
	int i;
	
	for (i = 0; i < 10; i++)
		times.push_back(uniform(1.0, 6400.0));
		
	std::sort(times.begin(), times.end());
	
	for (i = 0; i < 10; i++) {
		I3RecoPulse p;
		p.SetTime(times[i]);
		p.SetCharge(uniform(0.1, 9.0));
		p.SetWidth(3.5);
		pulsemap[key1].push_back(p);
		p.SetTime(times[i] + 17.3);
		pulsemap[key2].push_back(p);
	}
	
	I3RecoPulseSeriesMap slc_pulsemap;
	I3RecoPulseSeriesMap::iterator map_it;
	I3RecoPulseSeries::iterator pulse_it;
	
	for (map_it = pulsemap.begin(); map_it != pulsemap.end(); map_it++) {
		for (pulse_it = map_it->second.begin(); pulse_it != map_it->second.end(); pulse_it++) {
			I3RecoPulse pulse(*pulse_it);
			pulse.SetTime(pulse_it->GetTime() * 1.5);
			slc_pulsemap[map_it->first].push_back(pulse);
		}
	}
	slc_pulsemap[key1].resize(3);
	slc_pulsemap[key2].resize(3);
	
	unsigned n_slc(0), n_hlc(0), n_slc_readouts(0), n_hlc_readouts(0);
	
	I3SuperDST supi(pulsemap);
	std::list<I3SuperDSTReadout>::const_iterator lit1, lit2;
	std::vector<I3SuperDSTChargeStamp>::const_iterator stit1, stit2;
		
	std::list<I3SuperDSTReadout> hlc_readouts = supi.GetHLCReadouts();
	std::list<I3SuperDSTReadout> slc_readouts = supi.GetSLCReadouts();
	
	for (lit1 = hlc_readouts.begin(); lit1 != hlc_readouts.end(); lit1++) {
		n_hlc_readouts++;
		n_hlc += lit1->stamps_.size();
	}
	for (lit1 = slc_readouts.begin(); lit1 != slc_readouts.end(); lit1++) {
		n_slc_readouts++;
		n_slc += lit1->stamps_.size();
	}

	I3SuperDST supa;
	resurrect(supi, supa);
	
	std::list<I3SuperDSTReadout> hlc_readouts_unz = supa.GetHLCReadouts();
	std::list<I3SuperDSTReadout> slc_readouts_unz = supa.GetSLCReadouts();
	
	ENSURE_EQUAL( hlc_readouts.size(),
	    hlc_readouts_unz.size(), "Same number of keys");
	ENSURE_EQUAL( slc_readouts.size(),
	    slc_readouts_unz.size(), "Same number of keys");
	
	lit1 = hlc_readouts.begin();
	lit2 = hlc_readouts_unz.begin();
	for ( ; lit1 != hlc_readouts.end() && lit2 != hlc_readouts_unz.end(); lit1++, lit2++) {
		ENSURE_EQUAL(lit1->om_, lit2->om_);
		ENSURE_EQUAL(lit1->kind_, lit2->kind_);
		ENSURE(lit1->stamps_.size() > 0);
		ENSURE_EQUAL(lit1->stamps_.size(), lit2->stamps_.size());
		ENSURE_DISTANCE(lit1->start_time_, lit2->start_time_,
		    I3SuperDST::DecodeTime(1), "Start time is within "
		    "discretization error.");
		stit1 = lit1->stamps_.begin();
		stit2 = lit2->stamps_.begin();
		for ( ; stit1 != lit1->stamps_.end() && stit2 != lit2->stamps_.end(); stit1++, stit2++) {
			ENSURE_EQUAL(stit1->GetTimeCode(), stit2->GetTimeCode());
			ENSURE_EQUAL(stit1->GetChargeCode(), stit2->GetChargeCode());
			ENSURE_EQUAL(stit1->GetTime(), stit2->GetTime());
			ENSURE_EQUAL(stit1->GetCharge(), stit2->GetCharge());
		}
	}
	
	lit1 = slc_readouts.begin();
	lit2 = slc_readouts_unz.begin();
	for ( ; lit1 != slc_readouts.end() && lit2 != slc_readouts_unz.end(); lit1++, lit2++) {
		ENSURE_EQUAL(lit1->om_, lit2->om_);
		ENSURE_EQUAL(lit1->kind_, lit2->kind_);
		ENSURE(lit1->stamps_.size() > 0);
		ENSURE_EQUAL(lit1->stamps_.size(), lit2->stamps_.size());
		ENSURE_DISTANCE(lit1->start_time_, lit2->start_time_,
		    I3SuperDST::DecodeTime(1), "Start time is within "
		    "discretization error.");
		stit1 = lit1->stamps_.begin();
		stit2 = lit2->stamps_.begin();
		for ( ; stit1 != lit1->stamps_.end() && stit2 != lit2->stamps_.end(); stit1++, stit2++) {
			ENSURE_EQUAL(stit1->GetTimeCode(), stit2->GetTimeCode());
			ENSURE_EQUAL(stit1->GetChargeCode(), stit2->GetChargeCode());
			ENSURE_EQUAL(stit1->GetTime(), stit2->GetTime());
			ENSURE_EQUAL(stit1->GetCharge(), stit2->GetCharge());
		}
	}
}

void
ensure_superdst_equal(const I3SuperDST &sa, const I3SuperDST &b)
{
	I3RecoPulseSeriesMapConstPtr pa = sa.Unpack();
	I3RecoPulseSeriesMapConstPtr pb = sa.Unpack();
	typedef I3RecoPulseSeriesMap::const_iterator map_iter;
	typedef I3RecoPulseSeries::const_iterator series_iter;
	
	ENSURE_EQUAL(pa->size(), pb->size());
	for (map_iter pair_a=pa->begin(), pair_b=pb->begin(); pair_a!=pa->end(); pair_a++,pair_b++) {
		ENSURE_EQUAL(pair_a->first, pair_b->first);
		ENSURE_EQUAL(pair_a->second.size(), pair_b->second.size());
		for (series_iter pulse_a=pair_a->second.begin(), pulse_b=pair_b->second.begin(); pulse_a != pair_a->second.end(); pulse_a++,pulse_b++) {
			ENSURE_EQUAL(*pulse_a, *pulse_b);
		}
	}
}

TEST(TimeOverflow)
{
	OMKey key1(55,47);
	OMKey key2(23,17);
	
	// a time difference of exactly max_timecode_header
	// + max_overflow should be representable
	int dt = 65535 + 2047;
	for (int offset=-1; offset < 2; offset++) {
	
		I3RecoPulseSeriesMap pmap;
		I3RecoPulse pulse;
		pulse.SetTime(0);
		pulse.SetCharge(0);
		pulse.SetWidth(4);
		pulse.SetFlags(I3RecoPulse::LC);
		pmap[key1].push_back(pulse);

		pulse.SetTime(dt + offset);
		pmap[key2].push_back(pulse);
			
		I3SuperDST supi(pmap), supa;
		resurrect(supi, supa);
		ensure_superdst_equal(supi, supa);
	}
}

TEST(ChargeOverflow)
{
	OMKey key1(55,47);
	
	// a charged of exactly max_chargecode
	// + max_overflow should be representable
	int q = (1u<<6)-1 + UINT16_MAX;
	for (int offset=-1; offset < 2; offset++) {
		I3RecoPulseSeriesMap pmap;
		I3RecoPulse pulse;
		pulse.SetTime(0);
		// NB: we add one because the encoding floor()s the charge
		pulse.SetCharge((q+offset+1)*0.05);
		pulse.SetWidth(4);
		pulse.SetFlags(I3RecoPulse::LC);
		pmap[key1].push_back(pulse);
			
		I3SuperDST supi(pmap), supa;
		resurrect(supi, supa);
		ensure_superdst_equal(supi, supa);
	}
}

TEST(ZeroWidth)
{
	OMKey key1(55,47);
	
	// a charged of exactly max_chargecode
	// + max_overflow should be representable
	int q = (1u<<6)-1 + UINT16_MAX;
	for (int offset=-1; offset < 2; offset++) {
		I3RecoPulseSeriesMap pmap;
		I3RecoPulse pulse;
		pulse.SetTime(0);
		// NB: we add one because the encoding floor()s the charge
		pulse.SetCharge((q+offset+1)*0.05);
		pulse.SetWidth(0.);
		pulse.SetFlags(I3RecoPulse::LC);
		pmap[key1].push_back(pulse);
			
		I3SuperDST supi(pmap), supa;
		resurrect(supi, supa);
		ensure_superdst_equal(supi, supa);
	}
}

TEST(UnpackingAutomagically)
{
	I3RecoPulseSeriesMap pulsemap;
	I3EventHeader head;
	OMKey key1(55,47);
	OMKey key2(23,17);
	std::vector<double> times;
	int i;
	
	for (i = 0; i < 10; i++)
		times.push_back(uniform(1.0, 6400.0));
		
	std::sort(times.begin(), times.end());
	
	for (i = 0; i < 10; i++) {
		I3RecoPulse p;
		p.SetTime(times[i]);
		p.SetCharge(uniform(0.1, 9.0));
		p.SetWidth(3.5);
		pulsemap[key1].push_back(p);
		p.SetTime(times[i] + 17.3);
		pulsemap[key2].push_back(p);
	}
	
	I3RecoPulseSeriesMap slc_pulsemap;
	I3RecoPulseSeriesMap::iterator pmap_it;
	I3RecoPulseSeries::iterator pulse_it;
	
	for (pmap_it = pulsemap.begin(); pmap_it != pulsemap.end(); pmap_it++) {
		for (pulse_it = pmap_it->second.begin(); pulse_it != pmap_it->second.end(); pulse_it++) {
			I3RecoPulse pulse(*pulse_it);
			pulse.SetTime(pulse_it->GetTime() * 1.5);
			slc_pulsemap[pmap_it->first].push_back(pulse);
		}
	}
	slc_pulsemap[key1].resize(3);
	slc_pulsemap[key2].resize(3);
	
	I3RecoPulseSeriesMapPtr fake_hlc;
	I3RecoPulseSeriesMapPtr fake_slc;
	I3RecoPulseSeriesMap::const_iterator map_it, umap_it;
	
	I3SuperDSTPtr supi = boost::make_shared<I3SuperDST>(pulsemap);
	I3Frame frame;
	frame.Put("I3SuperDST", supi);
	
	I3RecoPulseSeriesMapConstPtr unpacked = frame.Get<I3RecoPulseSeriesMapConstPtr>("I3SuperDST");
	
	ENSURE((bool)unpacked, "I3Frame::Get() returned a valid pointer");
	
	ENSURE_EQUAL(unpacked->size(), 2u, "There's something in the unpacked map");

	map_it = pulsemap.find(key1);
	umap_it = unpacked->find(key1);
	ENSURE( map_it != pulsemap.end(), "Key 1 is in the original pulse series");
	ENSURE( umap_it != unpacked->end(), "Key 1 was unpacked");
	ENSURE( map_it->second.size() > 0, "There are pulses at key 1");
	ENSURE_EQUAL( umap_it->second.size(), map_it->second.size(), "All pulses were unpacked");
}

TEST(Unpacking)
{
	I3RecoPulseSeriesMap pulsemap;
	I3EventHeader head;
	OMKey key1(55,47);
	OMKey key2(23,17);
	std::vector<double> times;
	int i;
	
	for (i = 0; i < 10; i++)
		times.push_back(uniform(1.0, 6400.0));
		
	std::sort(times.begin(), times.end());
	
	for (i = 0; i < 10; i++) {
		I3RecoPulse p;
		p.SetTime(times[i]);
		p.SetCharge(uniform(0.1, 9.0));
		p.SetWidth(3.5);
		pulsemap[key1].push_back(p);
		p.SetTime(times[i] + 17.3);
		pulsemap[key2].push_back(p);
	}
	
	I3RecoPulseSeriesMap slc_pulsemap;
	I3RecoPulseSeriesMap::iterator pmap_it;
	I3RecoPulseSeries::iterator pulse_it;
	
	for (pmap_it = pulsemap.begin(); pmap_it != pulsemap.end(); pmap_it++) {
		for (pulse_it = pmap_it->second.begin(); pulse_it != pmap_it->second.end(); pulse_it++) {
			I3RecoPulse pulse(*pulse_it);
			pulse.SetTime(pulse_it->GetTime() * 1.5);
			slc_pulsemap[pmap_it->first].push_back(pulse);
		}
	}
	slc_pulsemap[key1].resize(3);
	slc_pulsemap[key2].resize(3);
	
	I3RecoPulseSeriesMapConstPtr fake_hlc;
	I3RecoPulseSeriesMap::const_iterator map_it, umap_it;
	
	I3SuperDST supi(pulsemap);
	fake_hlc = supi.Unpack();
	
	ENSURE( (bool)fake_hlc, "We got something to unpack");
	ENSURE_EQUAL( fake_hlc->size(), unsigned(2), "There's something in the map");
	
	map_it = pulsemap.find(key1);
	umap_it = fake_hlc->find(key1);
	ENSURE( map_it != pulsemap.end(), "Key 1 is in the original pulse series");
	ENSURE( umap_it != fake_hlc->end(), "Key 1 was unpacked");
	ENSURE( map_it->second.size() > 0, "There are pulses at key 1");
	ENSURE_EQUAL( umap_it->second.size(), map_it->second.size(), "All pulses were unpacked");
	
	I3RecoPulseSeries::const_iterator pit1, pit2;
	
	for (pit1 = map_it->second.begin(), pit2 = umap_it->second.begin(); 
	    (pit1 != map_it->second.end()) && (pit2 != umap_it->second.end());
	    pit1++, pit2++) {
		uint16_t l = I3SuperDST::EncodeCharge(pit1->GetCharge());
		double eps = I3SuperDST::DecodeCharge(l+1) - pit1->GetCharge();
		ENSURE_DISTANCE(pit1->GetCharge(), pit2->GetCharge(), eps,
		    "Charges match to within discretizatiion error");
	}
}

static void
TestRoundTrip(std::string filename, std::string hlc_name, std::string slc_name, unsigned nframes)
{
	using namespace boost::assign;

	boost::python::import("icecube.icetray");
	boost::python::import("icecube.dataio");

	I3Tray tray;

	std::vector<std::string> qkeys, pulsekeys;
	qkeys += hlc_name, slc_name;
	pulsekeys += hlc_name, slc_name;
	
	tray.AddModule("I3Reader", "vorleser")("FileName", filename);
	tray.AddModule("QConverter", "qify")("WritePFrame", false)("QKeys", qkeys);
	tray.AddModule("UnionMan", "EventPulses")("Keys", pulsekeys);
	tray.AddModule("I3SuperDSTTestModule", "pruefer")("Pulses", "EventPulses");
	
	tray.Execute(nframes);
}

static void
CheckDeserialization(std::string filename, std::string hlc_name,
    std::string slc_name, std::string superdst_name, unsigned nframes)
{
	using namespace boost::assign;

	boost::python::import("icecube.icetray");
	boost::python::import("icecube.dataio");

	I3Tray tray;

	std::vector<std::string> qkeys, pulsekeys;
	qkeys += hlc_name, slc_name;
	pulsekeys += hlc_name, slc_name;
	
	tray.AddModule("I3Reader", "vorleser")("FileName", filename);
	tray.AddModule("QConverter", "qify")("WritePFrame", false)("QKeys", qkeys);
	tray.AddModule("UnionMan", "EventPulses")("Keys", pulsekeys);
	tray.AddModule("I3SuperDSTTestModule", "pruefer")("Pulses", "EventPulses")("SuperDST", superdst_name)("Version", 0);
	
	tray.Execute(nframes);
}

static fs::path
GetDataDir()
{
	namespace fs = boost::filesystem;
	
	const std::string I3_TESTDATA(getenv("I3_TESTDATA"));
	fs::path data_dir(I3_TESTDATA + "/superdst");
	
	ENSURE(fs::exists(data_dir), "Directory "
	    "'$I3_TESTDATA/superdst' doesn't exist.");
	
	return (data_dir);
}

TEST(RoundTripper)
{
	namespace fs = boost::filesystem;

	fs::path data_dir = GetDataDir();
	
	/* Pulses extracted from 1.5 minutes of IC79 data from November 1, 2010 */
	fs::path test_data_ic79("superdst_test_pulses_Run00116821.i3.gz");

	ENSURE(fs::exists(data_dir/test_data_ic79), "File 'superdst_test_pulses_Run00116821.i3.gz' exists.");

	TestRoundTrip((data_dir/test_data_ic79).string(),
	    "InIceRawData_HLC_MergedPulses", "InIceRawData_SLC_FADCPulses", 300);
	
	/* Events with extremely large pulses from November 1, 2010 */
	fs::path test_data_ic79_zorchers("superdst_test_zorchers_Run00116821.i3.gz");
	
	ENSURE(fs::exists(data_dir/test_data_ic79_zorchers), "File 'superdst_test_zorchers_Run00116821.i3.gz' exists.");
	
	TestRoundTrip((data_dir/test_data_ic79_zorchers).string(),
	    "InIceRawData_HLC_MergedPulses", "InIceRawData_SLC_FADCPulses", 300);
}

TEST(Deserialize_32bit)
{
	namespace fs = boost::filesystem;

	fs::path data_dir = GetDataDir();
	
	/* 
	 * Pulses extracted from a few seconds of IC79 data from November 15, 2010,
	 * processed and compressed on RHEL_5.0_ia32.
	 */
	fs::path test_data("test_data_1115_ia32.i3.gz");
	
	ENSURE(fs::exists(data_dir/test_data), "File 'test_data_1115_ia32.i3.gz' exists.");
	
	CheckDeserialization((data_dir/test_data).string(),
	    "InIceRawData_HLC_MergedPulses", "InIceRawData_SLC_FADCPulses",
	    "I3SuperDST", 300);
}

TEST(Deserialize_64bit)
{
	namespace fs = boost::filesystem;

	fs::path data_dir = GetDataDir();
	
	/* 
	 * Pulses extracted from a few seconds of IC79 data from November 15, 2010,
	 * processed and compressed on RHEL_4.0_amd64.
	 */
	fs::path test_data("test_data_1115_amd64.i3.gz");
	
	ENSURE(fs::exists(data_dir/test_data), "File 'test_data_1115_amd64.i3.gz' exists.");
	
	CheckDeserialization((data_dir/test_data).string(),
	    "InIceRawData_HLC_MergedPulses", "InIceRawData_SLC_FADCPulses",
	    "I3SuperDST", 300);
}
