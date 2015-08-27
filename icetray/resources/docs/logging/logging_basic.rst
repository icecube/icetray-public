IceTray Logging Quick Reference
===============================

Log Levels in order of increasing verbosity and severity:

- trace
- debug
- info
- notice
- warn
- error
- fatal

Trace and debug are compiled out in release mode.  The default in debug
mode is info; the default in release mode is warn.

Functions
---------

* log_* - (e.g. log_trace) allows %d like syntax.
* log_*_stream() - (e.g. log_trace_stream) allows stream i/o syntax.
* i3_assert() - logs a message and asserts.
* i3_debug_assert() - debug log and assert.

Python Logging Example
----------------------

Can connect to python logging.  Makes lots of options available:

- send to syslog
- send to sql db
- lots of advanced adjustments

Example::

    # log an error
    icetray.logging.log_error('an error')
    
    # set default logging level
    icetray.logging.set_level(icetray.I3LogLevel.LOG_FATAL)
    icetray.logging.set_level('FATAL')
    icetray.set_log_level(icetray.I3LogLevel.LOG_FATAL)
    icetray.set_log_level('FATAL') # should work as of 2014-01-23
    
    # rotating files
    icetray.set_logger(icetray.logging.rotating_files)
    
    # syslog
    icetray.set_logger(icetray.logging.syslog)
    
    # console
    icetray.set_logger(icetray.logging.console)
    
    # set the unit
    icetray.logging.log_error('an error','my module')
    
    # set the level for the unit
    icetray.set_log_level_for_unit('my module',icetray.I3LogLevel.LOG_FATAL)
    
    
