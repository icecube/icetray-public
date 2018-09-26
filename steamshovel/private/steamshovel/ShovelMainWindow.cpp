#include "ShovelMainWindow.h"
#include "moc_ShovelMainWindow.cpp"
#include "ui_fullscreen_message.h"
#include "ui_tvmode_message.h"
#include "ui_update_dialog.h"
#include "ui_shovelmainwindow.h"

#include "SteamshovelApp.h"
#include "I3FrameTableModel.h"
#include "FramePicker.h"
#include "FileService.h"
#include "FilterWidget.h"
#include "ProjectionDialog.h"
#include "MovieDialog.h"
#include "MovieEngine.h"
#include "SaveFrameDialog.h"
#include "ConfigDialog.h"
#include "scripting/qmeta.h"
#include "scripting/data.h"
#include "scripting/gil.h"
#include "scripting/pyerr.h"
#include "scripting/embed.h"
#include "shovelart/Artist.h"
#include "shovelart/Scenario.h"
#include "shovelart/graphicsmath.h"

#include <dataclasses/I3Constants.h>
#include <icetray/I3Frame.h>

#include <iostream>
#include <stdexcept>
#include <set>

#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QSettings>
#include <QPixmap>
#include <QMessageBox>
#include <QActionGroup>
#include <QStatusBar>
#include <QCloseEvent>

#include <boost/foreach.hpp>

class SignalBlocker {
	QObject* sender_;
	QObject* receiver_;
	const char* signal_;
	const char* slot_;
	Qt::ConnectionType type_;
public:
	SignalBlocker( QObject* sender, const char* signal,
	               QObject* receiver, const char* slot,
	               Qt::ConnectionType type = Qt::AutoConnection ):
		sender_(sender), receiver_(receiver),
		signal_(signal), slot_(slot), type_(type)
	{
		QObject::disconnect( sender_, signal_, receiver_, slot_ );
	}

	SignalBlocker( QObject* sender, const char* signal, const char* slot,
	               Qt::ConnectionType type = Qt::AutoConnection ):
		sender_(sender), receiver_(sender),
		signal_(signal), slot_(slot), type_(type)
	{
		QObject::disconnect( sender_, signal_, receiver_, slot_ );
	}

	~SignalBlocker()
	{
		QObject::connect( sender_, signal_, receiver_, slot_, type_ );
	}
};

ShovelMainWindow::ShovelMainWindow( QString name ):
	QMainWindow(),
	ui( new Ui::MainWindow ),
	view_group_( new QActionGroup( this ) ),
	frames_info_( new QLabel( this ) ),
	frame_table_model_( new I3FrameTableModel( this ) ),
	fullscreen_warning_shown_(false),
	tv_mode_warning_shown_(false),
	session_read_(false)
{
	setObjectName( name );
	ui->setupUi( this );
	mengine_ = new MovieEngine( this, ui->gl_widget );
	FileService* fileService = &SteamshovelApp::instance()->files();
	statusBar()->addPermanentWidget( frames_info_ );

	connect( fileService,
	         SIGNAL(frameChanged(I3FramePtr, unsigned, const FrameInfo&)),
	         SLOT(handleFrameChanged(I3FramePtr, unsigned, const FrameInfo&)) );
	connect( fileService,
	         SIGNAL(newSegment(unsigned, unsigned, bool)),
	         SLOT(handleNewSegment(unsigned, unsigned, bool)) );
	connect( fileService,
	         SIGNAL(fileListChanged()),
	         SLOT(handleFileListChanged()) );
	handleFileListChanged();

	connect( ui->gl_widget,
	         SIGNAL(statusMessage(QString, int)),
	         statusBar(),
	         SLOT(showMessage(QString, int)) );

	// file menu
	connect( ui->actionOpen_file, SIGNAL(triggered()),
	         SLOT(doFileOpen()) );
	connect( ui->actionSave_frame, SIGNAL(triggered()),
	         SLOT(doFrameSave()) );
	connect( ui->actionReload_all,
	         SIGNAL(triggered()),
	         fileService,
	         SLOT(reloadAllFiles()) );
	connect( ui->actionClose_all,
	         SIGNAL(triggered()),
	         fileService,
	         SLOT(closeAllFiles()) );
	connect( ui->actionQuit, SIGNAL(triggered()),
	         SLOT(close()) );

	// session menu
	connect( ui->menuSession, SIGNAL(triggered(QAction*)),
	         SLOT(handleSessionAction(QAction*)) );

	// window menu
	connect( ui->actionToggle_full_screen, SIGNAL(triggered()),
	         SLOT(doToggleFullscreen()) );
	connect( ui->actionToggle_TV_mode, SIGNAL(triggered()),
	         SLOT(doToggleTVMode()) );
	connect( ui->actionShow_python_prompt, SIGNAL(toggled(bool)),
	         SLOT(doShellToggle(bool)) );

	// QActionGroup makes actions mutually exclusive
	view_group_->addAction( ui->action3D_View );
	view_group_->addAction( ui->actionSpreadsheet_View );
	connect( view_group_, SIGNAL(triggered(QAction*)),
	         SLOT(doSwitchView(QAction*)) );
	connect( ui->gl_widget->getScenario(),
	         SIGNAL(widgetArtistAdded(QString, QWidget*)),
	         SLOT(handleWidgetArtistAdded(QString, QWidget*)) );
	connect( ui->gl_widget->getScenario(),
	         SIGNAL(widgetArtistRemoved(QWidget*)),
	         SLOT(handleWidgetArtistRemoved(QWidget*)) );

	connect( ui->actionScreenshot, SIGNAL(triggered()),
	         SLOT(doScreenshotDialog()) );
	connect( ui->actionProjection, SIGNAL(triggered()),
	         SLOT(doProjectionDialog()) );
	ui->actionProjection->setEnabled(ProjectionDialog::isAvailable());
	connect( ui->actionMovie, SIGNAL(triggered()),
	         SLOT(doMovieDialog()) );

	connect( ui->actionConfiguration, SIGNAL(triggered()),
	         SLOT(doConfigDialog()) );

	// view menu
	connect( ui->actionPerspective_camera, SIGNAL(triggered(bool)),
	         ui->gl_widget, SLOT(setPerspective(bool)) );
	connect( ui->gl_widget, SIGNAL(perspectiveChanged(bool)),
	         ui->actionPerspective_camera, SLOT(setChecked(bool)) );
	connect( ui->actionChange_background_color, SIGNAL(triggered()),
	         ui->gl_widget, SLOT(doSetBackgroundColor()) );
	connect( ui->menuFocus_on, SIGNAL(triggered(QAction*)),
	         this, SLOT(focusOn(QAction*)) );
	ui->actionFocusIceCube_origin->setData( QVariant( vec3d(0, 0, 0) ) );
	ui->actionFocusIceTop->setData( QVariant( vec3d(0, 0, I3Constants::zIceTop ) ) );
	const double zBedrock = -I3Constants::OriginElev;
	ui->actionFocusIceCenter->setData(QVariant(
	  vec3d(0, 0, (I3Constants::zIceTop + zBedrock)/ 2.0 ) ));
	focus_menu_original_size_ = ui->menuFocus_on->actions().size();
	connect( ui->actionReset_camera, SIGNAL(triggered()),
	         ui->gl_widget, SLOT(resetCamera()) );
	connect( ui->actionLock_camera, SIGNAL(toggled(bool)),
	         SLOT(handleLockCamera(bool)) );
	connect( ui->gl_widget, SIGNAL(cameraLockChanged(bool)),
	         ui->actionLock_camera, SLOT(setChecked(bool)) );
	QActionGroup* camera_view_group = new QActionGroup(this);
	camera_view_group->setExclusive( false );
	camera_view_group->addAction( ui->actionViewXY );
	camera_view_group->addAction( ui->actionViewYZ );
	camera_view_group->addAction( ui->actionViewXZ );
	connect( camera_view_group, SIGNAL(triggered(QAction*)),
	         SLOT(handleCameraViewAction(QAction*)) );

	connect( ui->rotationButton, SIGNAL(toggled(bool)),
	         ui->gl_widget, SLOT(toggleSpin(bool)) );

	connect( ui->action_New_Window, SIGNAL(triggered()),
			 SLOT(doNewWindow()) );

	// help menu
	connect( ui->actionHelp, SIGNAL(triggered()),
	         this, SLOT(doHelpDialog()) );
	connect( ui->actionMessageLog, SIGNAL(triggered()),
	         SteamshovelApp::instance(), SLOT(startLogViewer()) );

	// spreadsheet view
	QSortFilterProxyModel* sortm = new QSortFilterProxyModel(this);
	sortm->setSourceModel( frame_table_model_ );
	sortm->setDynamicSortFilter( true );
	ui->frame_table_view->setModel( sortm );
	ui->frame_table_view->sortByColumn( 0, Qt::AscendingOrder );
	connect( frame_table_model_, SIGNAL(modelReset()),
	         ui->frame_table_view, SLOT(resizeColumnsToContents()) );

	// timeline related connections
	connect( ui->timeline_widget,
	         SIGNAL( timeWindowChanged(const TimeWindow&) ),
	         ui->gl_widget->getScenario(),
	         SLOT( updateWidgetArtists(const TimeWindow&) ) );
	connect( ui->timeline_widget,
	         SIGNAL( timeWindowChanged(const TimeWindow&) ),
	         ui->gl_widget,
	         SLOT( setTimeWindow(const TimeWindow&) ) );
	connect( ui->timeline_widget, SIGNAL( animationRunning(bool) ),
	         ui->button_playstop, SLOT( setChecked(bool) ) );
	connect( ui->speed_slider, SIGNAL( valueChanged(double) ),
	         ui->timeline_widget, SLOT( setAnimationRate(double) ) );
	connect( ui->timeline_widget, SIGNAL( animationRateChanged(double) ),
	         ui->speed_slider, SLOT( setValueSilent(double) ) );	

	connect( ui->gl_widget->getScenario(), SIGNAL( showLog() ),
	         SteamshovelApp::instance(), SLOT( startLogViewer() ) );

	ui->scenario_widget->setScenario( ui->gl_widget->getScenario() );
	ui->timeline_widget->reset(); // causes time extents information to reach the gl widget

	// load window-specific settings
	QSettings settings;
	restoreGeometry( settings.value("geometry/mainwindow").toByteArray() );
	ui->vertical_splitter->restoreState( settings.value("geometry/vertical_splitter").toByteArray() );
	restoreState( settings.value("window/state").toByteArray() );
	fullscreen_warning_shown_ = settings.value( "window/fullscrwarn" ).toBool();
	tv_mode_warning_shown_ = settings.value( "window/tvwarn" ).toBool();
	file_directory_ = settings.value( "window/file_directory" ).toString();
	session_directory_ = settings.value( "window/session_directory" ).toString();

	ui->speed_slider->setRange( 1e2, 2e4, 199, 0 );
	ui->speed_slider->setSpinBoxRange( 1.0, 1e9 );
	ui->speed_slider->setValue(
		settings.value( "animation_rate",
		                ui->timeline_widget->getAnimationRate() ).toDouble()
	);

	perhapsShowUpdates();

	this->setFocusPolicy( Qt::ClickFocus );
 	// set keyboard focus to FramePicker widget
	ui->frame_picker->setFocus();

	if( objectName() != "window" ){
		// this is a secondary window; adapt title, deactivate filter,
		// and make it stand on top of the original window
		setWindowTitle( windowTitle() + QString(": ") + objectName() );
		setWindowFlags( Qt::WindowStaysOnTopHint );
		ui->scenario_widget->disableFilter();
		ui->frame_picker->hide();
		handleFrameChanged( fileService->currentFrame(), 
		                    fileService->currentIndex(),
		                    fileService->frameIndex()[fileService->currentIndex()] );
	}
}

ShovelMainWindow::~ShovelMainWindow() {
	// only save settings of original main window
	if( objectName() == "window" ){
		log_debug( "Saving settings of ShovelMainWindow" );

		QSettings settings;
		settings.setValue( "geometry/mainwindow", saveGeometry() );
		settings.setValue( "geometry/vertical_splitter",
		                   ui->vertical_splitter->saveState() );
		settings.setValue( "window/state", saveState() );
		settings.setValue( "window/fullscrwarn", fullscreen_warning_shown_ );
		settings.setValue( "window/tvwarn", tv_mode_warning_shown_ );
		settings.setValue( "window/file_directory",
		                   file_directory_ );
		settings.setValue( "window/frame_save_directory",
		                   frame_save_directory_ );
		settings.setValue( "window/session_directory",
		                   session_directory_ );
		settings.setValue( "animation_rate", ui->speed_slider->value() );

		writeSession();

		// allow artists to perform cleanup actions
		ui->gl_widget->getScenario()->clear();
	}

	delete ui;
	delete mengine_;
}

void ShovelMainWindow::closeEvent( QCloseEvent* event ){
	if( objectName() == "window" )
		SteamshovelApp::instance()->quit();
	QMainWindow::closeEvent( event );
}

QString ShovelMainWindow::getScenarioCode() const {
	using namespace boost::python;
	scripting::ScopedGIL gil;
	QString code;
	try{
		object st = import( "icecube.steamshovel.util.scenariotools" );
		object dump_obj = st.attr( "exectableScenarioDump" )();
		code = extract<QString>( dump_obj );
	}
	catch( error_already_set& e ){
		log_error_stream( "Problem generating scenario code:\n" << e );
		code.clear();
	}
	return code;
}

bool ShovelMainWindow::loadScenarioCode(const QString& code) {
	bool scenario_okay = false;
	if( !code.isEmpty() ){
		scripting::ScopedGIL gil;
		try{
			SteamshovelApp::instance()->console().executeString( code.toStdString() );
			scenario_okay = true;
		}
		catch( boost::python::error_already_set& e ){
			log_error_stream( "Problem executing scenario code:\n"
					          << e << "\n"
					          << "String was:\n"
					          << code.toStdString() );
		}
	}
	return scenario_okay;
}

void ShovelMainWindow::perhapsShowUpdates() {
	// report about important updates that change workflow
	QSettings settings;
	const unsigned last_revision = settings.value( "last_revision" ).toUInt();

	if (last_revision == SHOVEL_REVISION)
		return;

	QString updates;
	{
		using namespace boost::python;
		scripting::ScopedGIL gil;
		try{
			object mod = import( "icecube.steamshovel.util.updates" );
			object s = mod.attr( "updatesSince" )( last_revision );
			updates = extract<QString>( s );
		}
		catch( error_already_set& e ){
			log_error_stream( "Problem getting update reports:\n" << e );
		}
	}

	if( !updates.isEmpty() ){
		// make a non-model dialog, so that users can
		// read about and try new features in parallel
		QDialog* d = new QDialog( this );
		Ui_UpdateDialog ui;
		ui.setupUi( d );
		ui.textBrowser->setText( updates );
		d->show();
		d->raise();
		d->activateWindow();
	}

	// remember current revision for next time
	settings.setValue( "last_revision", SHOVEL_REVISION );
}

void ShovelMainWindow::setBatchMode( bool b ){
	mengine_->setGuiProgress( !b );
}

int ShovelMainWindow::getActiveView()const{
	return ui->stackedWidget->currentIndex();
}

void ShovelMainWindow::setActiveView( int index ){
	// need to call doSwitchView so that everything is updated
	QList<QAction*> lst = ui->menuWindow->actions();
	const int begin = lst.indexOf( ui->action3D_View );
	const int end = lst.indexOf( ui->actionShow_python_prompt );
	index += begin;
	if( begin <= index && index < end ){
		doSwitchView( lst[index] );
	}else
		log_error( "view index is out of valid range" );
}

void ShovelMainWindow::captureActiveView( const std::string& filename ) {
	if( getActiveView() < 2 ){
		I3GLWidget* gl = ui->gl_widget;
		QSize s = gl->size();
		int dpi = gl->referenceDpi();
		double scale = 1.;
		double gamma = 1.;
		gl->screenshot_engine( s.width(), s.height(), filename, dpi, scale, gamma);
	}
	else {
		QWidget* w = ui->stackedWidget->currentWidget();
		QPixmap pixmap( w->size() );
		w->render( &pixmap );
		pixmap.save( QString::fromStdString(filename) );
	}
}

void ShovelMainWindow::handleWidgetArtistAdded( QString name, QWidget* widget ){
	log_debug_stream( "WidgetArtist " << name.toStdString() << " added" );
	ui->stackedWidget->addWidget( widget );
	QAction* a = new QAction( name, widget );
	a->setData( QVariant::fromValue( widget ) );
	a->setCheckable( true );
	ui->menuWindow->insertAction( ui->actionShow_python_prompt, a );
	view_group_->addAction( a );
	doSwitchView( a );
}

void ShovelMainWindow::handleWidgetArtistRemoved( QWidget* widget ){
	log_debug( "WidgetArtist removed" );
	doSwitchView( ui->action3D_View );
	ui->stackedWidget->removeWidget( widget );
}

void ShovelMainWindow::readSession(){
	if ( session_read_ )
		return;
	log_debug( "reading Session" );

	ArtistRegistry::global().restoreExternallyLoadedArtists();

	QSettings settings;
	QString scenario_code = settings.value( "scenario/dump" ).toString();
	if( !loadScenarioCode(scenario_code) ){
		log_debug( "Could not use scenario/dump, starting with empty scenario" );
		doSessionClear();
	}
	session_read_ = true;
}

void ShovelMainWindow::writeSession(){
	log_debug( "writing Session" );

	ArtistRegistry::global().saveExternallyLoadedArtists();

	QString scenario_code = getScenarioCode();
#if QT_VERSION >= 0x050000
	log_debug_stream( "Scenario dump at quit:\n" << scenario_code.toLatin1().data() );
#else
	log_debug_stream( "Scenario dump at quit:\n" << scenario_code.toAscii().data() );
#endif

	QSettings settings;
	if( !scenario_code.isEmpty() ) // empty code is an error
		settings.setValue( "scenario/dump", scenario_code );
}

void ShovelMainWindow::initScripting( const scripting::PyConsole& console )
{
	scripting::QMeta::exportObject( ui->gl_widget );
	scripting::QMeta::addToParent( this, ui->gl_widget, "gl" );
	scripting::QMeta::exportObject( ui->timeline_widget );
	scripting::QMeta::addToParent( this, ui->timeline_widget, "timeline" );
	scripting::QMeta::exportObject( mengine_ );
	scripting::QMeta::addToParent( this, mengine_, "movieEngine" );
	scripting::QMeta::exportObject( ui->scenario_widget->getFilter() );
	scripting::QMeta::addToParent( this, ui->scenario_widget->getFilter(), "frame_filter" );

	ui->actionShow_python_prompt->setEnabled( false );

	if( objectName() == "window" ){
		if( console.type() == scripting::PyConsole::IPython_widget ){
			console.embedInto( ui->shell_widget );
			ui->actionShow_python_prompt->setEnabled( true );
		}
	}
}

void ShovelMainWindow::handleFileListChanged(){
	QStringList openFiles = SteamshovelApp::instance()->files().openFiles();

	QList< QAction* > actions = ui->menuFile->actions();

	// menu structure: [other stuff], separator, [files], quit

	// remove all actions between separator and quit, find pos of separator
	int isep = actions.size();
	for( int i = 0; i < actions.size() - 1; ++i ){
		QAction* a = actions[i];
		if( a->isSeparator() )
			isep = i;
		if( i > isep )
			ui->menuFile->removeAction( a );
	}

	// add file actions
	QAction* fileAction = NULL;
	for( int i = 0; i < openFiles.size(); ++i ){
		QString path = openFiles[i];
		int pos = path.lastIndexOf( '/' );
		if( pos != -1 )
			path.remove( 0, pos + 1 );
		fileAction = new QAction( path, ui->menuFile );
		fileAction->setEnabled( false );
		ui->menuFile->insertAction( ui->actionQuit, fileAction );

                if (  i == openFiles.size()-1) {
                  setWindowFilePath(path);
                  setWindowTitle("SteamShovel - "+path);
                }                             
	}

	if( fileAction ){
		// enable last
		fileAction->setEnabled( true );
		connect( fileAction, SIGNAL(triggered()),
		         &(SteamshovelApp::instance()->files()),
		         SLOT(closeLastFile()) );
	}else{
		// openFiles was empty
		frames_info_->setText("0 frames");
	}
}

void ShovelMainWindow::handleNewSegment( unsigned start,
                                         unsigned stop,
                                         bool complete ){
	QString s = QString("%1 frames").arg( stop );
	if( !complete )
		s += "...";
	frames_info_->setText( s );
}

void ShovelMainWindow::handleFrameChanged( I3FramePtr frame,
                                           unsigned idx,
                                           const FrameInfo& finfo ){
	log_debug( "switch to frame %u", idx );
	// we want to control in which order and under what conditions
	// widgets are updated, so all frame updating is done here
	scripting::addDataToNamespace( frame, "frame" );
	if( frame ){
		QString s = QString( "All %1 frames" ).arg( frame->GetStop().id() );
		ui->loopallx_button->setText( s );
	}else{
		ui->loopallx_button->setText( "All X frames" );
	}

	// first update frame picker, so that user gets immediate feedback
	dynamic_cast<FramePickerScene*>(ui->frame_picker->scene())
	    ->frameUpdate( frame, idx, finfo );
	statusBar()->showMessage( "Updating..." );
	// Do not call QCoreApplication::processEvents() in here :(
	// otherwise two calls to change frames may be processed in
	// parallel, wrecking all kinds of havok

	// only update the display that is active
	switch( ui->stackedWidget->currentIndex() ){
	case 0:{ // GLWidget
		log_debug("update GLWidget");
		{
			// update timeline, but prevent superfluous update of GLWidget
			SignalBlocker b( ui->gl_widget,
			                 SIGNAL(cameraMoved()),
			                 SLOT(updateGL()) );
			ui->timeline_widget->frameUpdate( frame );
		}

		// update Scenario, which finally triggers update of GLWidget
		ui->gl_widget->getScenario()->setFrame( frame );

	} break;
	case 1:{ // FrameTable
		log_debug("update FrameTable");
		// update FrameTableView
		frame_table_model_->frameUpdate( frame );
		// update Scenario, but not GLWidget
		SignalBlocker b( ui->gl_widget,
		                 SIGNAL(cameraMoved()),
		                 SLOT(updateGL()) );
		ui->gl_widget->getScenario()->setFrame( frame );
		// do not update TimelineWidget
	} break;
	default:{ // WidgetArtist
		log_debug("update WidgetArtist");
		// prevent superfluous GLWidget updates
		SignalBlocker b( ui->gl_widget,
		                 SIGNAL(cameraMoved()),
		                 SLOT(updateGL()) );
		// update timeline and scenario, which updates WidgetArtist
		ui->gl_widget->getScenario()->setFrame( frame );
		ui->timeline_widget->frameUpdate( frame );
	}
	}

	// update FocusView
	updateFocusMenu( frame );

	if( idx != unsigned(-1) )
		statusBar()->showMessage( QString( "Frame %1" ).arg( idx ) );
	else{
		statusBar()->showMessage( "No frame loaded" );
	}

	// update FilterWidget feedback
	ui->scenario_widget->getFilter()->processFrame();
}

void ShovelMainWindow::handleLockCamera( bool lock ){
	if( lock ){
		ui->rotationButton->setChecked( false );
	}
	ui->rotationButton->setDisabled( lock );
	ui->gl_widget->setCameraLock( lock );
}

void ShovelMainWindow::doFileOpen(){
	QStringList filenames = QFileDialog::getOpenFileNames(
	    this, "Choose i3 files...",
	    file_directory_,
	    "I3 files (*.i3 *.i3.bz2 *.i3.gz)" );
	if( filenames.isEmpty() ){
		log_trace( "User canceled file select" );
		return;
	}

	QString last = filenames.back();
	file_directory_ = last.left( last.lastIndexOf("/") );
	BOOST_FOREACH( QString filename, filenames ){
#if QT_VERSION >= 0x050000
		log_debug( "Opening: %s", filename.toLatin1().constData() );
#else
		log_debug( "Opening: %s", filename.toAscii().constData() );
#endif
		SteamshovelApp::instance()->files().openLocalFile( filename.toStdString() );
	}
}

void ShovelMainWindow::doFrameSave(){
	FileService& fs = SteamshovelApp::instance()->files();
	if( !fs.currentFrame() )
		return;

	SaveFrameDialog d( this );
	if( d.exec() == QDialog::Rejected )
		return;

	QString filename = d.getSaveFileName();
	if( filename.isEmpty() )
		return;

	std::set<unsigned> indices = d.getIndices();
	if( indices.empty() )
		indices.insert( fs.currentIndex() );

	log_debug_stream( "Writing " << indices.size() << " "
	                  "frames to: " << filename.toStdString() );

	fs.saveFrames( filename, indices, d.saveParents() );
}

void ShovelMainWindow::doSessionOpen(){
	QString filename =
		QFileDialog::getOpenFileName(
            this, "Load a session...",
			session_directory_,
			"Sessions (*.py)" );
	if( filename.isEmpty() ){
		log_trace( "User canceled session select" );
		return;
	}

	session_directory_ = filename.left( filename.lastIndexOf("/") );
	doSessionOpen( filename );
}

void ShovelMainWindow::doSessionOpen( QString filename ){
#if QT_VERSION >= 0x050000
	log_debug("Loading session from: %s", filename.toLatin1().data() );
#else
	log_debug("Loading session from: %s", filename.toAscii().data() );
#endif

	QFile file( filename );
	if( file.open(QIODevice::ReadOnly | QIODevice::Text) ){
		QString save = getScenarioCode();
		if( !save.isEmpty() ){
			doSessionClear();
			QString code = file.readAll();
			file.close();
			bool scenario_okay = loadScenarioCode( code );
			if( scenario_okay ){
				ui->menuSession->addToRecentList( filename );
				return;
			} else {
				doSessionClear();
				loadScenarioCode( save );
			}
		}
	}
	log_error( "Error loading session" );
}

void ShovelMainWindow::doSessionSave(){
	QString filename =
		QFileDialog::getSaveFileName( this, "Save session...",
									  session_directory_,
									  "Sessions (*.py)" );
	if( filename.isEmpty() ){
		log_trace( "User canceled saving the session" );
		return;
	}
	// if .py extension is missing, add it
	if (!filename.endsWith(".py"))
		filename += ".py";

	session_directory_ = filename.left( filename.lastIndexOf("/") );
#if QT_VERSION >= 0x050000
	log_debug( "Saving session to: %s", filename.toLatin1().data() );
#else
	log_debug( "Saving session to: %s", filename.toAscii().data() );
#endif

	QString code = getScenarioCode();
	if( !code.isEmpty() ){
		QFile file( filename );
		if( file.open( QIODevice::WriteOnly | QIODevice::Text ) ){
#if QT_VERSION >= 0x050000
			file.write( code.toLatin1() );
#else
			file.write( code.toAscii() );
#endif
			file.close();
			ui->menuSession->addToRecentList( filename );
			return;
		}
	}
	log_error( "Session could not be saved" );
}

void ShovelMainWindow::doSessionClear(){
	ui->gl_widget->resetCamera();
	ui->gl_widget->setCameraLock( false );
	ui->gl_widget->setPerspective( true );
	ui->scenario_widget->getFilter()->setCode( QString() );
	ui->gl_widget->getScenario()->clear();
}

void ShovelMainWindow::doToggleFullscreen(){
	if( windowState() & Qt::WindowFullScreen ){
		showNormal();
	}
	else{
		if( !fullscreen_warning_shown_ ){
			QDialog msg;
			Ui_FullscreenWarning ui;
			ui.setupUi( &msg );
			msg.exec();
			if( ui.checkBox->isChecked() ){
				fullscreen_warning_shown_ = true;
			}
		}
		showFullScreen();
	}
}

void ShovelMainWindow::doNewWindow(){
	SteamshovelApp::instance()->newWindow();
}

void ShovelMainWindow::doToggleTVMode(){
	if( !tv_mode_warning_shown_ ){
		QDialog msg;
		Ui_TVModeWarning ui;
		ui.setupUi( &msg );
		msg.exec();
		if( ui.checkBox->isChecked() ){
			tv_mode_warning_shown_ = true;
		}
	}
	ui->gl_widget->toggleTVMode();
}

void ShovelMainWindow::doProjectionDialog(){
	WithRotationDisabled disable( ui->gl_widget );
	ProjectionDialog d( ui->gl_widget );
	d.exec();
	return;
}

void ShovelMainWindow::doScreenshotDialog(){
	QString filename = QFileDialog::getSaveFileName(
	    this, "Choose a path for the screenshot...",
	    QString("ShovelScreen.png"),
	    "Image Files (*.png *.jpg)");
	if( filename.isEmpty() ){
		log_trace( "User canceled saving the screenshot" );
		return;
	}
	captureActiveView( filename.toStdString() );
}

void ShovelMainWindow::doMovieDialog(){
	WithRotationDisabled disabled( ui->gl_widget );
	WithPlayPaused paused( ui->timeline_widget );
	MovieDialog d(ui->gl_widget, ui->timeline_widget->getTimeWindow(), mengine_ );
	if( d.exec() == QDialog::Accepted ){
		mengine_->produce();
	}
}

void ShovelMainWindow::doConfigDialog(){
	ConfigDialog d( this );
	d.exec();
}

void ShovelMainWindow::doHelpDialog(){
	using namespace boost::python;
	scripting::ScopedGIL gil;
	try{
		object mod = import( "icecube.steamshovel.util.help" );

		if( !mod.attr( "show" )() ){
			std::ostringstream msg;
			msg << "Documentation is missing. To build it, do\n"
			       "\n"
			       "  make html\n"
			       "\n"
			       "in the build directory of this software installation.";
			QMessageBox::warning( this, "Steamshovel Help",
			                      msg.str().c_str() );
		}
	}
	catch( error_already_set& e ){
		log_error_stream( "caught Python exception:\n" << e );
	}
}

void ShovelMainWindow::doSwitchView( QAction* sender ){
	sender->setChecked( true );

	// if current view is a WidgetArtist, deactive it
	if( ui->stackedWidget->currentWidget() != ui->view3D &&
	    ui->stackedWidget->currentWidget() != ui->viewSpreadsheet ){
		ui->stackedWidget->currentWidget()->setEnabled( false );
	}

	if( sender == ui->action3D_View ){
		log_debug( "switching to 3D" );
		ui->stackedWidget->setCurrentWidget( ui->view3D );
		if( ui->timeline_toggle->isChecked() )
			ui->timeline_widget->show();
	} else
	if( sender == ui->actionSpreadsheet_View ){
		log_debug( "switching to FrameTable" );
		ui->stackedWidget->setCurrentWidget( ui->viewSpreadsheet );
		ui->timeline_widget->hide();
	} else { // WidgetArtist
		log_debug( "switching to WidgetArtist" );
		QWidget* w = sender->data().value<QWidget*>();
		w->setEnabled( true );
		ui->stackedWidget->setCurrentWidget( w );
		if( ui->timeline_toggle->isChecked() )
			ui->timeline_widget->show();
	}
	FileService& fs = SteamshovelApp::instance()->files();
	handleFrameChanged( fs.currentFrame(), fs.currentIndex(),
	                    fs.frameIndex()[fs.currentIndex()] );
}

void ShovelMainWindow::doShellToggle( bool on ){
	ui->actionShow_python_prompt->setChecked( on );
	ui->controls->setCurrentIndex( int(on) );
}

void ShovelMainWindow::doExtras( bool on ){
	if( on )
		statusBar()->showMessage("The scroll opens and reveils extra content", 5000);
	else
		statusBar()->showMessage("The scroll remains closed", 5000);
}

void ShovelMainWindow::showEvent( QShowEvent* ev ){
	QMainWindow::showEvent( ev );
	ui->timeline_widget->reset();
	if( objectName() == "window" ){
		readSession();
	}
}

void ShovelMainWindow::keyPressEvent( QKeyEvent* ev ){
	if( ev->matches( QKeySequence::Close ) ){
		// QCoreApplication::processEvents();
		close();
	}

	switch( ev->key() ){

	// send unhandled arrow events to FramePicker
	case Qt::Key_Left:
	case Qt::Key_Right:
	case Qt::Key_Up:
	case Qt::Key_Down:
		ui->frame_picker->keyPressEvent( ev );
		break;

	// send unhandled Key_<num> to GLWidget
	case Qt::Key_0:
	case Qt::Key_1:
	case Qt::Key_2:
		ui->gl_widget->keyPressEvent( ev );
		break;

	case Qt::Key_Escape:
		if( windowState() & Qt::WindowFullScreen ){
			log_debug( "Escaping to normal window" );
			showNormal();
		}
		break;

	default:
		QMainWindow::keyPressEvent( ev );
		break;
	}
}

void ShovelMainWindow::focusOn( QAction* action ){
	QVariant datum = action->data();
	if( !datum.canConvert<vec3d>() ){
		log_error( "Bad focusOn action!" );
		return;
	}
	ui->gl_widget->focusCameraOn( datum.value<vec3d>() );
}

void ShovelMainWindow::handleSessionAction( QAction* action ){
#if QT_VERSION >= 0x050000
	log_debug( "Handling action: %s",
	           action->text().toLatin1().data() );
#else
	log_debug( "Handling action: %s",
	           action->text().toAscii().data() );
#endif

	if( action == ui->actionOpen_session ){
		doSessionOpen();
		return;
	}

	if( action == ui->actionSave_session ){
		doSessionSave();
		return;
	}

	if ( action == ui->actionClear_session ){
		doSessionClear();
		return;
	}

	// action holds recent or standard session
	QString filename = action->data().toString();
	doSessionOpen( filename );
}

void ShovelMainWindow::handleCameraViewAction( QAction* action ){
#if QT_VERSION >= 0x050000
	log_debug( "Handling action: %s",
	           action->text().toLatin1().data() );
#else
	log_debug( "Handling action: %s",
	           action->text().toAscii().data() );
#endif

	const vec3d loc_old = ui->gl_widget->getCameraLoc();
	const vec3d orig = ui->gl_widget->getCameraPivot();
	const double d = (loc_old - orig).length();

	vec3d orientation( 0, 0, 0 ); // "right" of camera
	vec3d loc_new = orig;
	if( action == ui->actionViewYZ ){
		orientation.setY( 1 );
		loc_new.setX( loc_new.x() + d );
	}else if( action == ui->actionViewXZ ){
		orientation.setX( 1 );
		loc_new.setY( loc_new.y() - d );
	}else if( action == ui->actionViewXY ){
		orientation.setX( 1 );
		loc_new.setZ( loc_new.z() + d );
	}else{ assert( !"should never happen" ); }

	ui->gl_widget->setCameraLoc( loc_new );
	ui->gl_widget->setCameraOrientation( orientation );
}

void ShovelMainWindow::updateFocusMenu( I3FramePtr frame ){
	QMenu* menu = ui->menuFocus_on;
	while( menu->actions().size() > focus_menu_original_size_ ){
		menu->removeAction( menu->actions().last() );
	}
	if( !frame )
		return;

	std::vector< std::string > keys = frame->keys();
	namespace bp = boost::python;
	scripting::ScopedGIL gil;
	bp::object mod = bp::import( "icecube.steamshovel.util.camerafocus" );
	bp::object fun = mod.attr( "getFocusPoint" );

	BOOST_FOREACH( const std::string& key, keys ){
		try{
			bp::object ret = fun( frame, key );
			vec3d pt = bp::extract<vec3d>( ret );
			QAction* a = menu->addAction( QString::fromStdString(key) );
			a->setData( QVariant(pt) );
		}
		catch( bp::error_already_set& e ){
			// if either the getFocusPoint function or the extraction fails, just skip this key
			PyErr_Clear();
		}
	}
}
