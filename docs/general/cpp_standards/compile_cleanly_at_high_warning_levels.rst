Compile cleanly at high warning levels
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The build system will specify ``-Wall``, a reasonable set of warnings, by
default.  If you don't see any complaints from the compiler, you're
golden.  If you do, pay attention to them.  The compiler is your
friend.  The normal state of affairs must be that the code compiles
without warning, otherwise people will start to ignore the warnings
and we will end up spending lots of time chasing bugs that were
actually very easy to find: the compiler was telling us about them all
the time.

This also makes our coding standard much much shorter.  For instance
"Dont return a pointer or reference to a temporary", is covered under
this item, because the compiler will flag this for you with all
warnings.

