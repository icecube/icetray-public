#include "ConfigDialog.h"
#include "moc_ConfigDialog.cpp"

#include "SteamshovelApp.h"
#include "FileService.h"
#include "ShovelLogger.h"

#include <shovelio/File.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Logging.h>

#include <QFileDialog>
#include <QStringList>
#include <QString>
#include <QCoreApplication>

#include <boost/foreach.hpp>

ConfigDialog::ConfigDialog( QWidget* parent ) :
	QDialog( parent )
{
	ui.setupUi( this );
	const FileService& fs = SteamshovelApp::instance()->files();

	// setup config
	ui.spinBox_backward->setValue( fs.getNBackward() );
	ui.spinBox_forward->setValue( fs.getNForward() );
	ui.edit_ignore_keys->setText( fs.getFilters().join(";") );
	ui.fontComboBox->setCurrentFont( QFont() );
	ui.spinBox_fontSize->setValue( QFont().pointSize() );
	geo_ = fs.defaultGeometry();
	updateGeometryText();
	const ShovelLogger* sl = static_cast<const ShovelLogger*>(
		GetIcetrayLogger().get()
	);
	ui.checkBox_open_logviewer->setChecked(
		sl->autoStartViewer() 
	);

    connect( ui.pushButton_I3Geometry,
    	     SIGNAL(clicked()),
    	     SLOT(loadGeometry()) );

    connect( this,
    	     SIGNAL(accepted()),
             SLOT(writeValues()) );

    connect( ui.pushButton_I3Geometry_Clear,
             SIGNAL(clicked()),
             SLOT(clearGeometry()) );
}

void
ConfigDialog::writeValues() const {
	const QStringList skipKeys = ui.edit_ignore_keys->text().split(";");
	FileService& fs = SteamshovelApp::instance()->files();
	ShovelLogger* sl = static_cast<ShovelLogger*>(
		GetIcetrayLogger().get()
	);

	bool files_need_reload = 
		fs.defaultGeometry() != geo_ ||
		fs.getFilters() != skipKeys;

	fs.setNForward( ui.spinBox_forward->value() );
	fs.setNBackward( ui.spinBox_backward->value() );
	fs.setFilters( skipKeys );
	QFont font = ui.fontComboBox->currentFont();
	font.setPointSize( ui.spinBox_fontSize->value() );
	SteamshovelApp::instance()->setFont( font );	
	fs.setDefaultGeometry( geo_ );
	sl->setAutoStartViewer( ui.checkBox_open_logviewer->isChecked() );

	if( files_need_reload )
		fs.reloadAllFiles();
}

void
ConfigDialog::updateGeometryText(){
	if( geo_ ){
		int string_max = 0;
		BOOST_FOREACH( const I3OMGeoMap::value_type& p,
		               geo_->omgeo ){
			string_max = std::max( p.first.GetString(), string_max );
		}
		ui.lineEdit_I3Geometry->setText(
			QString("IC%1: %2 DOMs, %3 stations")
			  .arg( string_max )
			  .arg( geo_->omgeo.size() )
			  .arg( geo_->stationgeo.size() ) );
	}else{
		ui.lineEdit_I3Geometry->clear();
	}
}

void
ConfigDialog::loadGeometry(){
	QString filename = QFileDialog::getOpenFileName( 0,
		"Open a file with an I3Geometry object",
		QString(),
		"I3 files (*.i3 *.i3.gz *.i3.bz2)"
	);
	if (filename.isEmpty())
		return;
	ui.lineEdit_I3Geometry->setText( "Loading..." );
	QCoreApplication::processEvents();
	shovelio::raw::File f( filename.toStdString() );
	I3FramePtr frame( new I3Frame );
	geo_.reset();
	const shovelio::raw::FilterList none;
	while( !geo_ &&
		   f.Next( frame, false, none ) ){
		geo_ = frame->Get<I3GeometryConstPtr>();
	}
	if( !geo_ )
		log_error( "could not load an I3Geometry object" );
	updateGeometryText();
}

void
ConfigDialog::clearGeometry(){
	geo_.reset();
	updateGeometryText();
}
