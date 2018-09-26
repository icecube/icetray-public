#ifndef I3_SHOVEL_LOGGER_H
#define I3_SHOVEL_LOGGER_H

#include <icetray/I3SimpleLoggers.h>
#include <QFile>
#include <QString>
#include <QMutex>

class ShovelLogger: public I3PrintfLogger {
	QFile file_;
	QMutex mutex_;
	bool auto_start_viewer_;

public:
	ShovelLogger();

	virtual void Log( I3LogLevel level,
	                  const std::string &unit,
	                  const std::string &file, int line,
	                  const std::string &func,
	                  const std::string &message );

	QString fileName() const { return file_.fileName(); }

	bool autoStartViewer() const { return auto_start_viewer_; }
	void setAutoStartViewer(bool on) { auto_start_viewer_ = on; }

	void exitMarker();
};

#endif
