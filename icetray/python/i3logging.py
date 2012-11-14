
import logging, string
from icecube.icetray import I3Logger, I3LogLevel

class LoggingBridge(I3Logger):
	pylevels = {
		I3LogLevel.LOG_TRACE : 5,
		I3LogLevel.LOG_DEBUG : logging.DEBUG,
		I3LogLevel.LOG_INFO  : logging.INFO,
		I3LogLevel.LOG_WARN  : logging.WARNING,
		I3LogLevel.LOG_ERROR : logging.ERROR,
		I3LogLevel.LOG_FATAL : logging.CRITICAL,
	}
	i3levels = dict([(v, k) for k, v in pylevels.iteritems()])
	def getLogger(self, unit):
		if len(unit) > 0:
			name = "icecube.%s" % unit
		else:
			name = "icecube"
		return logging.getLogger(name)
	def log(self, level, unit, file, line, func, msg):
		logger = self.getLogger(unit)
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
		if string.find(self._fmt,"%(asctime)") >= 0:
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
	
