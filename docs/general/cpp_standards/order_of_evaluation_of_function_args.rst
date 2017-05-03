Don't write code that depends on the order of evaluation of function arguments
------------------------------------------------------------------------------

The order in which function arguments are evaluated is undefined, so don't
rely on it.

If using iterators::

  foo(++i,++i)
  
it is undefined which argument will have which value.

It becomes even more dangerous if when evaluating an argument an
exception is raised. If you are creating a new object as an
argument it may or may not be created, leading to a memory leak.

So, always use named objects or constants to enforce order of evaluation.
