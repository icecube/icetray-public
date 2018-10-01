//
// gil_holder.hpp
//
// Make a Python callback threadsafe by acquiring the
// Global Interpreter Lock and releasing it at destruction.
//

#ifndef ICETRAY_PYTHON_GIL_HOLDER_HPP_INCLUDED
#define ICETRAY_PYTHON_GIL_HOLDER_HPP_INCLUDED

namespace boost { namespace python { namespace detail {

class gil_holder {
public:
	inline gil_holder() {
		gil_state = PyGILState_Ensure();
	}
	inline ~gil_holder() {
		PyGILState_Release(gil_state);
	}
private:
	PyGILState_STATE gil_state;
};    

}}}

#endif // ICETRAY_PYTHON_GIL_HOLDER_HPP_INCLUDED

