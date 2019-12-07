#include <I3Test.h>
#include <icetray/open.h>

#include <boost/config.hpp>
#if defined( BOOST_NO_STDC_NAMESPACE )
namespace std{
    using ::remove;
}
#endif

TEST_GROUP(CompressedIO);

void empty_file(const std::string& filepath){
	{
		boost::iostreams::filtering_ostream os;
		I3::dataio::open(os,filepath);
		//write nothing, let the file close
	}
	{
		boost::iostreams::filtering_istream is;
		I3::dataio::open(is,filepath);
		ENSURE(is.good());
		char c;
		is >> c;
		ENSURE(is.eof());
	}
	std::remove(filepath.c_str());
}

void single_int(const std::string& filepath){
	const int64_t a=-137724232832;
	{
		boost::iostreams::filtering_ostream os;
		I3::dataio::open(os,filepath);
		os << a;
	}
	{
		boost::iostreams::filtering_istream is;
		I3::dataio::open(is,filepath);
		ENSURE(is.good());
		int64_t b;
		is >> b;
		ENSURE(!is.fail());
		ENSURE(!is.bad());
		ENSURE_EQUAL(b,a);
	}
	std::remove(filepath.c_str());
}

void single_string(const std::string& filepath){
	const std::string s="isovoluminal";
	{
		boost::iostreams::filtering_ostream os;
		I3::dataio::open(os,filepath);
		os << s;
	}
	{
		boost::iostreams::filtering_istream is;
		I3::dataio::open(is,filepath);
		ENSURE(is.good());
		std::string t;
		is >> t;
		ENSURE(!is.fail());
		ENSURE(!is.bad());
		ENSURE_EQUAL(t,s);
	}
	std::remove(filepath.c_str());
}

void lots_of_data(const std::string& filepath){
	{
		boost::iostreams::filtering_ostream os;
		I3::dataio::open(os,filepath);
		for(unsigned int i=0; i<10000; i++){
			for(char c='!'; c<127; c++)
				os << c;
		}
	}
	{
		boost::iostreams::filtering_istream is;
		I3::dataio::open(is,filepath);
		ENSURE(is.good());
		for(unsigned int i=0; i<10000; i++){
			for(char c='!'; c<127; c++){
				char d=0;
				is >> d;
				ENSURE_EQUAL(d,c);
			}
		}
	}
	std::remove(filepath.c_str());
}

void test_format(const std::string& suffix){
        empty_file(I3Test::testfile("compression_test_empty"+suffix));
        single_int(I3Test::testfile("compression_test_int"+suffix));
	single_string(I3Test::testfile("compression_test_str"+suffix));
	lots_of_data(I3Test::testfile("compression_test_big"+suffix));
}

TEST(plain){ test_format(".txt"); }
TEST(gzip){ test_format(".i3.gz"); }
TEST(bzip2){ test_format(".i3.bz2"); }
TEST(zstd){ test_format(".i3.zst"); }
