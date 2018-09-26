#ifndef I3_SHOVEL_APPLICATION_H
#define I3_SHOVEL_APPLICATION_H

#include "scripting/embed.h"

#include <icetray/I3Logging.h>
#include <QApplication>
#include <QStringList>
#include <QString>
#include <QTemporaryFile>
#include <QProcess>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <string>

class FileService;
class ShovelMainWindow;

class SteamshovelApp : public QApplication
{
	Q_OBJECT;
	Q_PROPERTY(QString secret READ getSecret WRITE setSecret);
	Q_PROPERTY(QStringList windowNames READ getWindowNames);

	scripting::PyConsole& console_;
	std::vector< ShovelMainWindow* > windows_;
	boost::scoped_ptr<FileService> files_;
	QTemporaryFile startup_script_;
	Q_PID logviewer_pid_;

protected Q_SLOTS:
	void runStartupScripts();

public Q_SLOTS:
	void startLogViewer();

public:
	SteamshovelApp( int& argc, char* argv[],
	                scripting::PyConsole& context );
	~SteamshovelApp();

	void init( const std::vector<std::string>&, bool );

	static SteamshovelApp* instance() {
		return dynamic_cast<SteamshovelApp*>(QApplication::instance());
	}

	virtual bool notify( QObject* receiver, QEvent* event );

	FileService& files(){ return *files_; }
	scripting::PyConsole& console(){ return console_; }

	QString getSecret() const;
	void setSecret( QString );

	ShovelMainWindow* newWindow( bool show = true );

	QStringList getWindowNames() const;

public Q_SLOTS:
	void quit( int return_code = 0 );

Q_SIGNALS:
	bool extras( bool );
};
#endif /* I3_SHOVEL_APPLICATION_H */
