.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

.. highlight:: cpp

Use vector (and string::c_str) to exchange data with non-C++ APIs.
------------------------------------------------------------------

If you need to get a :code:`const char*` out of a  :code:`string` so that
you can pass it to a C function, use the function :code:`c_str`::

 void takes_a_c_style_string(const char*);
 void takes_an_stl_string(const string&);
 string cpp_string;
 cpp_string = "something";

 takes_an_stl_string(cpp_string);             // OK
 takes_a_c_style_string(cpp_string);          // error, string doesn't convert to const char*
 takes_a_c_style_string(cpp_string.c_str());  // ok, explicitly convert it
 takes_an_stl_string("some string literal");  // ok, const char* does convert to string
