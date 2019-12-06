
import logging, string, traceback
from icecube.icetray import I3Logger, I3LogLevel

class LoggingBridge(I3Logger):
	pylevels = {
		I3LogLevel.LOG_TRACE  : 5,
		I3LogLevel.LOG_DEBUG  : logging.DEBUG,
		I3LogLevel.LOG_INFO   : logging.INFO,
		I3LogLevel.LOG_NOTICE : 25,
		I3LogLevel.LOG_WARN   : logging.WARNING,
		I3LogLevel.LOG_ERROR  : logging.ERROR,
		I3LogLevel.LOG_FATAL  : logging.CRITICAL,
	}
	i3levels = dict([(v, k) for k, v in pylevels.items()])
	def __init__(self):
		I3Logger.__init__(self)
		self.getLogger("").setLevel(logging.INFO)
	def getLogger(self, unit):
		if len(unit) > 0:
			name = "icecube.%s" % unit
		else:
			name = "icecube"
		return logging.getLogger(name)
	def log(self, level, unit, file, line, func, msg):
		logger = self.getLogger(unit)
		if logger.isEnabledFor(self.pylevels[level]):
			record = logging.LogRecord(logger.name, self.pylevels[level], file, line, msg, tuple(), None, None)
			logger.handle(record)
	def get_level_for_unit(self, unit):
		return self.i3levels.get(self.getLogger(unit).getEffectiveLevel(), I3LogLevel.LOG_FATAL)
	def set_level_for_unit(self, unit, level):
		self.getLogger(unit).setLevel(self.pylevels[level])
	def set_level(self, level):
		self.getLogger("").setLevel(self.pylevels[level])

class ColorFormatter(logging.Formatter):
	def format(self, record):
		record.message = record.getMessage()
		if "%(asctime)" in self._fmt:
			record.asctime = self.formatTime(record, self.datefmt)
		d = dict(record.__dict__)
		if record.levelname in ("CRITICAL", "ERROR"):
			d['levelname'] = "\x1b[1;31m %s \x1b[0m" % d['levelname']
		d['filename'] = "\x1b[1m%s\x1b[0m" % d['filename']
			
		s = self._fmt % d
		return "\x1b[1m%s\x1b[0m" % s

BASIC_FORMAT = "%(filename)s:%(lineno)s %(levelname)s: %(message)s"

def _setup(format=BASIC_FORMAT):
	logging.addLevelName(5, 'TRACE')
	logging.basicConfig(format=format)
	I3Logger.global_logger = LoggingBridge()

def console(colors=True):
	import sys
	from os import isatty
	_setup()
	if colors and isatty(sys.stderr.fileno()):
		logging.root.handlers[0].setFormatter(ColorFormatter(BASIC_FORMAT))
	else:
		logging.root.handlers[0].setFormatter(logging.Formatter(BASIC_FORMAT))

def rotating_files(filename, maxBytes=0, backupCount=0):
	from logging.handlers import RotatingFileHandler
	_setup()
	handler = RotatingFileHandler(filename, maxBytes=maxBytes, backupCount=backupCount)
	handler.setFormatter(logging.Formatter("[%(asctime)s] "+BASIC_FORMAT))
	logging._acquireLock()
	logging.root.handlers = list()
	logging.root.addHandler(handler)
	logging._releaseLock()

def syslog():
	from logging.handlers import SysLogHandler
	_setup()
	handler = SysLogHandler()
	handler.setFormatter(logging.Formatter("[%(asctime)s] "+BASIC_FORMAT))
	logging._acquireLock()
	logging.root.handlers = list()
	logging.root.addHandler(handler)
	logging._releaseLock()

def _translate_level(name):
	if isinstance(name, I3LogLevel):
		return name
	elif hasattr(I3LogLevel, 'LOG_'+name.upper()):
		return getattr(I3LogLevel, 'LOG_'+name.upper())
	else:
		raise ValueError("Unknown logging level '%s'" % name)

def set_level(level):
	"""
	Set the global logging level.
	
	:param level: the log level. This may also be specified as a string.

	Examples::
		icetray.logging.set_level(icetray.logging.I3LogLevel.LOG_INFO)
		icetray.logging.set_level('INFO')
	"""
	I3Logger.global_logger.set_level(_translate_level(level))

def set_level_for_unit(unit, level):
	"""
	Set the logging level for a specific logging unit.
	
	:param level: the log level. This may also be specified as a string.

	Examples::
		icetray.logging.set_level_for_unit('I3Reader', icetray.logging.I3LogLevel.LOG_TRACE)
		icetray.logging.set_level('I3Reader', 'TRACE')
	"""
	I3Logger.global_logger.set_level_for_unit(unit, _translate_level(level))

def log_trace(message, unit="Python"):
	tb = traceback.extract_stack(limit=2)[0]
	I3Logger.global_logger.log(I3LogLevel.LOG_TRACE, unit, tb[0], tb[1],
	    tb[2], message)

def log_debug(message, unit="Python"):
	tb = traceback.extract_stack(limit=2)[0]
	I3Logger.global_logger.log(I3LogLevel.LOG_DEBUG, unit, tb[0], tb[1],
	    tb[2], message)

def log_info(message, unit="Python"):
	tb = traceback.extract_stack(limit=2)[0]
	I3Logger.global_logger.log(I3LogLevel.LOG_INFO, unit, tb[0], tb[1],
	    tb[2], message)

def log_notice(message, unit="Python"):
	tb = traceback.extract_stack(limit=2)[0]
	I3Logger.global_logger.log(I3LogLevel.LOG_NOTICE, unit, tb[0], tb[1],
	    tb[2], message)

def log_warn(message, unit="Python"):
	tb = traceback.extract_stack(limit=2)[0]
	I3Logger.global_logger.log(I3LogLevel.LOG_WARN, unit, tb[0], tb[1],
	    tb[2], message)

def log_error(message, unit="Python"):
	tb = traceback.extract_stack(limit=2)[0]
	I3Logger.global_logger.log(I3LogLevel.LOG_ERROR, unit, tb[0], tb[1],
	    tb[2], message)

def log_fatal(message, unit="Python"):
	tb = traceback.extract_stack(limit=2)[0]
	I3Logger.global_logger.log(I3LogLevel.LOG_FATAL, unit, tb[0], tb[1],
	    tb[2], message)
	raise RuntimeError(message + " (in " + tb[2] + ")")

