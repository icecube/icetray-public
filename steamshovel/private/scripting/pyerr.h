#ifndef I3_SHOVEL_SCRIPTING_PYERR_H_
#define I3_SHOVEL_SCRIPTING_PYERR_H_

#include <iosfwd>
#include <boost/python.hpp>

/** An ostream operator for Python Exceptions.
 *  The implementation is moderately evil, because printing the
 *  exception also clears the Python error state (internally,
 *  PyErr_Fetch is used). This is usually what you want.
 */
std::ostream& operator<<(std::ostream& os, boost::python::error_already_set& e);

#endif
