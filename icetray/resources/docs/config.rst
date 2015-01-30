.. _icetray-config:

icetray-config
--------------
Get the flags necessary to compile and link code using icetray and icetray projects. 

icetray-config takes as arguments the names of all of the projects which the user wishes to use. Projects transitively required by explicitly listed projects may be omitted however. For example, `icetray-config icetray dataio` and `icetray-config dataio` produce the same output, as the latter deduces that the icetray project is needed by the dataio project and automatically includes it (and does the same for the dataclasses and interfaces projects which are also needed by dataio). 

If flags for compiling and linking are necessary separately, they can be obtained by passing the --cflags (or -compile) and --ldflags (or -link) flags, respectively, to icetray-config. 

Usage (within the env-shell):

 % $(CXX) `icetray-config dataio` my_program.cxx -o my_program

Help:

 % ./env-shell.sh icetray-config --help