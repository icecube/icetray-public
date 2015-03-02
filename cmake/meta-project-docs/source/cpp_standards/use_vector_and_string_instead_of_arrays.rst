
Use std::vector and std::string instead of arrays.
--------------------------------------------------

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


There's a decent reference on strings, vectors, and other STL stuff at
http://www.cppreference.com/cppstring/.


