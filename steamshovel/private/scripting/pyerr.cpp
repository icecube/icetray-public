#include "gil.h"
#include "pyerr.h"
#include <ostream>

namespace { 
  using namespace boost::python;
  list format_value( handle<> hexc, handle<> hval ){
    object exctype(hexc);
    tuple value(hval);
    list ls;
    object v1 = value[1];
    ls.append( str(v1[0]) + str(":") + str(v1[1]) + str(":") + str(v1[2]) + str("\n") + v1[3] );
    ls.append( exctype.attr("__name__")  );
    ls.append( str(": ") );
    ls.append( value[0] );
    ls.append( str("\n") );
    return ls;
  }
}

std::ostream& operator<<(std::ostream& os, boost::python::error_already_set& e) {
  using namespace boost::python;
  scripting::ScopedGIL gil;

  PyObject *exc, *val, *tb;
  PyErr_Fetch(&exc, &val, &tb);
  if( !exc ) return os;

  handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));

  object traceback = import("traceback");
  object lines;
  if( !tb ){
      if( val && PyTuple_Check( val ) ){
        lines = format_value( hexc, hval );
      }else{
        object format_exception_only = traceback.attr("format_exception_only");
        lines = format_exception_only( hexc, hval );
      }
  }else{
      if( val && PyTuple_Check( val ) ){
        lines = format_value( hexc, hval );
      }else{
        object format_exception = traceback.attr("format_exception");
        lines = format_exception(hexc, hval, htb);       
      }
  }

  object s = str("").join(lines).strip();
  os << extract<const char*>(s)();

  // PyErr_Fetch clears the error state, in case it
  // needs to be restored, use this line:
  //   PyErr_Restore(exc, val, tb);
  return os;
}
