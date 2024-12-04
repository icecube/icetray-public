// SPDX-FileCopyrightText: 2024 The IceTray Contributors
//
// SPDX-License-Identifier: BSD-2-Clause

//
// gil_holder.hpp
//
// Make a Python callback threadsafe by acquiring the
// Global Interpreter Lock and releasing it at destruction.
//

#ifndef ICETRAY_PYTHON_GIL_HOLDER_HPP_INCLUDED
#define ICETRAY_PYTHON_GIL_HOLDER_HPP_INCLUDED

#include "pybindings.hpp"
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

class restore_exceptions {
public:
	inline restore_exceptions() {
#if PY_VERSION_HEX < 0x030c0000
		PyErr_Fetch(&current_exc.type, &current_exc.value, &current_exc.traceback);
		PyErr_NormalizeException(&current_exc.type, &current_exc.value, &current_exc.traceback);
#else
		current_exc = PyErr_GetRaisedException();
#endif
	}
	/// @brief Restore the current exception
	inline void set_raised_exception() {
#if PY_VERSION_HEX < 0x030c0000
		if (current_exc.value != nullptr) {
			PyObject *type, *value, *traceback;
			PyErr_Fetch(&type, &value, &traceback);
			PyErr_NormalizeException(&type, &value, &traceback);
			PyException_SetContext(value, current_exc.value);
			current_exc.value = value;
			current_exc.type = type;
			current_exc.traceback = traceback;
		}
#else
		if (current_exc != nullptr) {
			PyObject *new_exc = PyErr_GetRaisedException();
			PyException_SetContext(new_exc, current_exc);
			current_exc = new_exc;
		}
#endif
	}
	inline ~restore_exceptions() {
#if PY_VERSION_HEX < 0x030c0000
		if (current_exc.value != nullptr)
			PyErr_Restore(current_exc.type, current_exc.value, current_exc.traceback);
#else
		if (current_exc != nullptr)
			PyErr_SetRaisedException(current_exc);
#endif
	}
	void clear() {
#if PY_VERSION_HEX < 0x030c0000
		if (current_exc.type != nullptr) {
			Py_DECREF(current_exc.type);
			current_exc.type = nullptr;
		}
		if (current_exc.value != nullptr) {
			Py_DECREF(current_exc.value);
			current_exc.value = nullptr;
		}
		if (current_exc.traceback != nullptr) {
			Py_DECREF(current_exc.traceback);
			current_exc.traceback = nullptr;
		}
#else
		if (current_exc != nullptr) {
			Py_DECREF(current_exc);
			current_exc = nullptr;
		}
#endif
	}
private:
#if PY_VERSION_HEX < 0x030c0000
	struct { PyObject *type, *value, *traceback; } current_exc;
#else
	PyObject *current_exc;
#endif
};

class allow_threads {
public:
	inline allow_threads() {
		thread_state = PyEval_SaveThread();
	}
	inline ~allow_threads() {
		PyEval_RestoreThread(thread_state);
	}
private:
	PyThreadState *thread_state;
};

}}}

#endif // ICETRAY_PYTHON_GIL_HOLDER_HPP_INCLUDED

