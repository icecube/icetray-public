#ifndef I3_SHOVEL_SCREENSHOT_DIALOG_H
#define I3_SHOVEL_SCREENSHOT_DIALOG_H

#include <QDialog>

#include "ui_screenshot.h"

namespace Ui{ class ProjectionDialog; }
class I3GLWidget;
class QGraphicsScene;

class ProjectionDialog : public QDialog {

Q_OBJECT;

public:
	ProjectionDialog( I3GLWidget* gl );
	~ProjectionDialog();

	static bool isAvailable();

public Q_SLOTS:
	void rescale();
	void accept();

private:
	Ui::ProjectionDialog ui;
	I3GLWidget* gl_;
};

#endif /* I3_SHOVEL_SCREENSHOT_DIALOG_H */
