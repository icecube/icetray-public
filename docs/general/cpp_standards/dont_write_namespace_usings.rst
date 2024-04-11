.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. highlight:: cpp

Don't write namespace **using** in a header file or before and **#include**
---------------------------------------------------------------------------

"Namespace using" is for your convenience, not for you to inflict on others.
Never write a :code:`using` declaration or a :code:`using` directive before an :code:`#include` directive.

The only reason to use :code:`using namespace` is to reduce typing.  It, unfortunately,
reduces readability.  I would recommend not using it **ever**.

To save typing *prefer* :

 - Rename the namespace to reduce typing, e.g. instead of :code:`using namespace boost::python;` prefer :code:`namespace bp = boost::python;`.
   This makes it easier for someone reading your code to see where things come from.
 - Fully qualify the function or class, e.g. :code:`using boost::python::class_;`

Both are a little extra typing, but it makes it much easier for others to understand
and debug your code.
