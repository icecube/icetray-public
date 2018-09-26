#include "scripting/gil.h"

#include <QGraphicsScene>
#include <QFileDialog>
#include <QSettings>

#include "I3GLWidget.h"
#include "ProjectionDialog.h"
#include "moc_ProjectionDialog.cpp"

#include "SteamshovelApp.h"
#include "FileService.h"

ProjectionDialog::ProjectionDialog( I3GLWidget* gl ) :
	QDialog(),
	gl_( gl )
{
	ui.setupUi( this );

	QSize maxdim = gl_->maxScreenshotDims();
	ui.widthSpinner->setMaximum( maxdim.width() );
	ui.heightSpinner->setMaximum( maxdim.height() );

	ui.dpiSpinner->setValue( gl->referenceDpi() );

	QSettings settings;
	ui.dpiSpinner->setValue(
	    settings.value("screenshot/dpi",
	                   ui.dpiSpinner->value() ).toInt() 
	);
	ui.widthSpinner->setValue(
	    settings.value("screenshot/width",
	                   ui.widthSpinner->value() ).toInt()
	);
	ui.heightSpinner->setValue(
	    settings.value("screenshot/height",
	                   ui.heightSpinner->value() ).toInt()
	);
	ui.gammaSpinner->setValue(
	    settings.value("screenshot/gamma",
	                   ui.gammaSpinner->value() ).toDouble()
	);
	ui.includeXYZProjection->setChecked(
	    settings.value("screenshot/includeXYZProjection",
	                   ui.includeXYZProjection->isChecked()).toBool()
	);
	ui.includeColorscale->setChecked(
	    settings.value("screenshot/includeColorscale",
	                   ui.includeColorscale->isChecked()).toBool()
	);

	connect( ui.dpiSpinner, SIGNAL(valueChanged(int)), SLOT(rescale()) );
	connect( ui.widthSpinner, SIGNAL(valueChanged(int)), SLOT(rescale()) );
	connect( ui.heightSpinner, SIGNAL(valueChanged(int)), SLOT(rescale()) );
	connect( ui.gammaSpinner, SIGNAL(valueChanged(double)), SLOT(rescale()) );

	rescale();
}

ProjectionDialog::~ProjectionDialog()
{
	QSettings settings;
	settings.setValue( "screenshot/dpi", ui.dpiSpinner->value() );
	settings.setValue( "screenshot/width", ui.widthSpinner->value() );
	settings.setValue( "screenshot/height", ui.heightSpinner->value() );
	settings.setValue( "screenshot/gamma", ui.gammaSpinner->value() );
	settings.setValue( "screenshot/includeXYZProjection", ui.includeXYZProjection->isChecked() );
	settings.setValue( "screenshot/includeColorscale", ui.includeColorscale->isChecked() );
}

bool ProjectionDialog::isAvailable() {
	using namespace boost::python;
	scripting::ScopedGIL gil;
	try {
		import( "icecube.steamshovel.util.projection" );
		return true;
	} catch(error_already_set& e) {
		PyErr_Clear();
		log_debug("could not import icecube.steamshovel.util.projection");
		return false;
	}
}

void ProjectionDialog::accept(){
	int w = ui.widthSpinner->value();
	int h = ui.heightSpinner->value();
	int dpi = ui.dpiSpinner->value();
	double scale = ui.scaleSpinner->value();
	double gamma = ui.gammaSpinner->value();
	// obey the convention that an unmodified dpi means no dpi setting is intended
	if( dpi == gl_->referenceDpi() )
		dpi = 0;
	QString fileName = QFileDialog::getSaveFileName( this, "Save event projection",
	                                                 "ShovelScreen.png",
	                                                 "Images (*.png)" );
	if( fileName.length() ) {
		using namespace boost::python;
		scripting::ScopedGIL gil;
		I3FramePtr frame = SteamshovelApp::instance()->files().currentFrame();

		// We don't need to try/catch the import() here
		// ProjectionDialog::isAvailable() takes care of it
		// for us, and if it fails we'll never get here anyway
		
		object projectionutils = import( "icecube.steamshovel.util.projection" );
		bool do_xyz = ui.includeXYZProjection->isChecked();
		bool do_colorscale = ui.includeColorscale->isChecked();
		try {
			projectionutils.attr( "get_projection" )(
					fileName.toStdString(), frame, do_xyz, do_colorscale, w, h, dpi, scale, gamma
			);
			QDialog::accept();
		} catch (error_already_set& e) {
			if(PyErr_Occurred()) {
				PyErr_Print();
			}
			PyErr_Clear();
			log_error("icecube.steamshovel.util.projection.get_projection() failed");
			QDialog::reject();
		}
	}
}

void ProjectionDialog::rescale(){
	if ( !ui.manualScaling->isChecked() ) {
		ui.scaleSpinner->setValue( double(ui.dpiSpinner->value()) / gl_->referenceDpi() );
	}
}

