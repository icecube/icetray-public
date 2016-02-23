gfilt, the C++ error message descrambler
========================================

:command:`gfilt`, from Leor Zolman at http://www.bdsoft.com is a handy
utility that reformats and descrambles error messages. For instance
this program:

.. code-block:: cpp

   #include <map>
   #include <string>

   int main()
   {
       using namespace std;

       multimap<string, int> mm;
       mm.insert(0);

       return 0;
   }


generates the following error message with gcc:

.. code-block:: none

   foo.cxx: In function 'int main()':
   foo.cxx:9: error: no matching function for call to 'std::multimap<std::string, int, std::less<std::string>, std::allocator<std::pair<const std::string, int> > >::insert(int)'
   /usr/include/gcc/darwin/4.0/c++/bits/stl_multimap.h:347: note: candidates are: typename std::_Rb_tree<_Key, std::pair<const _Key, _Tp>, std::_Select1st<std::pair<const _Key, _Tp> >, _Compare, _Alloc>::iterator std::multimap<_Key, _Tp, _Compare, _Alloc>::insert(const std::pair<const _Key, _Tp>&) [with _Key = std::string, _Tp = int, _Compare = std::less<std::string>, _Alloc = std::allocator<std::pair<const std::string, int> >]
   /usr/include/gcc/darwin/4.0/c++/bits/stl_multimap.h:371: note:                 typename std::_Rb_tree<_Key, std::pair<const _Key, _Tp>, std::_Select1st<std::pair<const _Key, _Tp> >, _Compare, _Alloc>::iterator std::multimap<_Key, _Tp, _Compare, _Alloc>::insert(typename std::_Rb_tree<_Key, std::pair<const _Key, _Tp>, std::_Select1st<std::pair<const _Key, _Tp> >, _Compare, _Alloc>::iterator, const std::pair<const _Key, _Tp>&) [with _Key = std::string, _Tp = int, _Compare = std::less<std::string>, _Alloc = std::allocator<std::pair<const std::string, int> >]

**gfilt** pares this down to::

  BD Software STL Message Decryptor v2.47 for gcc
  foo.cxx: In function 'int main()':
  foo.cxx:9: error: No match for 'multimap<string, int>::insert(int)'

Which is quite an improvement. All those colons and angle brackets can
make one a bit dizzy.

It is packaged with our cmake and enabled by default.  Turn off the cmake 
variable :envvar:`USE_GFILT` to disable it.

For the vast majority of cases, it works well, but if it is on and you
really can't make sense of an error, try turning it off. It might be
supressing something important.
