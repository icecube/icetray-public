#ifndef I3_SHOVEL_MAINWINDOW_H
#define I3_SHOVEL_MAINWINDOW_H

#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>

#include <QMainWindow>
#include <QString>

namespace Ui { class MainWindow; }
namespace scripting { class PyConsole; }
class MovieEngine;
class QAction;
class QActionGroup;
class QCloseEvent;
class I3FrameTableModel;
class QLabel;
class QWidget;
struct FrameInfo;

class ShovelMainWindow : public QMainWindow
{
	Q_OBJECT;
	Q_PROPERTY( int activeView READ getActiveView WRITE setActiveView );

	SET_LOGGER("ShovelMainWindow");

	Ui::MainWindow* ui;
	QActionGroup* view_group_;
	QLabel* frames_info_;
	MovieEngine* mengine_;
	I3FrameTableModel* frame_table_model_;
	bool fullscreen_warning_shown_;
	bool tv_mode_warning_shown_;
	bool session_read_;
	int focus_menu_original_size_;
	QString file_directory_;
	QString frame_save_directory_;
	QString session_directory_;

public:

	ShovelMainWindow( QString );
	~ShovelMainWindow();

	void initScripting( const scripting::PyConsole& );
	void setBatchMode( bool );

	int getActiveView() const;
	void setActiveView( int );

	Q_INVOKABLE void captureActiveView( const std::string& );

	void closeEvent( QCloseEvent* );

protected:
	void keyPressEvent( QKeyEvent* );
	void showEvent( QShowEvent* );

	void updateFocusMenu( I3FramePtr );

	QString getScenarioCode() const;
	bool loadScenarioCode(const QString& );

	void perhapsShowUpdates();

protected Q_SLOTS:
	void handleFileListChanged();
	void handleNewSegment(unsigned, unsigned, bool);
	void handleFrameChanged( I3FramePtr, unsigned, const FrameInfo& );
	void handleLockCamera( bool );
	void handleWidgetArtistAdded( QString, QWidget* );
	void handleWidgetArtistRemoved( QWidget* );

	void focusOn( QAction* );
	void handleSessionAction( QAction* );
	void handleCameraViewAction( QAction* );

	void readSession();
	void writeSession();

	// scripts should use showFullScreen() and showNormal()
	void doToggleFullscreen();

	void doNewWindow();

public Q_SLOTS:
	/* menu-driven actions */
	void doFileOpen();
	void doFrameSave();

	void doSessionOpen();
	void doSessionOpen( QString );
	void doSessionSave();
	void doSessionClear();

	void doToggleTVMode();

	void doProjectionDialog();
	void doScreenshotDialog();
	void doMovieDialog();
	void doConfigDialog();
	void doHelpDialog();

	void doSwitchView( QAction* );

	void doShellToggle( bool );

	void doExtras( bool );
};

#endif /* I3_SHOVEL_MAINWINDOW_H */
