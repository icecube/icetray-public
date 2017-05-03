
Declare variables as locally as possible.
-----------------------------------------

Avoid leaving variables laying around whose scope is larger than their
actual usefulness.  You shouldn't declare a variable until you have
enough information to initialize it and are ready to use it.  Don't
make variables members of your class if they can be local to one of
the classes' member functions.  Globals are the worst, only use them
when necessary.

Define each variable as locally as you can, which is usually exactly
at the point where you also have enough data to initalize it, and
immediately before its first use.   

