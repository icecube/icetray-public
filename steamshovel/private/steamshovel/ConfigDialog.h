#ifndef I3_SHOVEL_CONFIG_DIALOG_H
#define I3_SHOVEL_CONFIG_DIALOG_H

#include "ui_config_dialog.h"
#include <QDialog>

#include <dataclasses/geometry/I3Geometry.h>

namespace Ui{ class ConfigDialog; }
class QWidget;

class ConfigDialog : public QDialog {

	Q_OBJECT;
	Ui::ConfigDialog ui;
	I3GeometryConstPtr geo_;

public:
	ConfigDialog( QWidget* );
	virtual ~ConfigDialog() {}

public Q_SLOTS:
	void writeValues() const;
	void updateGeometryText();
	void loadGeometry();
	void clearGeometry();
};

#endif
