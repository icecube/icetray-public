#include "SaveFrameDialog.h"
#include "moc_SaveFrameDialog.cpp"

#include <icetray/I3Logging.h>

#include "scripting/gil.h"
#include "scripting/pyerr.h"

#include <QSettings>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QRegExpValidator>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include <boost/python.hpp>

SaveFrameDialog::SaveFrameDialog( QWidget* parent ) :
	QFileDialog( parent,
	             "Choose a path for an i3 file...",
	             QString(),
	             "I3 files (*.i3 *.i3.gz *.i3.bz2 *.i3.zst);;All files (*)" ),
	lineedit_( new QLineEdit( this ) ),
	checkbox_( new QCheckBox( "Save parents", this ) )
{
	setAcceptMode( QFileDialog::AcceptSave );
	QSettings settings;
	restoreState( settings.value("saveframedialog/state").toByteArray() );

	lineedit_->setPlaceholderText( "leave empty to save current frame" );
	QRegExpValidator* validator = new QRegExpValidator( QRegExp( "[0-9\\-, ]*" ), this );
	lineedit_->setValidator( validator );
	checkbox_->setChecked( settings.value("saveframedialog/save_parents", true).toBool() );

	QGridLayout* mainlayout = dynamic_cast<QGridLayout*>( layout() );
	if( mainlayout && mainlayout->columnCount() == 3 ){
		const int nrow = mainlayout->rowCount();
		QLabel* label = new QLabel( "Frame indices to save:", this );
		mainlayout->addWidget( label, nrow, 0 );
		QHBoxLayout* hl = new QHBoxLayout;
		hl->addWidget( lineedit_ );
		hl->addWidget( checkbox_ );
		mainlayout->addLayout( hl, nrow, 1, 1, 2 );
	}else
		log_error( "QFileDialog has unexpected layout, please update class SaveFrameDialog" );
}

SaveFrameDialog::~SaveFrameDialog()
{
	QSettings settings;
	settings.setValue( "saveframedialog/state", saveState() );
	settings.setValue( "saveframedialog/save_parents", checkbox_->isChecked() );
}

QString SaveFrameDialog::getSaveFileName() const
{
	QStringList files = selectedFiles();
	if( files.empty() )
		return QString();

	QString filename = files[0];
	// add the .i3.gz extension if no valid extension is provided
	if( !filename.endsWith(".i3") && !filename.endsWith(".i3.gz") 
	    && !filename.endsWith(".i3.bz2") && !filename.endsWith(".i3.zst") && !filename.endsWith(".i3.zstd")){
		filename += ".i3.gz";
	}
	return filename;
}

std::set<unsigned> SaveFrameDialog::getIndices() const
{
	scripting::ScopedGIL gil;
	using namespace boost::python;

	std::set<unsigned> indices;
	try{ 
		object module = import("icecube.steamshovel.util.parse_indices");
		object numbers = module.attr( "parse_indices" )( lineedit_->text() );

		for( int i = 0, n = len(numbers); i < n; ++i ){
			object num = numbers[i];
			unsigned idx = extract<unsigned>( num );
			indices.insert( idx );
		}
	}
	catch( error_already_set& e ){
		log_error_stream( "Got a Python exception:\n" << e );
	}
	return indices;
}
