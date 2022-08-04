
Use std::vector and std::string instead of arrays
-------------------------------------------------

Don't use C-style const char \*'s, arrays, ``strcmp``, ``strcpy``, or
``strncmp``.  C-style arrays (``float[3]``) can be used if their size
is known at compile time.  Use STL strings instead::

 string s1, s2;

 s1 == s2; // equality comparison
 if (s1 < s2) { } // dictionary comparison

 s1 = s2; // assignment

 s1 = "heeheehee";

 s1 += s2; // concatenation

 cout << "length of s1 is " << s1.length();

Reasoning
"""""""""

Buffer overruns and related security flaws are a serious problem with
C-style code, specifically arrays, pointer arithmetic, and memory
management. Avoid this completely by using higher level C++ facilities.

Reasons to prefer C++ std:

* They manage their own memory automatically

* They have a rich interface, making complex functionality easy

* They are compatible with the C memory model

* They offer extended checking

* They are already highly optimized for most situations

Reference
"""""""""

There's a decent reference on strings, vectors, and other STL items at
https://www.cppreference.com/cppstring/.


