#include <icetray/scratch.h>
#include <icetray/I3Logging.h>

#include <cstdlib> //for getenv
#include <cstring> //for strlen
#include <sys/errno.h> //for errno, EEXIST
#include <sys/stat.h> //for mkdir
#include <unistd.h> //for getcwd, getuid
#include <pwd.h> //for getpwuid
#include <sys/param.h> //path length limits

namespace I3 {
	namespace dataio {
		namespace {
			std::string scratch_dir;
			
			bool find_or_make_dir(const std::string& path){
				int result=mkdir(path.c_str(),S_IRWXU);
				if(result==-1)
					result=errno;
				return(result==0 || result==EEXIST);
			}
		}
		
		std::string GetScratchDirectory(){
			//if we already have something, use that
			if(!scratch_dir.empty())
				return(scratch_dir);
			
			//try environment variables
			//TODO: for these we do no validation;
			//adding it might be more user-friendly
			char* envResult;
			envResult=getenv("_CONDOR_SCRATCH_DIR");
			if(envResult){
				scratch_dir=envResult;
				return(scratch_dir);
			}
			envResult=getenv("TMPDIR");
			if(envResult){
				scratch_dir=envResult;
				return(scratch_dir);
			}
			
			//try some somewhat common locations
			//TODO: getpwuid is discouraged on darwin but getpwuid_r is not
			//available on common (old) linux versions (like RHEL 6)
			passwd* pw=getpwuid(getuid());
			std::string username=pw->pw_name;
			{
				std::string tmpPath="/scratch/"+username;
				if(find_or_make_dir(tmpPath)){
					scratch_dir=tmpPath;
					return(scratch_dir);
				}
			}
			{
				std::string tmpPath="/global/scratch/"+username;
				if(find_or_make_dir(tmpPath)){
					scratch_dir=tmpPath;
					return(scratch_dir);
				}
			}
			
			//fall back on using the current working directory
			const unsigned int pathSize=
			#ifdef PATH_MAX //Linux
				PATH_MAX;
			#else
			#ifdef MAXPATHLEN //BSDs
				MAXPATHLEN;
			#else
				#error Bad
				4096; //Just pick something large-ish
			#endif
			#endif
			scratch_dir.resize(pathSize);
			char* cwd_res=getcwd(&scratch_dir.front(),pathSize);
			if(!cwd_res)
				log_fatal_stream("Unable to get current working directory: error "
				                 << errno);
			scratch_dir.resize(strlen(cwd_res));
			log_warn_stream("Cannot find a suitable scratch directory on this machine; "
			                << "falling back to the current working directory ("
			                << scratch_dir << ").");
			return(scratch_dir);
		}
		
		void SetScratchDirectory(std::string path){
			if(find_or_make_dir(path))
				scratch_dir=path;
			else
				log_fatal_stream("Unable to set scratch directory to " << path
				                 << ": directory could not be created or is inaccessible");
		}
		
		void UnsetScratchDirectory(){
			scratch_dir.clear();
		}
	} //namespace dataio
} //namespace I3
