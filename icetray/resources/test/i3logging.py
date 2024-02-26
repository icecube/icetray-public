#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""
Test i3logger
"""

from icecube import icetray


def main():
    lvl = icetray.get_log_level_for_unit('blah')
    if lvl != icetray.I3LogLevel.LOG_INFO and lvl != icetray.I3LogLevel.LOG_NOTICE:
        raise Exception('default level is not INFO or NOTICE')

    log = icetray.I3Logger()

    gl = log.global_logger
    if not isinstance(gl, icetray.I3Logger):
        raise Exception('global logger is not an I3Logger')

    try:
        log.log(icetray.I3LogLevel.LOG_INFO, 'blah', 'file', 123, 'func', 'message')
    except NotImplementedError:
        pass
    else:
        raise Exception('I3LoggerBase should not be able to log')
    gl.log(icetray.I3LogLevel.LOG_INFO, 'blah', 'file', 123, 'func', 'message')

    lvl = log.get_level_for_unit('blah')
    if lvl != icetray.I3LogLevel.LOG_INFO and lvl != icetray.I3LogLevel.LOG_NOTICE:
        raise Exception('default level is not INFO')

    log.set_level_for_unit('blah2', icetray.I3LogLevel.LOG_WARN)
    lvl = log.get_level_for_unit('blah2')
    if lvl != icetray.I3LogLevel.LOG_WARN:
        raise Exception('cannot set log level to WARN')

    log.set_level(icetray.I3LogLevel.LOG_ERROR)
    lvl = log.get_level_for_unit('blah3')
    if lvl != icetray.I3LogLevel.LOG_ERROR:
        raise Exception('cannot set log level to ERROR')


if __name__ == '__main__':
    main()
