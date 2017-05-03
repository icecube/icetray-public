
Don't write namespace **using** in a header file or before and **#include**
---------------------------------------------------------------------------

Namespace **using** are for your convenience, not for you to inflict on others :
Never write a **using** declaration or a **using** directive before an **include** directive.

The only reason to use "using namespace" is to reduce typing.  It, unfortunately,
reduces readability.  I would recommend not using "namespace using" **ever**.

To save typing *prefer* :
 - Rename the namespace to reduce typing, e.g. instead of ``using namespace boost::python;`` prefer ``namespace bp = boost::python;``.  This makes it easier for someone reading your code to see where things come from.
 - Fully qualify the function or class, e.g. ``using boost::python::class_;``
 
Both are a little extra typing, but it makes it much easier for others to understand
and debug your code.

