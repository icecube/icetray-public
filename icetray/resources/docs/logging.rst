Logging
=======

The IceTray framework has a built in logging system that uses
*log4cplus* (http://log4cplus.sourceforge.net/) as a base.

How to use the logging system
-----------------------------
Call logging functions like you would call ``printf()``:

.. highlight:: cpp

::

   #include <icetray/I3Logging.h>
  
    void doSomething() {
      int j=6;
      log_debug("here is a debug message, j is %d", j);
    }
  };


Will output:

.. highlight:: none

::

  MyClass.h:14  here is a debug message j is 6

There are several logging levels available.  Each has a different meaning:

.. _log_fatal:
.. _log_error:
.. _log_warn:
.. _log_info:
.. _log_debug:
.. _log_trace:
.. index:: log_trace, log_error, log_warn, log_info, log_debug, log_trace

=============   ====================================================================================
function        semantics
=============   ====================================================================================
``log_fatal``   Only called for fatal errors, will throw an exception and data processing will stop.
``log_error``  	Non-fatal (recoverable) exception. No exception thrown.
``log_warn``   	Possible error conditions approaching....
``log_info``   	Information to tell operator what's going on.
``log_debug``  	Information for system expert.
``log_trace``  	Usable in inner loops, ok if tons of output is generated at this level.
=============  	====================================================================================

The "standard" logging level is WARN.  If your module is working
happily, no output at log_warn or higher should be visible.

Here are some guidelines and example statements at each level to help
you choose the right logging level:

==============  =============================================================================
function        example
==============  =============================================================================
``log_fatal``   (will cause exit) "Unable to open input file", "No DB connection available"
``log_error``   (won't exit, but your module has a serious problem that needs attention) "Found malformed event data", "divide by zero detected in module".
``log_warn``    (non-serious errors, "non-standard" configuration detected) "OM key out of range", "<1TB disk space remains", "no AMANDA data in >5 minutes"....
``log_info``    (<< 1/event) Major transitions, new files open, "processed 1000 events" type statements,
``log_debug``   One or two lines output for each process call (per event, config, etc) per module
``log_trace``   line by line debugging is possible.
==============  =============================================================================

.. warning:: 

  If you compile in ``Release`` mode (see :ref:`CMAKE_BUILD_TYPE
  <CMAKE_BUILD_TYPE>`), log_trace and log_debug are "compiled out"
  (removed by the C preprocessor) and will not be available, not
  matter how you configure your logging output.

How to add a custom logging channel to your module
--------------------------------------------------

.. index:: SET_LOGGER
.. _SET_LOGGER:
  
You can add a logging channel just for your module, using the SET_LOGGER() macro:

I3SimpleModule.h:

.. highlight:: cpp

::

 class I3SimpleModule : public I3PhysicsModule
 {

  public:

   I3SimpleModule(const I3Context& context);
   void Configure();
   void Physics(I3Frame& frame);
   void Finish();

  private:

   string outputFilename_;

   SET_LOGGER("I3SimpleModule");
 };

That opens up a configurable logging channel called I3SimpleModule for
you to independently configure the logging level for...

Configuring the logger's output
-------------------------------

Consider the following configuration file, located either at
$I3_BUILD/log4cplus.conf, or pointed to by the environment variable
$I3LOGGING_CONFIG. (A template is provided in
icetray/resources/log4cplus.conf)

.. highlight:: sh

::

   #
   # configure the format of the output, these defaults are fine
   #
   log4cplus.appender.default=log4cplus::ConsoleAppender
   log4cplus.appender.default.layout=log4cplus::PatternLayout
   log4cplus.appender.default.layout.ConversionPattern=%F:%L %m%n
   
   #
   # set the threshold of the default "rootLogger
   #
   log4cplus.rootLogger=ERROR, default
   
   #
   # set the threshold and non-additivity of your logger.
   # if you have more classes, just copy and modify these lines.
   #
   log4cplus.logger.I3SimpleModule=DEBUG, default
   log4cplus.additivity.I3SimpleModule=false

Compilation Units
-----------------

You can specify a logger local to a file or other scope. See the
I3Logging header file to see what SET_LOGGER() provides, and provide
that. Remember that it is C++ scope rules that determine which logger
will be found.

Emacs Tricks
------------

Here is a favorite emacs trick. These log messages (file and line
error-message) are in the same format as gcc compilation errors. When
put the output of your application into an emacs buffer with
compilation minor mode enabled M-x compilation-minor-mode, emacs will
interpret the file and line numbers, and if you put the cursor on one
of these log messages and hit enter, emacs will take you, in another
window, to that line at that file.

One way to do this is: save the output of the program to a file, load
it in emacs and enable compilation-minor-mode. One can start up a
shell buffer, enable compilation-minor-mode there, and run the make in
this window. Or one can, upon invoking "compile" ( C-x v m ) respond
to the prompt Compile command: with make && ./run_my_app. This is good
for iterative work on hard bugs.

More information about configuring logging output
-------------------------------------------------

The logging subsystem configures itself via a configuration file
pointed to by the environment variable I3LOGGING_CONFIG. A sample
logging file is located at
$I3_BUILD/icetray/resources/log4cplus.conf. If you'd like to customize
your logging, you may modify this file and set your I3LOGGING_CONFIG
appropriately.

These links might help):

Log4cplus homepage 
  http://log4cplus.sourceforge.net/

