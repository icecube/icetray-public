#ifndef I3_SHOVEL_SCRIPTING_PYTHON_GIL_H
#define I3_SHOVEL_SCRIPTING_PYTHON_GIL_H

#include <boost/python.hpp>

namespace scripting{

class ScopedGIL{
private:
	PyGILState_STATE s;
public:
	ScopedGIL(){
		s = PyGILState_Ensure();
	}
	~ScopedGIL(){
		PyGILState_Release( s );
	}
};

} // namespace scripting

#endif /* I3_SHOVEL_SCRIPTING_PYTHON_GIL_H */
