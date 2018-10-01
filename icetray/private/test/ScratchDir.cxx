#include <I3Test.h>
#include <icetray/scratch.h>
#include <cstdio> //for remove
#include <cstdlib> //for setenv
#include <unistd.h> //for getcwd, rmdir
#include <sys/param.h> //path length limits

namespace{
	std::string getcwd(){
			const unsigned int pathSize=
			#ifdef PATH_MAX //Linux
				PATH_MAX;
			#else
			#ifdef MAXPATHLEN //BSDs
				MAXPATHLEN;
			#else
				4096; //Just pick something large-ish
			#endif
			#endif
		std::string cwd;
		cwd.resize(pathSize);
		::getcwd(&cwd.front(),pathSize);
		return(cwd);
	}
	
	const std::string test_condor_dir="TEST_CONDOR_SCRATCH_DIR";
	const std::string test_tmpdir="TEST_TMPDIR";
}

TEST_GROUP(ScratchDir)

TEST(ManualOverride){
	I3::dataio::UnsetScratchDirectory();
	setenv("_CONDOR_SCRATCH_DIR",test_condor_dir.c_str(),1);
	setenv("TMPDIR",test_tmpdir.c_str(),1);
	std::string expected=getcwd()+"/MANUAL_SCRATCH_DIR";
	I3::dataio::SetScratchDirectory(expected);
	ENSURE_EQUAL(I3::dataio::GetScratchDirectory(),expected);
	rmdir(expected.c_str()); //SetScratchDirectory will have created this, so delete it
}

TEST(CONDOR_SCRATCH_DIR){
	I3::dataio::UnsetScratchDirectory();
	//if set, _CONDOR_SCRATCH_DIR overrides TMPDIR
	setenv("_CONDOR_SCRATCH_DIR",test_condor_dir.c_str(),1);
	setenv("TMPDIR",test_tmpdir.c_str(),1);
	ENSURE_EQUAL(I3::dataio::GetScratchDirectory(),test_condor_dir);
}

TEST(TMPDIR){
	I3::dataio::UnsetScratchDirectory();
	//if set, TMPDIR should be used when _CONDOR_SCRATCH_DIR is not set
	unsetenv("_CONDOR_SCRATCH_DIR");
	setenv("TMPDIR",test_tmpdir.c_str(),1);
	ENSURE_EQUAL(I3::dataio::GetScratchDirectory(),test_tmpdir);
}

//TODO: Come up with a way to test root level paths, or get past them to test
//the current directory final fallback
