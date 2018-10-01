Logging
=======

.. contents::

.. toctree::
   :maxdepth: 4

   logging/logging_basic

How to Use the Logging System
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

  **DEBUG (MyClass)**:  here is a debug message, j is 6 (**MyClass.h:14** in **void MyClass::doSomething()**)

Each of the ``printf()``-style logging functions has an ``iostreams``-style analogue that can be
useful for easily formatting non-POD types. For example, both of following logging calls: 

::
   
   #include <icetray/I3Logging.h>
  
    void doSomething() {
      OMKey key(64, 26);
      std::string name("InIceRawData")
      log_debug("Looking for OMKey(%d,%d) in %s", key.GetString(), key.GetOM(), name.c_str());
      log_debug_stream("Looking for " << key << " in " << name);
    }

would yield identical output:

    **DEBUG (MyClass)**:  Looking for OMKey(64,26) in InIceRawData (**MyClass.h:14** in **void MyClass::doSomething()**)


There are several logging levels available.  Each has a different meaning:

.. _log_fatal:
.. _log_error:
.. _log_warn:
.. _log_info:
.. _log_debug:
.. _log_trace:
.. index:: log_fatal, log_error, log_warn, log_info, log_debug, log_trace

=============   ====================================================================================
Function        Semantics
=============   ====================================================================================
``log_fatal``   Only called for fatal errors, will throw an exception and data processing will stop.
``log_error``  	Non-fatal (recoverable) error. No exception thrown.
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
Function        Example
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
  (removed by the C preprocessor) and will not be available, no
  matter how you configure your logging output.

How to Add a Custom Logging Channel to your Module
--------------------------------------------------

.. index:: SET_LOGGER
.. _SET_LOGGER:
  
You can add a logging channel just for your module, using the SET_LOGGER() macro:

I3SimpleModule.h:

.. highlight:: cpp

::

 class I3SimpleModule : public I3Module
 {

  public:

   I3SimpleModule(const I3Context& context);
   void Configure();
   void Physics(I3FramePtr frame);
   void Finish();

  private:

   std::string outputFilename_;

   SET_LOGGER("I3SimpleModule");
 };

That opens up a configurable logging channel called I3SimpleModule. This lets you configure
logging for I3SimpleModule separately than the global logging preferences. For example, you
can show log messages for I3SimpleModule at a different level (e.g. debug) than the global
preferences. In addition, "I3SimpleModule" will be printed at the beginning of every log
output and, for some logging backends, output from I3SimpleModule can be diverted to a
different destination (e.g. to its own log file). 

Configuring the logger's output
-------------------------------

Icetray logging messages can be routed to different places (e.g. the console or to a file)
by choosing a specific logger module or changing the logging threshold either for a specific
logging unit (e.g. MySimpleModule above) or the default for all logging units. Like most
aspects of Icetray, this behavior can be controlled programmatically from Python as part
of an Icetray script.

Setting the default logger
^^^^^^^^^^^^^^^^^^^^^^^^^^

There are three logging backends provided with Icetray: I3NullLogger, I3PrintfLogger, and
the Python logging facility. The logger currently in use by Icetray is accessible from 
Python at ``icetray.I3Logger.global_logger`` and can be changed by assignment to an instance
of a different logging class. The equivalent C++ routine is the global function
``SetIcetrayLogger``. The logger can be changed at any time -- although it is usually
set at the beginning of an Icetray script after the icetray library has been loaded.

I3NullLogger
____________

I3NullLogger is a logger that does not log anything. This can be useful if log messages
are never desired (although ``log_fatal()`` calls will still cause program termination).
Logging can be disabled by:

.. highlight:: python

::

  icetray.I3Logger.global_logger = icetray.I3NullLogger()

I3PrintfLogger
______________

I3PrintfLogger sends log messages to the standard error stream (usually the console),
including decorative colors unless standard error has been redirected to a file. This is
the default logging module. Error messages look like this:

  **ERROR (I3Tray):** This I3Tray has already been executed. Ignoring second call to Execute() (**I3Tray.cxx:442** in **void I3Tray::Execute()**)

The first section in bold shows the level of the log message (ERROR) and the logging unit
(I3Tray) producing the error. The next part in normal text is the message passed to, in this
case, ``log_error()``, and the final sections in bold give the file, line number, and
function from which the logging system was called.

The default logger can be set to I3PrintfLogger by:

::

  icetray.I3Logger.global_logger = icetray.I3PrintfLogger()

Python Logging
______________

Icetray also includes a bridge to the `python logging facility <http://docs.python.org/library/logging.html>`_, which contains a number
of handlers able to do things like write log messages to a rotating set of files, to the
system logging daemon, to email, or to a web server. You can also define your own.

Several convenience functions are provided for standard uses of the logging facility either
to write to standard error (like I3PrintfLogger), to rotating log files, or to the syslog
daemon.

To use python logging to the console, do:

::

  icetray.logging.console()

To use python logging to log files, do:

::

  icetray.logging.rotating_files('/path/to/filename')

To log to syslogd (e.g. the ``Console`` program on Mac OS X):

::

  icetray.logging.syslog()

More complicated things can be done if desired using the Python logging infrastructure --
see the `Python documentation <http://docs.python.org/library/logging.html>`_ for details.


In addition to configuring logging from Python, all the C++ logging functions (log_info() etc.) are available from Python under icetray.logging. These functions take two arguments: a log message and optional logging unit (the default is the "Python" unit). For example:

::

  icetray.logging.log_fatal("Things are irreparably broken!", "I3FooModule")

Setting Logging Thresholds
^^^^^^^^^^^^^^^^^^^^^^^^^^

The logging threshold is set by default to LOG_WARN for all modules (i.e. only
``log_warn()`` and more severe conditions will be logged). If you want to change
this global threshold, this can be changed by I3Logger's ``set_level()`` method.
For example, to get more information, the global log threshold can be reduced to LOG_INFO:

::

  icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)

This can also be changed on a per-logging stream level. For example, to get extremely
verbose information just from I3Tray, while leaving all other subsystems at their normal
levels:

::

  icetray.set_log_level_for_unit('I3Tray', icetray.I3LogLevel.LOG_TRACE)

