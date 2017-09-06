#ifndef ICETRAY_SCRATCH_H_INCLUDED
#define ICETRAY_SCRATCH_H_INCLUDED

#include <string>

namespace I3 {
	namespace dataio {
		/**
		 * Attempt to get a path to a directory where large temporary files
		 * can be written. The order of directories which will be tried is:
		 *
		 * - The path manually set by `SetScratchDirectory`
		 * - The value of the `_CONDOR_SCRATCH_DIR` environment variable, if set
		 * - The value of the `TMPDIR` environment variable, if set
		 * - /scratch/<real user name>
		 * - /global/scratch/<real user name>
		 * - The current working directory
		 *
		 * The first directory which exists or can be created will be returned.
		 * That value will then be cached and returned on subsequent calls (so
		 * even if the current working directory was selected, calling `chdir`
		 * and then calling `GetScratchDirectory` again will continue to yield 
		 * the original directory. However, directly calling 
		 * `SetScratchDirectory` will replace the cached value, so users should 
		 * not depend on the result of this function never changing.
		 */
		std::string GetScratchDirectory();
		
		/**
		 * Manually set the path which should be returned by 
		 * `GetScratchDirectory`. This makes most sense before the first call to
		 * `GetScratchDirectory`, but is not currently forbidden at later times.
		 * \param path the scratch directory path
		 */
		void SetScratchDirectory(std::string path);
		
		/**
		 * Unsets any cached directory path, forcing the next call to
		 * `GetScratchDirectory` to recompute its value. 
		 * This function is intended primarily for testing.
		 */
		void UnsetScratchDirectory();
	}
}

#endif
